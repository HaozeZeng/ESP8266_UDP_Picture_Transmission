#include "PictureTranmission.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "PictureProcess.h"
#include "cJSON.h"
const char *TAG = "test";
QueueHandle_t PictureQueueHandler;
QueueHandle_t PictureJsonQueueHandler;

// 发送图片给上位机
// 处理完一部分发一部分
// 开始帧的length用来装有多少帧
// 以Json形式发送
// 完成
// 代码美化
char template[IMAGE_FRAME_SIZE + 100] = "{"
                                        "\"header\": %d,"
                                        "\"length\": %d,"
                                        "\"packIdx\": %d,"
                                        "\"picID\": %d,"
                                        "\"Timestamp\": %d,"
                                        "\"body\": \"%s\""
                                        "}";

#define StartFrame(packlength)                      \
    {                                               \
        PicFrame.header = START;                    \
        PicFrame.length = packlength;               \
        PicFrame.packIdx = 0;                       \
        PicFrame.picID = IMAGE_STREAM;              \
        memset(PicFrame.body, 0, IMAGE_FRAME_SIZE); \
        strcpy(PicFrame.body, "StartFrame");        \
        PicFrame.Timestamp = xTaskGetTickCount();   \
    }

#define EndFrame(packIdx)                           \
    {                                               \
        PicFrame.header = END;                      \
        PicFrame.length = 0;                        \
        PicFrame.packIdx = packIdx;                 \
        PicFrame.picID = IMAGE_STREAM;              \
        memset(PicFrame.body, 0, IMAGE_FRAME_SIZE); \
        strcpy(PicFrame.body, "EndFrame");          \
        PicFrame.Timestamp = xTaskGetTickCount();   \
    }

uint32_t GetPacklength(uint32_t imgsize)
{
    int packlength = imgsize / IMAGE_FRAME_SIZE;
    if (imgsize % IMAGE_FRAME_SIZE != 0)
        packlength++;
    return packlength;
}

bool PictureProcess(char *imgbuf, uint32_t imgsize)
{
    pFrame PicFrame;
    BaseType_t err = pdFALSE;
    uint8_t packIdx = 0;
    uint32_t FrameNum = GetPacklength(imgsize);
    char *JsonBuffer;
    char *tmpbuf = imgbuf;
    int size = imgsize;
    // 发送开始帧
    StartFrame(FrameNum);
    // 分配内存用于存储JSON数据
    JsonBuffer = (char *)malloc(sizeof(template));
    // 格式化图片帧信息到JSON缓冲区
    snprintf(JsonBuffer, sizeof(template), template,
             PicFrame.header, PicFrame.length, PicFrame.packIdx,
             PicFrame.picID, PicFrame.Timestamp, PicFrame.body);
    // 将JSON数据发送到队列
    err = xQueueSend(PictureJsonQueueHandler, &JsonBuffer, portMAX_DELAY);
    // 释放内存
    free(JsonBuffer);
    // 检查发送是否失败
    if (err == pdFALSE)
    {
        ESP_LOGW("JsonBuffer", "SendError HEAD %d", PicFrame.header);
        return err;
    }
    packIdx++;

    // ESP_LOGW("FrameNum", "%d", FrameNum);
    // 遍历每个帧
    for (int i = 0; i < FrameNum; i++)
    {
        // 检查剩余数据是否大于一帧的大小
        if ((size - (IMAGE_FRAME_SIZE - 1)) > 0) // IMAGE_FRAME_SIZE中最后一位是\0
        {
            int length = IMAGE_FRAME_SIZE - 1; // 为他留下一个\0位
            // 复制数据到帧体
            memcpy(PicFrame.body, tmpbuf, length); // length为实际的大小
            PicFrame.body[IMAGE_FRAME_SIZE] = '\0';
            // 设置帧头信息
            PicFrame.header = PROCESSING;
            PicFrame.length = length;
            PicFrame.packIdx = packIdx;
            PicFrame.picID = IMAGE_STREAM;
            PicFrame.Timestamp = xTaskGetTickCount();
#ifdef DEBUG
            // 调试信息输出
            printf("--------------------DEBUG-----------------------------\n");
            printf("PicFrame.header=%d\n", PicFrame.header);
            printf("PicFrame.length=%d\n", PicFrame.length);
            printf("PicFrame.packIdx=%d\n", PicFrame.packIdx);
            printf("PicFrame.picID=%d\n", PicFrame.picID);
            printf("PicFrame.Timestamp=%d\n", PicFrame.Timestamp);
            printf("PicFrame.body=%s\n", (char *)PicFrame.body);
#endif

            // 分配内存用于存储JSON数据
            char *JsonBuffer = (char *)malloc(sizeof(template));
            // 格式化图片帧信息到JSON缓冲区
            snprintf(JsonBuffer, sizeof(template), template,
                     PicFrame.header, PicFrame.length, PicFrame.packIdx,
                     PicFrame.picID, PicFrame.Timestamp, PicFrame.body);
            // 减少剩余数据大小，移动到下一帧
            size -= length;
            tmpbuf = tmpbuf + length;
            // 将JSON数据发送到队列
            err = xQueueSend(PictureJsonQueueHandler, &JsonBuffer, portMAX_DELAY);
            // 释放内存
            free(JsonBuffer);
            // 检查发送是否失败
            if (err == pdFALSE)
            {
                ESP_LOGW("JsonBuffer", "SendError %d", PicFrame.header);
                return err;
            }
        }
        // 检查剩余数据是否小于一帧的大小
        else if ((size - (IMAGE_FRAME_SIZE - 1)) < 0) // IMAGE_FRAME_SIZE - 1为实际要传的大小
        {
            int length = size % (IMAGE_FRAME_SIZE - 1);
            // 复制数据到帧体
            memcpy(PicFrame.body, tmpbuf, length);
            PicFrame.body[length] = '\0';
            // 设置帧头信息
            PicFrame.header = PROCESSING;
            PicFrame.length = length;
            PicFrame.packIdx = packIdx;
            PicFrame.picID = IMAGE_STREAM;
            PicFrame.Timestamp = xTaskGetTickCount();
#ifdef DEBUG
            // 调试信息输出
            printf("--------------------DEBUG-----------------------------\n");
            printf("PicFrame.header=%d\n", PicFrame.header);
            printf("PicFrame.length=%d\n", PicFrame.length);
            printf("PicFrame.packIdx=%d\n", PicFrame.packIdx);
            printf("PicFrame.picID=%d\n", PicFrame.picID);
            printf("PicFrame.Timestamp=%d\n", PicFrame.Timestamp);
            printf("PicFrame.body=%s\n", (char *)PicFrame.body);
#endif
            // 减少剩余数据大小，移动到下一帧
            size -= length;
            tmpbuf = tmpbuf + length;
            // 分配内存用于存储JSON数据
            JsonBuffer = (char *)malloc(sizeof(template));
            // 格式化图片帧信息到JSON缓冲区
            snprintf(JsonBuffer, sizeof(template), template,
                     PicFrame.header, PicFrame.length, PicFrame.packIdx,
                     PicFrame.picID, PicFrame.Timestamp, PicFrame.body);
            // 将JSON数据发送到队列
            err = xQueueSend(PictureJsonQueueHandler, &JsonBuffer, portMAX_DELAY);
            // 释放内存
            free(JsonBuffer);
            // 检查发送是否失败
            if (err == pdFALSE)
            {
                ESP_LOGW("JsonBuffer", "SendError %d", PicFrame.header);
                return err;
            }
        }
        // 处理最后一帧
        else
        {
            int length = (IMAGE_FRAME_SIZE - 1);
            // 设置帧头信息
            PicFrame.header = PROCESSING;
            PicFrame.length = length;
            PicFrame.packIdx = packIdx;
            PicFrame.picID = IMAGE_STREAM;
            PicFrame.Timestamp = xTaskGetTickCount();
            // 复制数据到帧体
            memcpy(PicFrame.body, tmpbuf, length);
            PicFrame.body[IMAGE_FRAME_SIZE] = '\0';

#ifdef DEBUG
            // 调试信息输出
            printf("--------------------DEBUG-----------------------------\n");
            printf("PicFrame.header=%d\n", PicFrame.header);
            printf("PicFrame.length=%d\n", PicFrame.length);
            printf("PicFrame.packIdx=%d\n", PicFrame.packIdx);
            printf("PicFrame.picID=%d\n", PicFrame.picID);
            printf("PicFrame.Timestamp=%d\n", PicFrame.Timestamp);
            printf("PicFrame.body=%s\n", (char *)PicFrame.body);
#endif
            // 分配内存用于存储JSON数据
            JsonBuffer = (char *)malloc(sizeof(template));
            // 格式化图片帧信息到JSON缓冲区
            snprintf(JsonBuffer, sizeof(template), template,
                     PicFrame.header, PicFrame.length, PicFrame.packIdx,
                     PicFrame.picID, PicFrame.Timestamp, PicFrame.body);
            // 将JSON数据发送到队列
            err = xQueueSend(PictureJsonQueueHandler, &JsonBuffer, portMAX_DELAY);
            // 释放内存
            free(JsonBuffer);
            // 检查发送是否失败
            if (err == pdFALSE)
            {
                ESP_LOGW("JsonBuffer", "SendError %d", PicFrame.header);
                return err;
            }
        }
        // 延迟一段时间
        vTaskDelay(20);
        packIdx++;
    }
    // 处理结束帧
    EndFrame(packIdx);
    // 分配内存用于存储JSON数据
    JsonBuffer = (char *)malloc(sizeof(template));
    // 格式化图片帧信息到JSON缓冲区
    snprintf(JsonBuffer, sizeof(template), template,
             PicFrame.header, PicFrame.length, PicFrame.packIdx,
             PicFrame.picID, PicFrame.Timestamp, PicFrame.body);
    // 将JSON数据发送到队列
    err = xQueueSend(PictureJsonQueueHandler, &JsonBuffer, portMAX_DELAY);
    // 释放内存
    free(JsonBuffer);
    // 检查发送是否失败
    if (err == pdFALSE)
    {
        ESP_LOGW("JsonBuffer", "END %d", PicFrame.header);
        return err;
    }
    return err;
}
