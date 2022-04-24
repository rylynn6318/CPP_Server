#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(std::shared_ptr<SendBufferChunk> owner, BYTE* buffer, int32 allocSize)
	: _owner(owner), _buffer(buffer), _allocSize(allocSize)
{
}

auto SendBuffer::Buffer() -> BYTE*
{
	return _buffer;
}

auto SendBuffer::WriteSize() -> int32
{
	return _writeSize;
}

auto SendBuffer::Close(uint32 writeSize) -> void
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);
}

SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

auto SendBufferChunk::Reset() -> void
{
	_open = false;
	_usedSize = 0;
}

auto SendBufferChunk::Open(uint32 allocSize) -> std::shared_ptr<SendBuffer>
{
	ASSERT_CRASH(SEND_BUFFER_CHUNK_SIZE >= allocSize);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

auto SendBufferChunk::Close(uint32 writeSize) -> void
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;
}

auto SendBufferChunk::IsOpen() -> bool
{
	return _open;
}

auto SendBufferChunk::Buffer() -> BYTE*
{
	return &_buffer[_usedSize];
}


auto SendBufferChunk::FreeSize() -> uint32
{
	return static_cast<uint32>(_buffer.size() - _usedSize);
}


auto SendBuffManager::Open(uint32 size) -> std::shared_ptr<SendBuffer>
{
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 다쓰면 버리고 새거로 교체
	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	std::cout << "FREE : " << LSendBufferChunk->FreeSize() << std::endl;

	return LSendBufferChunk->Open(size);
}

auto SendBuffManager::Push(std::shared_ptr<SendBufferChunk> buffer) -> void
{
	WRITE_LOCK;
	_sendBufferChunk.push_back(buffer);
}

auto SendBuffManager::Pop() -> std::shared_ptr<SendBufferChunk>
{
	{
		WRITE_LOCK;
		if (_sendBufferChunk.empty() == false)
		{
			std::shared_ptr<SendBufferChunk> sendBufferChunk = _sendBufferChunk.back();
			_sendBufferChunk.pop_back();
			return sendBufferChunk;
		}
	}
	return std::shared_ptr<SendBufferChunk>(xnew<SendBufferChunk>(), PushGlobal);
}

auto SendBuffManager::PushGlobal(SendBufferChunk* buffer) -> void
{
	GSendBufferManager->Push(std::shared_ptr<SendBufferChunk>(buffer, PushGlobal));
}
