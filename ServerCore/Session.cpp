#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
#include "IocpEvent.h"

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

auto Session::Send(std::shared_ptr<SendBuffer> sendBuffer) -> void
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);
		if (_sendRegisterd.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
	{
		RegisterSend();
	}
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
		ProcessSend(numOfBytes);
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
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();
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

auto Session::RegisterSend() -> void
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this(); // WSASend가 끝나기전까지 메모리해제되지 않도록 RefCount + 1

	// 보낼 데이터를 SendEvent에 등록
	{
		WRITE_LOCK;

		int32 writeSize{ 0 };
		while (_sendQueue.empty() == false)
		{
			std::shared_ptr<SendBuffer> sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO : 예외 체크

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather
	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (std::shared_ptr<SendBuffer> sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes{ 0 };
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr; // Ref Count -1
			_sendEvent.sendBuffers.clear(); // Ref Count -1
			_sendRegisterd.store(false);
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

	// 컨텐츠 코드에서 구현
	OnDisconnected();
	GetService()->ReleaseSession(GetSession());
}

auto Session::ProcessRecv(int32 numOfBytes) -> void
{
	_recvEvent.owner = nullptr; // Ref Count -1

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();

	// 컨텐츠 코드에서 구현
	int32 processLength = OnRecv(_recvBuffer.ReadPos(), dataSize);
	if (processLength < 0 || dataSize < processLength || _recvBuffer.OnRead(processLength) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	_recvBuffer.Clean();

	// 수신 등록
	RegisterRecv();
}

auto Session::ProcessSend(int32 numOfBytes) -> void
{
	_sendEvent.owner = nullptr; // Ref Count -1
	_sendEvent.sendBuffers.clear(); // Ref Count -1

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 구현
	OnSend(numOfBytes);

	WRITE_LOCK;
	if (_sendQueue.empty())
	{
		_sendRegisterd.store(false);
	}
	else
	{
		RegisterSend();
	}
}

auto Session::HandleError(int32 errorCode) -> void
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : 로그 구체화
		std::cout << "Handle Error : " << errorCode << std::endl;
		break;
	}
}

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

auto PacketSession::GetPacketSession() -> std::shared_ptr<PacketSession>
{
	return static_pointer_cast<PacketSession>(shared_from_this());
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLength = 0;

	while (true)
	{
		int32 dataSize = len - processLength;

		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[0]));
		if (dataSize < header.size)
			break;

		OnRecvPacket(&buffer[0], header.size);
		processLength += header.size;
	}

	return processLength;
}
