#include "PictureTranmission.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "PictureProcess.h"

static const char *TAG = "UDP_CLIENT";
#define HOST_IP_ADDR "192.168.145.187"
#define PORT 3333
#define UDP_Init_OK BIT0

EventGroupHandle_t TransmitReadyHandler;
extern QueueHandle_t PictureJsonQueueHandler;

int SendSock = -1;
struct sockaddr_in destAddr;
struct sockaddr_in sourceAddr;
struct sockaddr_in sourceAddr;
socklen_t socklen = sizeof(sourceAddr);

void UDP_Client_Init();
void PictureTranmissionInit();
void PictureTranmissionTask(void *para);

void UDP_Client_Init()
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;
    struct timeval ReceiveTimeOverflow = {
        .tv_sec = 10,
        .tv_usec = 0,
    };
    struct timeval SendTimeOverflow = {
        .tv_sec = 10,
        .tv_usec = 0,
    };
    destAddr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);
    while (1)
    {
        SendSock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        setsockopt(SendSock, SOL_SOCKET, SO_RCVTIMEO, &ReceiveTimeOverflow, sizeof(ReceiveTimeOverflow));
        setsockopt(SendSock, SOL_SOCKET, SO_SNDTIMEO, &SendTimeOverflow, sizeof(SendTimeOverflow));
        if (SendSock >= 0)
        {
            ESP_LOGI("UDP_Client", "Successful Create Socket");
            ESP_LOGI("UDP_Client", "Attached to %s", addr_str);
            break;
        }
    }
}
// 图片源maybe可以把实时获取的数据丢进去
char img[14000] = "data:image/jpeg;base64,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";

void PictureTranmissionInit()
{
    UDP_Client_Init();
    taskENTER_CRITICAL();
    PictureJsonQueueHandler = xQueueCreate(1, sizeof(char *));
    xTaskCreate(PictureTranmissionTask, "PictureTranmissionTask", 10000, NULL, 5, NULL);
    taskEXIT_CRITICAL();
    while (1)
    {
        PictureProcess(img, strlen(img));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void PictureTranmissionTask(void *para)
{
    char *receivebuf;
    while (1)
    {
        xQueueReceive(PictureJsonQueueHandler, &receivebuf, portMAX_DELAY);
        printf("--------------------Transmit---------------------\r\n");
        printf("%s\r\n", receivebuf);
        printf("--------------------End--------------------------\r\n");
        int err = sendto(SendSock, (char *)receivebuf, strlen(receivebuf), 0, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
