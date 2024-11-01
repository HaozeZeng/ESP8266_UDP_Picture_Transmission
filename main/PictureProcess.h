#ifndef PictureProcess_H
#define PictureProcess_H
#include "mqttbasedef.h"

#define IMAGE_FRAME_SIZE 1025 // 每一帧body的大小,实际大小为1024要减去最后一位
#define IMAGE_STREAM 0        // 可能会有多个图片流，所以要区分

enum category
{
    START = 0,
    PROCESSING = 1,
    END = 2
};
typedef struct PictureFrame
{
    enum category header;        // START, PROCESSING, END
    uint16_t length;             // sizeof(body)
    uint8_t picID;               // 某一帧图片
    uint8_t packIdx;             // 一帧图片的一个json片段
    char body[IMAGE_FRAME_SIZE]; // 实际大小为IMAGE_FRAME_SIZE-1
    uint32_t Timestamp;          // 时间戳
} pFrame;
bool PictureProcess(char *imgbuf, uint32_t imgsize);

#endif