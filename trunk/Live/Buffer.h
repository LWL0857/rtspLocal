#ifndef BXC_RTSPSERVER_BUFFER_H
#define BXC_RTSPSERVER_BUFFER_H
#include <stdlib.h>
#include <algorithm>
#include <stdint.h>
#include <assert.h>

class Buffer
{
public:
	static const int initialSize;// 初始大小

	explicit Buffer();
	~Buffer();

	int readableBytes() const//写到的位置和读到的位置之差，也就是剩下还可以读取的字节数
	{
		return mWriteIndex - mReadIndex;
	}

	int writableBytes() const//可写缓冲字节的大小
	{
		return mBufferSize - mWriteIndex;
	}

	int prependableBytes() const // 可预留字节数
	{
		return mReadIndex;
	}

	char* peek()//返回可写的位置的指针
	{
		return begin() + mReadIndex;
	}

	const char* peek() const// 返回可读数据的指针（const版本）
	{
		return begin() + mReadIndex;
	}

	const char* findCRLF() const// 查找第一个CRLF（回车换行）的位置
	{
		const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
		return crlf == beginWrite() ? NULL : crlf;
	}

	const char* findCRLF(const char* start) const// 从指定位置开始查找第一个CRLF的位置
	{
		assert(peek() <= start);
		assert(start <= beginWrite());
		const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
		return crlf == beginWrite() ? NULL : crlf;
	}

	const char* findLastCrlf() const// 查找最后一个CRLF的位置
	{
		const char* crlf = std::find_end(peek(), beginWrite(), kCRLF, kCRLF + 2);
		return crlf == beginWrite() ? NULL : crlf;
	}
	void retrieveReadZero() { // 重置读索引
		// 自定义函数（本次读取的数据不全，已读取的数据归零，重新从头读取）
		mReadIndex = 0;
	}

	void retrieve(int len)// 读取指定长度的数据
	{
		assert(len <= readableBytes());
		if (len < readableBytes())
		{
			mReadIndex += len;
		}
		else
		{
			retrieveAll();
		}
	}

	void retrieveUntil(const char* end)// 读取直到指定位置的数据
	{
		assert(peek() <= end);
		assert(end <= beginWrite());
		retrieve(end - peek());
	}

	void retrieveAll() // 重置读写索引 
	{
		mReadIndex = 0;
		mWriteIndex = 0;
	}

	char* beginWrite()
	{
		return begin() + mWriteIndex;
	}

	const char* beginWrite() const // 返回可写数据的指针
	{
		return begin() + mWriteIndex;
	}

	void unwrite(int len)// 取消写入指定长度的数据
	{
		assert(len <= readableBytes());
		mWriteIndex -= len;
	}

	/* 确保有足够的空间 */
	void ensureWritableBytes(int len)
	{
		if (writableBytes() < len)
		{
			makeSpace(len);
		}
		assert(writableBytes() >= len);
	}

	void makeSpace(int len)// 调整空间大小
	{
		if (writableBytes() + prependableBytes() < len) //如果剩余空间不足
		{
			/* 扩大空间 */
			mBufferSize = mWriteIndex + len;
			mBuffer = (char*)realloc(mBuffer, mBufferSize);
		}
		else //剩余空间足够
		{
			/* 移动内容 */
			int readable = readableBytes();
			std::copy(begin() + mReadIndex,
				begin() + mWriteIndex,
				begin());
			mReadIndex = 0;
			mWriteIndex = mReadIndex + readable;
			assert(readable == readableBytes());
		}
	}

	void append(const char* data, int len)
	{
		ensureWritableBytes(len); //调整扩大的空间
		std::copy(data, data + len, beginWrite()); //拷贝数据

		assert(len <= writableBytes());
		mWriteIndex += len;//重新调节写位置
	}

	void append(const void* data, int len) // 追加数据（重载版本）
	{
		append((const char*)(data), len);
	}

	int read(int fd);
	int write(int fd);

private:
	char* begin()// 返回缓冲区的起始位置
	{
		return mBuffer;
	}

	const char* begin() const
	{
		return mBuffer;
	}

private:
	char* mBuffer;
	int mBufferSize;// 缓冲区大小
	int mReadIndex; // 当前
	int mWriteIndex;// 当前从socket实际读取到的字节长度

	static const char* kCRLF;// CRLF字符串
};

#endif //BXC_RTSPSERVER_BUFFER_H