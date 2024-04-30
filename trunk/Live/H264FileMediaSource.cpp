#include "H264FileMediaSource.h"
#include "../Base/Log.h"
#include <fcntl.h>

static inline int startCode3(uint8_t* buf);
static inline int startCode4(uint8_t* buf);

H264FileMediaSource* H264FileMediaSource::createNew(UsageEnvironment* env, const std::string& file)
{
    return new H264FileMediaSource(env, file);
    //    return New<H264FileMediaSource>::allocate(env, file);
}

H264FileMediaSource::H264FileMediaSource(UsageEnvironment* env, const std::string& file) :
    MediaSource(env) {
    mSourceName = file;
    mFile = fopen(file.c_str(), "rb");
    setFps(25);

    for (int i = 0; i < DEFAULT_FRAME_NUM; ++i) {
        mEnv->threadPool()->addTask(mTask);
    }
}

H264FileMediaSource::~H264FileMediaSource()
{
    fclose(mFile);
}

void H264FileMediaSource::handleTask()
{
    std::lock_guard <std::mutex> lck(mMtx);

    if (mFrameInputQueue.empty())
        return;

    MediaFrame* frame = mFrameInputQueue.front();
    int startCodeNum = 0;

    while (true)
    {
        frame->mSize = getFrameFromH264File(frame->temp, FRAME_MAX_SIZE);
        if (frame->mSize < 0) {
            return;
        }
        if (startCode3(frame->temp)){
            startCodeNum = 3;
        }else{
            startCodeNum = 4;
        }
        frame->mBuf = frame->temp + startCodeNum;
        frame->mSize -= startCodeNum;

        uint8_t naluType = frame->mBuf[0] & 0x1F;
        //LOGI("startCodeNum=%d,naluType=%d,naluSize=%d", startCodeNum, naluType, frame->mSize);

        if (0x09 == naluType) {
            // discard the type byte
            continue;
        }
        else if (0x07 == naluType || 0x08 == naluType) {
            //continue;
            break;
        }
        else {
            break;
        }
    }

    mFrameInputQueue.pop();
    mFrameOutputQueue.push(frame);
}

static inline int startCode3(uint8_t* buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        return 1;
    else
        return 0;
}

static inline int startCode4(uint8_t* buf)
{
    if (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
        return 1;
    else
        return 0;
}

static uint8_t* findNextStartCode(uint8_t* buf, int len)
{
    int i;

    if (len < 3)
        return NULL;

    for (i = 0; i < len - 3; ++i)
    {
        if (startCode3(buf) || startCode4(buf))
            return buf;

        ++buf;
    }

    if (startCode3(buf))
        return buf;

    return NULL;
}
//从h264文件中取得一帧数据
int H264FileMediaSource::getFrameFromH264File(uint8_t* frame, int size)
{
    // 检查文件是否打开，如果没有打开则返回-1
    if (!mFile) {
        return -1;
    }

    int r, frameSize;
    uint8_t* nextStartCode;
    //从mFile文件中读取size次1个字节的数据到frame数据中里
    r = fread(frame, 1, size, mFile);
    //读到的frame的前三位或前四位不是 0 0 0 1  或者 0 0 1 ，即不是nalu分隔符
    if (!startCode3(frame) && !startCode4(frame)) {
        //将mfile文件内部读取指针移动到文件内部的开头
        fseek(mFile, 0, SEEK_SET);
        
        LOGE("Read %s error, no startCode3 and no startCode4",mSourceName.c_str());
        return -1;
    }

    //在文件内部接着寻找分割符
    // 在frame + 3的位置开始寻找下一个分割符，并将其地址赋值给nextStartCode
    // r - 3表示从frame + 3开始的剩余字节数
    nextStartCode = findNextStartCode(frame + 3, r - 3);
    if (!nextStartCode) {
        //没找到就将文件内部指针移动到开头
        fseek(mFile, 0, SEEK_SET);
        // 如果没有找到下一个分割符，则将文件内部指针移动到开头
    // 并将帧的大小设置为
        frameSize = r;
        LOGE("Read %s error, no nextStartCode, r=%d", mSourceName.c_str(),r);
    }else {
        //帧的大小等于
         // 如果找到了下一个分割符，则将帧的大小设置为nextStartCode与frame之间的距离
        // 并将文件内部指针移动到下一个分割符之后
        frameSize = (nextStartCode - frame);
        //
  /*      (nextStartCode - frame)表示两个指针之间的偏移量，即nextStartCode指针减去frame指针的结果。
            这个偏移量表示了在内存中nextStartCode指针和frame指针之间的字节数。*/
        fseek(mFile, frameSize - r, SEEK_CUR);

       /*找到了下一个分割符nextStartCode之后执行的。它将文件指针移动到当前帧的末尾位置。*/

    }
    return frameSize;
}