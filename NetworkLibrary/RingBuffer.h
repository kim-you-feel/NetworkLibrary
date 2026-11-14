#pragma once

class CRingBuffer
{
private:
	char* buffer;
	int capacity;
	int internalSize;

	char* writePos;
	char* readPos;

public:
	CRingBuffer();
	CRingBuffer(int iBufferSize);
	~CRingBuffer();

	//void Resize();
	int GetBufferSize();
	int getUseSize();
	int getFreeSize();

	int Enqueue(const char* chpData, int iSize);
	int Dequeue(char* chpDest, int iSize);
	int Peek(char* chpDest, int iSize);
	void ClearBuffer(void);

	int	DirectEnqueueSize(void);
	int	DirectDequeueSize(void);

	int	MoveRear(int iSize);
	int	MoveFront(int iSize);

	char* GetFrontBufferPtr(void);
	char* GetRearBufferPtr(void);

};