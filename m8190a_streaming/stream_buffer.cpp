#include "StdAfx.h"
#include "stream_buffer.h"

using namespace std;

stream_buffer::stream_buffer(int segmentSize, uint circularBufSize)
    :CircularBufferSize(circularBufSize),
    wfBuffer(NULL) {
    hMutex = CreateMutex(NULL, FALSE, NULL);

    bufferSize = segmentSize * 2;

    InitBuffer();
}

stream_buffer::~stream_buffer(void) {
    DeleteBuffer();
    CloseHandle(hMutex);
}

void stream_buffer::DeleteBuffer() {
    if (wfBuffer != NULL) {

        for (unsigned int index = 0; index < CircularBufferSize; index++) {
            wfBuffer[index]->Destroy();
            delete wfBuffer[index];
            wfBuffer[index] = NULL;
        }

        delete[] wfBuffer;
        wfBuffer = NULL;
    }
}

void stream_buffer::Clear() {
    InitBuffer();
}

void stream_buffer::InitBuffer() {
    DeleteBuffer();

    wfBuffer = new CComSafeArray<short>*[CircularBufferSize];

    for (unsigned int index = 0; index < CircularBufferSize; index++) {
        wfBuffer[index] = new CComSafeArray<short>(bufferSize);
    }

    writeIndex = 0;
    readIndex = 0;
    count = 0;

    isReading = false;
    isWriting = false;
}

void stream_buffer::Enqueue(short* wfData, int length) {
    WaitForSingleObject(hMutex, INFINITE);

    short* pData = NULL;
    HRESULT hr = SafeArrayAccessData(*(wfBuffer[writeIndex]->GetSafeArrayPtr()), (void**)&pData);

    if (!SUCCEEDED(hr)) {
        throw _com_error(hr);
    }

    memcpy((void*)pData, wfData, length * 2);

    SafeArrayUnaccessData(*(wfBuffer[writeIndex]->GetSafeArrayPtr()));

    count++;
    writeIndex++;

    if (writeIndex >= CircularBufferSize) writeIndex = 0;

    ReleaseMutex(hMutex);
}

void stream_buffer::IsReading(bool value) {
    WaitForSingleObject(hMutex, INFINITE);

    isReading = value;

    if (!isReading) {
        readIndex++;

        if (readIndex >= CircularBufferSize) readIndex = 0;

        count--;
    }

    ReleaseMutex(hMutex);
}

LPSAFEARRAY* stream_buffer::GetReadBuffer() const {
    if (isReading && wfBuffer[readIndex] != NULL) {
        return wfBuffer[readIndex]->GetSafeArrayPtr();
    }

    return NULL;
}

void stream_buffer::IsWriting(bool value) {
    WaitForSingleObject(hMutex, INFINITE);

    isWriting = value;

    if (!isWriting) {
        count++;
        writeIndex++;

        if (writeIndex >= CircularBufferSize) writeIndex = 0;
    }

    ReleaseMutex(hMutex);
}

LPSAFEARRAY* stream_buffer::GetWriteBuffer() const {
    if (isWriting && wfBuffer[writeIndex] != NULL) {
        return wfBuffer[writeIndex]->GetSafeArrayPtr();
    }

    return NULL;
}

int stream_buffer::Capacity() const {
    return CircularBufferSize;
}

int stream_buffer::Count() const {
    return count;
}
