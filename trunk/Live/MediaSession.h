#ifndef BXC_RTSPSERVER_MEDIASESSION_H
#define BXC_RTSPSERVER_MEDIASESSION_H
#include <string>
#include <list>

#include "RtpInstance.h"
#include "Sink.h"

#define MEDIA_MAX_TRACK_NUM 2




//该类用于管理媒体会话，
//包括添加数据生产者和消费者，
//生成SDP描述等功能。

class MediaSession
{
public:
    enum TrackId
    {
        TrackIdNone = -1,
        TrackId0    = 0,
        TrackId1    = 1,
    };

    static MediaSession* createNew(std::string sessionName);
    explicit MediaSession(const std::string& sessionName);
    ~MediaSession();

public:

    std::string name() const { return mSessionName; }
    std::string generateSDPDescription();
    bool addSink(MediaSession::TrackId trackId, Sink* sink);// 添加数据生产者

    bool addRtpInstance(MediaSession::TrackId trackId, RtpInstance* rtpInstance);// 添加数据消费者
    bool removeRtpInstance(RtpInstance* rtpInstance);// 删除数据消费者


    bool startMulticast();
    bool isStartMulticast();
    std::string getMulticastDestAddr() const { return mMulticastAddr; }
    uint16_t getMulticastDestRtpPort(TrackId trackId);

private:
    class Track {
    public:
        Sink* mSink;
        int mTrackId;
        bool mIsAlive;
        std::list<RtpInstance*> mRtpInstances;
    };
    // 获取指定轨道
    Track* getTrack(MediaSession::TrackId trackId);
    // 发送RTP包回调函数
    static void sendPacketCallback(void* arg1, void* arg2, void* packet,Sink::PacketType packetType);
    void handleSendRtpPacket(MediaSession::Track* tarck, RtpPacket* rtpPacket);



private:
    std::string mSessionName;
    std::string mSdp;// SDP描述
    Track mTracks[MEDIA_MAX_TRACK_NUM];
    bool mIsStartMulticast;
    std::string mMulticastAddr;
    RtpInstance* mMulticastRtpInstances[MEDIA_MAX_TRACK_NUM];
    RtcpInstance* mMulticastRtcpInstances[MEDIA_MAX_TRACK_NUM];
};
#endif //BXC_RTSPSERVER_MEDIASESSION_H