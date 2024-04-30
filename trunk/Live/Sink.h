#ifndef _RTSPSERVER_SINK_H
#define _RTSPSERVER_SINK_H
#include <string>
#include <stdint.h>

#include "Rtp.h"
#include "MediaSource.h"
#include "../Scheduler/Event.h"
#include "../Scheduler/UsageEnvironment.h"


class Sink
{
public:
    enum PacketType
    {
        UNKNOWN = -1,
        RTPPACKET = 0,
    };

    typedef void (*SessionSendPacketCallback)(void* arg1, void* arg2, void* packet, PacketType packetType);

    Sink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType);
    virtual ~Sink();

    void stopTimerEvent();

    virtual std::string getMediaDescription(uint16_t port) = 0;
    virtual std::string getAttribute() = 0;

    void setSessionCb(SessionSendPacketCallback cb,void* arg1, void* arg2);

protected:

    virtual void sendFrame(MediaFrame* frame) = 0;
    void sendRtpPacket(RtpPacket* packet);
    // 定时执行函数，传入间隔时间
    void runEvery(int interval);
private:

    static void cbTimeout(void* arg);
    void handleTimeout();

protected:
    UsageEnvironment* mEnv;// UsageEnvironment对象指针
    MediaSource* mMediaSource;// MediaSource对象指
    SessionSendPacketCallback mSessionSendPacket;// 会话回调函数指针
   void* mArg1; // 回调函数参数1
    void* mArg2; // 回调函数参数2

    uint8_t mCsrcLen; // CSRC长度
    uint8_t mExtension; // 扩展标志
    uint8_t mPadding; // 填充标志
    uint8_t mVersion; // 版本号
    uint8_t mPayloadType; // 负载类型
    uint8_t mMarker; // 标记位
    uint16_t mSeq; // 序列号
    uint32_t mTimestamp; // 时间戳
    uint32_t mSSRC; // SSRC标识符

private:
    TimerEvent* mTimerEvent;// 定时器事件对象指针
    Timer::TimerId mTimerId;// runEvery()之后获取// 定时器ID，用于runEvery()之后获取
};

#endif //BXC_RTSPSERVER_SINK_H