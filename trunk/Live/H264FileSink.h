#ifndef BXC_RTSPSERVER_H264FILESINK_H
#define BXC_RTSPSERVER_H264FILESINK_H

#include <stdint.h>
#include "Sink.h"



class H264FileSink : public Sink
{
public:
    /**
   * 创建一个新的H264FileSink对象
   * @param env UsageEnvironment对象指针
   * @param mediaSource MediaSource对象指针
   * @return H264FileSink对象指针
   */
    static H264FileSink* createNew(UsageEnvironment* env, MediaSource* mediaSource);

    H264FileSink(UsageEnvironment* env, MediaSource* mediaSource);
    virtual ~H264FileSink();
    virtual std::string getMediaDescription(uint16_t port);
    virtual std::string getAttribute();
    virtual void sendFrame(MediaFrame* frame);

private:
    RtpPacket mRtpPacket; // RTP数据包对象
    int mClockRate; // 时钟频率
    int mFps; // 帧率
};

#endif //BXC_RTSPSERVER_H264FILESINK_H