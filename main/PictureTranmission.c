#include "PictureTranmission.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "PictureProcess.h"

static const char *TAG = "UDP_CLIENT";
#define HOST_IP_ADDR "192.168.145.19"
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
char img[14000] = "FFD8FFE000104A464946000101010078"
                  "00780000FFE100224578696600004D4D"
                  "002A0000000800010112000300000001"
                  "0001000000000000FFDB004300020101"
                  "02010102020202020202020305030303"
                  "03030604040305070607070706070708"
                  "090B0908080A0807070A0D0A0A0B0C0C"
                  "0C0C07090E0F0D0C0E0B0C0C0CFFDB00"
                  "4301020202030303060303060C080708"
                  "0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C"
                  "0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C"
                  "0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C0C"
                  "0C0CFFC0001108004000400301220002"
                  "1101031101FFC4001F00000105010101"
                  "01010100000000000000000102030405"
                  "060708090A0BFFC400B5100002010303"
                  "020403050504040000017D0102030004"
                  "11051221314106135161072271143281"
                  "91A1082342B1C11552D1F02433627282"
                  "090A161718191A25262728292A343536"
                  "3738393A434445464748494A53545556"
                  "5758595A636465666768696A73747576"
                  "7778797A838485868788898A92939495"
                  "969798999AA2A3A4A5A6A7A8A9AAB2B3"
                  "B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CA"
                  "D2D3D4D5D6D7D8D9DAE1E2E3E4E5E6E7"
                  "E8E9EAF1F2F3F4F5F6F7F8F9FAFFC400"
                  "1F010003010101010101010101000000"
                  "0000000102030405060708090A0BFFC4"
                  "00B51100020102040403040705040400"
                  "01027700010203110405213106124151"
                  "0761711322328108144291A1B1C10923"
                  "3352F0156272D10A162434E125F11718"
                  "191A262728292A35363738393A434445"
                  "464748494A535455565758595A636465"
                  "666768696A737475767778797A828384"
                  "85868788898A92939495969798999AA2"
                  "A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9"
                  "BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7"
                  "D8D9DAE2E3E4E5E6E7E8E9EAF2F3F4F5"
                  "F6F7F8F9FAFFDA000C03010002110311"
                  "003F00F3FF0015785FC3BF1F3C1FF066"
                  "E2DF47D363BED3CB69B3F9702AF9C155"
                  "586FC0F9B83D4FA57DE5F063F67FF0AD"
                  "87876C6DDBC37A1B347100736719EDF4"
                  "AFCFAFF826ADCAF8FF0057D26DEE3CC7"
                  "874E927BE894FF0008D91C60FE791F81"
                  "AFD46F85212054DDC10318AFC8EB56A8"
                  "EB724A4F73FA3B2DA14E185F6918ABD8"
                  "9FFE19DFC23B7FE459D033FF005E11FF"
                  "00F13552E7F677F0A2AFCBE19D071FF5"
                  "E11F3FA57A4A956887F8D3A5B06961DC"
                  "83B715EAFB08B8F53CFF00AC352B348F"
                  "9E7C77F003C330B36DF0FE86B91C62CA"
                  "31CFE55E13F163E0FE87A55ACDB344D2"
                  "55B9C11689FE15F4E7C6FF00891E19F8"
                  "73048DAF6BDA5E9D22E7F772DC28918F"
                  "A05CE7F4AF91FE2EFED6BE15F105FB5A"
                  "E90350D5E476DA3ECB09607F3FE95E5B"
                  "C3D5F6978A763D48D6A1ECFDFB5CF9CB"
                  "E3BF83D746D17FB6B47D3F4E8AF74E7F"
                  "993ECEAAB321EAA78FD6BE76F887E3ED"
                  "17E273C2B359D8E91E20B3FDDC913A2A"
                  "A5C0EDCF4CFBF7AFAEBC693DAF8C748B"
                  "AB687CD459810D1CB198DE36F420D7E7"
                  "BFED35E0AB9D0BE23BF96EF1C8AE4640"
                  "EA3B57D2E5F4FDA45C247C5E751717ED"
                  "628FB2BFE08DDE36FB678A7C45757863"
                  "B6B836B1456D6E0E56282327A7D7764D"
                  "7DC97FFB79695E06D466B7D3F44D4FC4"
                  "13DA9DB22DB2ED527B8CE0FE75F95BFF"
                  "0004D7F194BE16FDA26CEC5A495AD354"
                  "84C6EC14ED5C8C609FF3CD7EADAFC3BB"
                  "3B6D1E376B75D174344DD7B79656A26B"
                  "BB8CF240241C67D719AE3CC3074A15F9"
                  "8F6325C5559E13951D4FC24FF828549E"
                  "36D7162D43C27FD996520E03CDFBE53E"
                  "E315EC9A8FED096E7C19797FA7DBC8C2"
                  "DD0B10064A8AF98743F87DE1FF00146A"
                  "133F853C21E2F8A1B72A52F6FAE551AE"
                  "B3DD5589E0FB815EFDF0E7C14345F014"
                  "D617D6CCB26A103C72A4A4168F3EE38F"
                  "CAB1A951C7E17A1D94E8A9EFB9F23F8B"
                  "3E165C7C7EF8812ACD696A5A590CD2CF"
                  "72C4AC793EB51F8D7C07E1BF81F6B6F6"
                  "D6BE2A8EE3550097B3D2AC14AC78EECE"
                  "14FEA6BE80F00780EC26F13DE68B7ADE"
                  "54E5F636D62BBF1D064735D6789BF662"
                  "F0EC9691AC9611C61410C62455790672"
                  "7737539FAD3C3D752415F0AA36B9F30E"
                  "B1F0566F887E07B3D7A3C19A48F392BB"
                  "5A4EDCFF008D7C39FB6B7C02BA87C471"
                  "EA255628D59629D8F3B7A0CD7EAFF8C6"
                  "7D37C1DA4AE9F6D1C70DBC485553D2BE"
                  "09FDBD752DFA05FBDBC2B32B4AA5B03A"
                  "0CD2C3E2671ACF949C660A32A5A9E7FF"
                  "00B08780749D1BC4B79637568CD75F61"
                  "8F52B3940C3178C2B48B9FF7727F0AFD"
                  "5EFD9EFC51A6F8A7E1FD84D1B46F0CB1"
                  "01B1C640EC73EF9AFC6DFD98BF69FD32"
                  "FAE7C25E228988FECD852DB57B541F3E"
                  "CC795215F5CAE78F5AFD32F817E22D35"
                  "3458EEFC2FAA7DBBC3FA90173679C6E8"
                  "09C6E89BD0A9AACD3DA42B7BC7364156"
                  "954C3249EA7D6FA368967030686DE18F"
                  "701F7539AC6F135FC435F5B356FDEE31"
                  "B7BF3587F0F3E234F7913472AEE9957E"
                  "5C7F1571FF00182E7C77E1CD534FD5B4"
                  "05D0F74D781AEDF528E46448403C0D9C"
                  "A9E9C904571CAA46A42C7A94E8BA757D"
                  "EF91C5FC75D2352F077C53FB446DF667"
                  "BAC3C459B01B18AF4CF855F13BFE130F"
                  "0E35B5E73A859A147E7B7AD7CF7FB587"
                  "C34F1A7ED21AB4536B97D71A4E8BA36D"
                  "BB71A25D7973DCA70400C46E0BDFA569"
                  "FC08F895A07C2F2D03DD5F489242B028"
                  "BC7DD32ED18F99BB9F7A9A74DC1686D5"
                  "24E4929EE73DFB4B78B2E20F1F2D8C6C"
                  "C2266249CE2BC2BE3BE950EA7E199964"
                  "C36F8C865EB9AF61FDA9F50D37C4BFF1"
                  "39D1E6F36E2D7E67453C85EE6BC2F54D"
                  "45FC5BE119A567CB72B83C56B08B5EF7"
                  "532AB514D72967FE0AAFFF0004CEF853"
                  "FF0004E5FD927E1BFC60F87E750D2DBC"
                  "54F6BA66A9A45CDDB5C43334D6CD2F9D"
                  "196CB290C9C8CE083DB15F317EC35FB6"
                  "FDAF817E3069B9BA923D2B589858DC45"
                  "E61F2B739015B1D339C0CE3A1AF16FF8"
                  "288FFC15E3E24FFC1443C3FE12F0DF89"
                  "174FD17C29E07B75B7D3748D383796D2"
                  "2A6C3348CC49772A303A000F03935F30"
                  "E9BE27BCD2CC2F6D3490BDBC8258CA9C"
                  "6C60720FD6BEF717952C47BCF73F16C9"
                  "F389611ABBD0FE9CFC0BF14A3D3EFA19"
                  "90656423A7A549F183E36BEAA63B57FD"
                  "CDA44DB9B9EA3BE7D88AF85FFE09EBFB"
                  "6C2FED31F06ADB75C2AF8974344B7D46"
                  "2620316C7120FF006580FC0E6BE968B5"
                  "7D37E215ADAD9DF6D5B857F2EEA22C54"
                  "C89D882A41AF8BA9857426E123F5DC3E"
                  "3E389A11A9033341FDBF7E1BEAFE28BB"
                  "B0B3D42F358D7345B63A74B69080B6F3"
                  "B83819727802B88F881AF7893C5971E6"
                  "E97E13B36B7BA6DE823BBD9B49F739FC"
                  "F8FA56FD97ECEBF033E1FF00896FAEAE"
                  "7408B43D42625E691659FF007C739C83"
                  "BB0735A9A9FED07F0FFC07E1F6D3FC37"
                  "637FAA5C365225D8DD7D99893F957453"
                  "85351BADCF4A9A8257AACC8B3F0C2E87"
                  "692437CF1F9CD6EA67546DCA8CD8CAE7"
                  "BE2BC27C4DACAE8DACEAD670F30C6C5D"
                  "31D00AF506F126A9068B777DAAC5E4CD"
                  "7D26E48CFF000815F3C7C58F151B2F18"
                  "5D2C7BB7DE461140FEF1AE7F66DBB9E4"
                  "E2AB538C9F26C7FFD9--------------";

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
