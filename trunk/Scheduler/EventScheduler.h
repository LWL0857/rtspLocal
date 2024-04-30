#ifndef BXC_RTSPSERVER_EVENTSCHEDULER_H
#define BXC_RTSPSERVER_EVENTSCHEDULER_H

#include <vector>
#include <queue>
#include <mutex>
#include <stdint.h>
#include "Timer.h"
#include "Event.h"
class Poller;

class EventScheduler
{
public:
    enum PollerType
    {
        POLLER_SELECT,
        POLLER_POLL,
        POLLER_EPOLL
    };
    static EventScheduler* createNew(PollerType type);

    explicit EventScheduler(PollerType type);
    virtual ~EventScheduler();
public:
    bool addTriggerEvent(TriggerEvent* event);//添加触发事件。
    Timer::TimerId addTimedEventRunAfater(TimerEvent* event, Timer::TimeInterval delay); //加延迟执行的定时事件。
    Timer::TimerId addTimedEventRunAt(TimerEvent* event, Timer::Timestamp when); //添加在指定时间执行的定时事件。
    Timer::TimerId addTimedEventRunEvery(TimerEvent* event, Timer::TimeInterval interval); //添加循环执行的定时事件。
    bool removeTimedEvent(Timer::TimerId timerId); //移除定时事件。
    bool addIOEvent(IOEvent* event); //添加IO事件。
    bool updateIOEvent(IOEvent* event); //更新IO事件。
    bool removeIOEvent(IOEvent* event);

    void loop(); //启动事件循环。
    //    void wakeup();
    Poller* poller(); //获取底层的轮询器对象。
    void setTimerManagerReadCallback(EventCallback cb, void* arg); //设置定时器管理器的读回调函数。

private:
    void handleTriggerEvents();

private:
    bool mQuit;
    Poller* mPoller;
    TimerManager* mTimerManager;
    std::vector<TriggerEvent*> mTriggerEvents;

    std::mutex mMtx;

    // WIN系统专用的定时器回调start
    EventCallback mTimerManagerReadCallback;
    void* mTimerManagerArg;
    // WIN系统专用的定时器回调end
};

#endif //BXC_RTSPSERVER_EVENTSCHEDULER_H