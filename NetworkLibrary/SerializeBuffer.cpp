#include <windows.h>
#include "SerializeBuffer.h"

CPacket::CPacket() : m_chpBuffer(new char[eBUFFER_DEFAULT]), frontPtr(m_chpBuffer), rearPtr(m_chpBuffer), m_iBufferSize(eBUFFER_DEFAULT), m_iDataSize(0)
{
}

CPacket::CPacket(int iBufferSize) :m_chpBuffer(new char[iBufferSize]), frontPtr(m_chpBuffer), rearPtr(m_chpBuffer), m_iBufferSize(iBufferSize), m_iDataSize(0)
{
}

CPacket::~CPacket()
{
	if (m_chpBuffer != nullptr)
	{
		delete[] m_chpBuffer;
	}
}

void CPacket::Clear()
{
	frontPtr = m_chpBuffer;
	rearPtr = m_chpBuffer;
	m_iDataSize = 0;
}

int CPacket::MoveWritePos(int iSize)
{
	if (rearPtr + iSize > m_chpBuffer + m_iBufferSize)
	{
		return 0;
	}
	rearPtr += iSize;
	m_iDataSize += iSize;
	return iSize;
}

int CPacket::MoveReadPos(int iSize)
{
	if (frontPtr + iSize > rearPtr) {
		return 0;
	}
	frontPtr += iSize;
	m_iDataSize -= iSize;
	return iSize;
}

CPacket& CPacket::operator=(CPacket& clSrcPacket)
{
	if (this == &clSrcPacket) return *this;

	if (rearPtr + clSrcPacket.GetDataSize() > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	memcpy(m_chpBuffer, clSrcPacket.m_chpBuffer, clSrcPacket.GetDataSize());
	frontPtr = clSrcPacket.frontPtr;
	rearPtr = clSrcPacket.rearPtr;
	m_iBufferSize = clSrcPacket.m_iBufferSize;
	m_iDataSize = clSrcPacket.m_iDataSize;

	return *this;
}

CPacket& CPacket::operator<<(unsigned char byValue)
{
	if (rearPtr + sizeof(unsigned char) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(unsigned char*)rearPtr = byValue;
	rearPtr += sizeof(unsigned char);
	m_iDataSize += sizeof(unsigned char);

	return *this;

}

CPacket& CPacket::operator<<(char chValue)
{
	if (rearPtr + sizeof(char) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*rearPtr = chValue;
	rearPtr += sizeof(char);
	m_iDataSize += sizeof(char);

	return *this;
}

CPacket& CPacket::operator<<(short shValue)
{
	if (rearPtr + sizeof(short) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(short*)rearPtr = shValue;
	rearPtr += sizeof(short);
	m_iDataSize += sizeof(short);

	return *this;
}

CPacket& CPacket::operator<<(unsigned short wValue)
{
	if (rearPtr + sizeof(unsigned short) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(unsigned short*)rearPtr = wValue;
	rearPtr += sizeof(unsigned short);
	m_iDataSize += sizeof(unsigned short);

	return *this;
}

CPacket& CPacket::operator<<(int iValue)
{
	if (rearPtr + sizeof(int) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(int*)rearPtr = iValue;
	rearPtr += sizeof(int);
	m_iDataSize += sizeof(int);

	return *this;
}


CPacket& CPacket::operator<<(unsigned int iValue)
{
	if (rearPtr + sizeof(unsigned int) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(unsigned int*)rearPtr = iValue;
	rearPtr += sizeof(unsigned int);
	m_iDataSize += sizeof(unsigned int);

	return *this;
}


CPacket& CPacket::operator<<(long lValue)
{
	if (rearPtr + sizeof(long) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(long*)rearPtr = lValue;
	rearPtr += sizeof(long);
	m_iDataSize += sizeof(long);

	return *this;
}

CPacket& CPacket::operator<<(float fValue)
{
	if (rearPtr + sizeof(float) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(float*)rearPtr = fValue;
	rearPtr += sizeof(float);
	m_iDataSize += sizeof(float);

	return *this;
}

CPacket& CPacket::operator<<(double dValue)
{
	if (rearPtr + sizeof(double) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(double*)rearPtr = dValue;
	rearPtr += sizeof(double);
	m_iDataSize += sizeof(double);

	return *this;
}

CPacket& CPacket::operator<<(unsigned long long lValue)
{
	if (rearPtr + sizeof(unsigned long long) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(unsigned long long*)rearPtr = lValue;
	rearPtr += sizeof(unsigned long long);
	m_iDataSize += sizeof(unsigned long long);

	return *this;
}

CPacket& CPacket::operator<<(long long lValue)
{
	if (rearPtr + sizeof(long long) > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return *this;
	}

	*(long long*)rearPtr = lValue;
	rearPtr += sizeof(long long);
	m_iDataSize += sizeof(long long);

	return *this;
}

CPacket& CPacket::operator>>(char& chValue)
{
	if (frontPtr + sizeof(char) > rearPtr)
	{
		// over buff
		return *this;
	}

	chValue = *(char*)frontPtr;
	frontPtr += sizeof(char);
	m_iDataSize -= sizeof(char);
	return *this;
}

CPacket& CPacket::operator>>(unsigned char& byValue)
{
	if (frontPtr + sizeof(unsigned char) > rearPtr)
	{
		// over buff
		return *this;
	}

	byValue = *(unsigned char*)frontPtr;
	frontPtr += sizeof(unsigned char);
	m_iDataSize -= sizeof(unsigned char);

	return *this;
}

CPacket& CPacket::operator>>(short& shValue)
{
	if (frontPtr + sizeof(short) > rearPtr)
	{
		// over buff
		return *this;
	}

	shValue = *(short*)frontPtr;
	frontPtr += sizeof(short);
	m_iDataSize -= sizeof(short);

	return *this;
}

CPacket& CPacket::operator>>(unsigned short& wValue)
{
	if (frontPtr + sizeof(unsigned short) > rearPtr)
	{
		// over buff
		return *this;
	}

	wValue = *(unsigned short*)frontPtr;
	frontPtr += sizeof(unsigned short);
	m_iDataSize -= sizeof(unsigned short);

	return *this;
}

CPacket& CPacket::operator>>(int& iValue)
{
	if (frontPtr + sizeof(int) > rearPtr)
	{
		// over buff
		return *this;
	}

	iValue = *(int*)frontPtr;
	frontPtr += sizeof(int);
	m_iDataSize -= sizeof(int);

	return *this;
}

CPacket& CPacket::operator>>(long& lValue)
{
	if (frontPtr + sizeof(long) > rearPtr)
	{
		// over buff
		return *this;
	}

	lValue = *(long*)frontPtr;
	frontPtr += sizeof(long);
	m_iDataSize -= sizeof(long);

	return *this;
}

CPacket& CPacket::operator>>(float& fValue)
{
	if (frontPtr + sizeof(float) > rearPtr)
	{
		// over buff
		return *this;
	}

	fValue = *(float*)frontPtr;
	frontPtr += sizeof(float);
	m_iDataSize -= sizeof(float);

	return *this;
}


CPacket& CPacket::operator>>(double& dValue)
{
	if (frontPtr + sizeof(double) > rearPtr)
	{
		// over buff
		return *this;
	}

	dValue = *(double*)frontPtr;
	frontPtr += sizeof(double);
	m_iDataSize -= sizeof(double);

	return *this;
}

CPacket& CPacket::operator>>(bool& bValue)
{
	if (frontPtr + sizeof(bool) > rearPtr)
	{
		return *this;
	}

	bValue = *(bool*)frontPtr;
	frontPtr += sizeof(bool);
	m_iDataSize -= sizeof(bool);

	return *this;

}

CPacket& CPacket::operator>>(unsigned long long& lValue)
{
	if (frontPtr + sizeof(unsigned long long) > rearPtr)
	{
		// over buff
		return *this;
	}

	lValue = *(unsigned long long*)frontPtr;
	frontPtr += sizeof(unsigned long long);
	m_iDataSize -= sizeof(unsigned long long);

	return *this;
}

CPacket& CPacket::operator>>(long long& lValue)
{
	if (frontPtr + sizeof(long long) > rearPtr)
	{
		// over buff
		return *this;
	}

	lValue = *(long long*)frontPtr;
	frontPtr += sizeof(long long);
	m_iDataSize -= sizeof(long long);

	return *this;
}

int CPacket::GetData(char* chpDest, int iSize)
{
	if (frontPtr + iSize > rearPtr)
	{
		// over buff
		return 0;
	}
	memcpy(chpDest, frontPtr, iSize);
	frontPtr += iSize;
	m_iDataSize -= iSize;

	return iSize;
}

int CPacket::PutData(char* chpSrc, int iSrcSize)
{
	if (rearPtr + iSrcSize > m_chpBuffer + m_iBufferSize)
	{
		// no buff
		return 0;
	}
	memcpy(rearPtr, chpSrc, iSrcSize);
	rearPtr += iSrcSize;
	m_iDataSize += iSrcSize;

	return iSrcSize;
}


