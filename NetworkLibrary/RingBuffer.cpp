#include "RingBuffer.h"
#include <iostream>

#define DEFAULT_SIZE 4096

CRingBuffer::CRingBuffer() : buffer(new char[DEFAULT_SIZE + 1]), internalSize(DEFAULT_SIZE + 1), capacity(DEFAULT_SIZE), readPos(buffer), writePos(buffer)
{
}

CRingBuffer::CRingBuffer(int iBufferSize) : buffer(new char[iBufferSize + 1]), internalSize(iBufferSize + 1), capacity(iBufferSize), readPos(buffer), writePos(buffer)
{

}

CRingBuffer::~CRingBuffer()
{
	if (buffer != nullptr)
	{
		delete[] buffer;
		buffer = nullptr;
	}
}

int CRingBuffer::GetBufferSize()
{
	return capacity;
}

int CRingBuffer::getUseSize()
{
	if (writePos - readPos < 0)
	{
		return (writePos - readPos) + internalSize;
	}

	return (writePos - readPos);
}

int CRingBuffer::getFreeSize()
{
	return capacity - getUseSize();
}

int CRingBuffer::Enqueue(const char* chpData, int iSize)
{
	if (iSize <= 0 || chpData == nullptr || iSize > getFreeSize())
	{
		std::wcout << L"Ring Buffer Size lack " << std::endl;
		return 0;
	}

	char* nextPos = writePos;
	int spaceToEnd = (buffer + internalSize) - nextPos;

	if (iSize < spaceToEnd)
	{
		memcpy(nextPos, chpData, iSize);
		nextPos += iSize;
	}
	else if (iSize == spaceToEnd)
	{
		memcpy(nextPos, chpData, iSize);
		nextPos = buffer;
	}
	else
	{
		memcpy(nextPos, chpData, spaceToEnd);
		memcpy(buffer, chpData + spaceToEnd, iSize - (spaceToEnd));
		nextPos = buffer + iSize - (spaceToEnd);
	}

	writePos = nextPos;
	return iSize;

}

int CRingBuffer::Dequeue(char* chpDest, int iSize)
{
	if (iSize > getUseSize())
	{
		std::wcout << L"DEQUEUE GET USE SIZE" << std::endl;
		return 0;
	}

	char* nextPos = readPos;
	int spaceToEnd = (buffer + internalSize) - nextPos;

	if (iSize < spaceToEnd)
	{
		memcpy(chpDest, nextPos, iSize);
		nextPos += iSize;
	}
	else if (iSize == spaceToEnd)
	{
		memcpy(chpDest, nextPos, iSize);
		nextPos = buffer;
	}
	else
	{
		memcpy(chpDest, nextPos, spaceToEnd);
		memcpy(chpDest + spaceToEnd, buffer, iSize - (spaceToEnd));
		nextPos = buffer + iSize - (spaceToEnd);
	}

	readPos = nextPos;
	return iSize;

}

int CRingBuffer::Peek(char* chpDest, int iSize)
{
	if (iSize > getUseSize())
	{
		std::wcout << L"PEEK GET USE SIZE" << std::endl;
		return 0;
	}

	char* nextPos = readPos;
	int spaceToEnd = (buffer + internalSize) - nextPos;

	if (iSize < spaceToEnd)
	{
		memcpy(chpDest, nextPos, iSize);
		nextPos += iSize;
	}
	else if (iSize == spaceToEnd)
	{
		memcpy(chpDest, nextPos, iSize);
		nextPos = buffer;
	}
	else
	{
		memcpy(chpDest, nextPos, spaceToEnd);
		memcpy(chpDest + spaceToEnd, buffer, iSize - (spaceToEnd));
		nextPos = buffer + iSize - (spaceToEnd);
	}

	return iSize;
}

void CRingBuffer::ClearBuffer(void)
{
	writePos = buffer;
	readPos = buffer;
}



int CRingBuffer::DirectEnqueueSize(void)
{
	int spaceToEnd = (buffer + internalSize) - writePos;
	int freeSize = getFreeSize();

	return (freeSize < spaceToEnd) ? freeSize : spaceToEnd;
}



int CRingBuffer::DirectDequeueSize(void)
{
	if (writePos - readPos >= 0)
	{
		return getUseSize();
	}
	else
	{
		return (buffer + internalSize) - readPos;
	}
}



int CRingBuffer::MoveRear(int iSize)
{
	if (iSize > getFreeSize())
	{
		return 0;
	}

	char* nextPos = writePos;
	nextPos = writePos + iSize;
	if (nextPos >= buffer + internalSize)
	{
		nextPos -= internalSize;
	}

	writePos = nextPos;

	return iSize;
}

int CRingBuffer::MoveFront(int iSize)
{
	if (iSize > getUseSize())
	{
		return 0;
	}

	char* nextPos = readPos;
	nextPos = readPos + iSize;
	if (nextPos >= buffer + internalSize)
	{
		nextPos -= internalSize;
	}

	readPos = nextPos;

	return iSize;

}

char* CRingBuffer::GetFrontBufferPtr(void)
{
	return readPos;
}

char* CRingBuffer::GetRearBufferPtr(void)
{
	return writePos;
}
