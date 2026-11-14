#pragma once


class CPacket
{
protected:
	int m_iBufferSize;
	int m_iDataSize;

public:

	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1400
	};

	CPacket();
	CPacket(int iBufferSize);
	virtual ~CPacket();
	void Clear();
	int GetBufferSize(void) { return m_iBufferSize; }
	int GetDataSize(void) { return m_iDataSize; }
	char* GetBufferPtr(void) { return m_chpBuffer; }
	int MoveWritePos(int iSize);
	int MoveReadPos(int iSize);

	CPacket& operator = (CPacket& clSrcPacket);

	//////////////////////////////////////////////////////////////////////////
	// 넣기.	각 변수 타입마다 모두 만듬.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator << (unsigned char byValue);
	CPacket& operator << (char chValue);

	CPacket& operator << (short shValue);
	CPacket& operator << (unsigned short wValue);

	CPacket& operator << (int iValue);
	CPacket& operator << (unsigned int iValue);
	CPacket& operator << (long lValue);
	CPacket& operator << (float fValue);

	CPacket& operator << (double dValue);
	CPacket& operator << (unsigned long long lValue);
	CPacket& operator << (long long lValue);

	//////////////////////////////////////////////////////////////////////////
	// 빼기.	각 변수 타입마다 모두 만듬.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator >> (char& chValue);
	CPacket& operator >> (unsigned char& byValue);

	CPacket& operator >> (short& shValue);
	CPacket& operator >> (unsigned short& wValue);

	CPacket& operator >> (int& iValue);
	CPacket& operator >> (long& lValue);
	CPacket& operator >> (float& fValue);

	CPacket& operator >> (double& dValue);
	CPacket& operator >> (bool& bValue);
	CPacket& operator >> (unsigned long long& lValue);
	CPacket& operator >> (long long& lValue);


	//////////////////////////////////////////////////////////////////////////
	// 데이타 얻기.
	//
	// Parameters: (char *)Dest 포인터. (int)Size.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		GetData(char* chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// 데이타 삽입.
	//
	// Parameters: (char *)Src 포인터. (int)SrcSize.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		PutData(char* chpSrc, int iSrcSize);


private:
	char* m_chpBuffer;
	char* frontPtr;
	char* rearPtr;
};