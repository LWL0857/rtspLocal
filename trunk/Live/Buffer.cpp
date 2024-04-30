#include "Buffer.h"
#include "../Scheduler/SocketsOps.h"

const int Buffer::initialSize = 1024;
const char* Buffer::kCRLF = "\r\n";

Buffer::Buffer() :
    mBufferSize(initialSize),
    mReadIndex(0),
    mWriteIndex(0)
{
    mBuffer = (char*)malloc(mBufferSize);
}

Buffer::~Buffer()
{
    free(mBuffer);
}


int Buffer::read(int fd)
{
    char extrabuf[65536];
    const int writable = writableBytes();
    //先把数据接收到extrabuf里
    const int n = ::recv(fd, extrabuf, sizeof(extrabuf), 0);
    if (n <= 0) {
        return -1;
    }
    else if (n <= writable)
    {
        //如果接收到的数据的大小小于buffer可写的字节数
        //将数据拷贝到buffer里,并且移动写索引
        std::copy(extrabuf, extrabuf + n, beginWrite()); //拷贝数据
        mWriteIndex += n;

    }
    else
    {
        //buffer写不下extrabuffer读到的数据时，先把buffer写满,然后把剩余的数据，
        // 通过append函数扩大buffer，然后copy到buffer的尾部，
        //并且调整写索引。
        std::copy(extrabuf, extrabuf + writable, beginWrite()); //拷贝数据
        mWriteIndex += writable;
        append(extrabuf+ writable, n - writable);
    }
    return n;
}

int Buffer::write(int fd)
{
    return sockets::write(fd, peek(), readableBytes());
}