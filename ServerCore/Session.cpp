#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "IocpEvent.h"

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

auto Session::Send(BYTE* buffer, int32 len) -> void
{
	// TEMP
	SendEvent* sendEvent = xnew<SendEvent>();
	sendEvent->owner = shared_from_this(); //Rec Count ++
	sendEvent->buffer.resize(len);
	::memcpy(sendEvent->buffer.data(), buffer, len);

	WRITE_LOCK;
	RegisterSend(sendEvent);
}

auto Session::Connect() -> bool
{
	return RegisterConnect();
}

auto Session::Disconnect(const WCHAR* cause) -> void
{
	if (_connected.exchange(false) == false)
		return;

	// TEMP
	std::wcout << "Disconnect :" << cause << std::endl;

	// 컨텐츠 코드에서 구현
	OnDisconnected();
	GetService()->ReleaseSession(GetSession());

	RegisterDisconnect();
}

auto Session::GetService() -> std::shared_ptr<Service>
{
	return _service.lock();
}

auto Session::SetService(std::shared_ptr<Service> service) -> void
{
	_service = service;
}

auto Session::SetNetAddress(NetAddress netAddress) -> void
{
	_netAddress = netAddress;
}

auto Session::GetNetAddress() -> NetAddress
{
	return _netAddress;
}

auto Session::GetSocket() -> SOCKET
{
	return _socket;
}

auto Session::IsConnected() -> bool
{
	return _connected;
}

auto Session::GetSession() -> std::shared_ptr<Session>
{
	return static_pointer_cast<Session>(shared_from_this());
}

auto Session::GetHandle() -> HANDLE
{
	return reinterpret_cast<HANDLE>(_socket);
}

auto Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) -> void
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
		break;
	default:
		break;
	}
}

auto Session::RegisterConnect() -> bool
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0 /* 여유 있는 포트 중 아무거나*/) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this(); // Rec Count +1;

	DWORD numOfBytes{ 0 };
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();

	if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr; // Rec Count -1
			return false;
		}
	}

	return true;
}

auto Session::RegisterDisconnect() -> bool
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this(); // Ref Count +1

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr; // Rec Count -1
			return false;
		}
	}

	return true;
}

auto Session::RegisterRecv() -> void
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); // WSARecv가 끝나기전까지 메모리해제되지 않도록 RefCount + 1

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	wsaBuf.len = len32(_recvBuffer);
	DWORD numOfBytes{ 0 };
	DWORD flags{ 0 };

	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // RefCount -1
		}
	}
}

auto Session::RegisterSend(SendEvent* sendEvent) -> void
{
	if (IsConnected() == false)
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD numOfBytes{ 0 };
	if (SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT & numOfBytes, 0, sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->owner = nullptr;
			xdelete(sendEvent);
		}
	}
}

auto Session::ProcessConnect() -> void
{
	_connectEvent.owner = nullptr; // Rec Count -1
	_connected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSession());

	// 컨텐츠 코드에서 구현
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

auto Session::ProcessDisconnect() -> void
{
	_disconnectEvent.owner = nullptr; // Rec Count -1
}

auto Session::ProcessRecv(int32 numOfBytes) -> void
{
	_recvEvent.owner = nullptr; // Ref Count -1

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	// 컨텐츠 코드에서 구현
	OnRecv(_recvBuffer, numOfBytes);

	// 수신 등록
	RegisterRecv();
}

auto Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes) -> void
{
	sendEvent->owner = nullptr; // Ref Count -1
	xdelete(sendEvent);

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 구현
	OnSend(numOfBytes);
}

auto Session::HandleError(int32 errorCode) -> void
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
	default:
		// TODO : 로그 구체화
		std::cout << "Handle Error : " << errorCode << std::endl;
		break;
	}
}
