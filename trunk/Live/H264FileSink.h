#ifndef BXC_RTSPSERVER_H264FILESINK_H
#define BXC_RTSPSERVER_H264FILESINK_H

#include <stdint.h>
#include "Sink.h"



class H264FileSink : public Sink
{
public:
    /**
   * ����һ���µ�H264FileSink����
   * @param env UsageEnvironment����ָ��
   * @param mediaSource MediaSource����ָ��
   * @return H264FileSink����ָ��
   */
    static H264FileSink* createNew(UsageEnvironment* env, MediaSource* mediaSource);

    H264FileSink(UsageEnvironment* env, MediaSource* mediaSource);
    virtual ~H264FileSink();
    virtual std::string getMediaDescription(uint16_t port);
    virtual std::string getAttribute();
    virtual void sendFrame(MediaFrame* frame);

private:
    RtpPacket mRtpPacket; // RTP���ݰ�����
    int mClockRate; // ʱ��Ƶ��
    int mFps; // ֡��
};

#endif //BXC_RTSPSERVER_H264FILESINK_H