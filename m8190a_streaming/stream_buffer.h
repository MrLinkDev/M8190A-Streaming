#pragma once

class stream_buffer {
public:

	stream_buffer(int segmentSize, uint circularBufSize);
	~stream_buffer(void);

	void Enqueue(short* wfData, int count);

	void IsReading(bool value);
	void IsWriting(bool value);

	LPSAFEARRAY* GetReadBuffer() const;
	LPSAFEARRAY* GetWriteBuffer() const;

	void Clear();
	int Capacity() const;
	int Count() const;

private:

	void InitBuffer();
	void DeleteBuffer();

private:

	uint CircularBufferSize;
	uint writeIndex;
	uint readIndex;
	int count;
	int bufferSize;
	bool isReading;
	bool isWriting;
	HANDLE hMutex;
	CComSafeArray<short>** wfBuffer;
};

