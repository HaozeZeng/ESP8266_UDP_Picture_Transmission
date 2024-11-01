#include "mqttbasedef.h"
#include "station.h"
#include "PictureTranmission.h"
#include "PictureProcess.h"

static const char *TAG = "Main";
extern void defaultTask();

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_station(); // init wifi station
    PictureTranmissionInit();
}