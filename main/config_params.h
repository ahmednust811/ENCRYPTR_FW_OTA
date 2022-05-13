#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_timer.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_wifi_default.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <sys/random.h>
#include "esp_timer.h"
#include "esp_sntp.h"
#include <time.h>
#include <sys/time.h>


xQueueHandle   gOTAMQueue;

//uint8_t BME680_CHIPID;


/**********************************************VARIABLES******************************************/


//QueueHandle_t xQueue1;


/**********************************************TYPEDEFS******************************************/

