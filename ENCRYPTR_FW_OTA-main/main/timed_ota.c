#include "ota_task.h"
#include "main.h"
#include "timed_ota.h"
#define expiry_time_secs 60;
void timed_task(){
     sntp_setoperatingmode(SNTP_OPMODE_POLL);
    ESP_LOGI(TAG,"WAITING FOR SNTP");
 sntp_setservername(0, "time.google.com");
    //sntp_setservername(1,"time.nist.gov");
    sntp_init();
 time_t now;
    time(&now);
    uint32_t iat = now;              // Set the time now.
    uint32_t exp = iat + expiry_time_secs;
    while(1){

        if(iat>exp){

            time(&now);
            iat = now;
            exp = iat + expiry_time_secs;
            ESP_LOGI("timed_ota:","%s","starting ota task");
            SignalOTAEvent(INITIATE_JOBS_OTA);


        }
        time(&now);
        iat = now;
        ESP_LOGI("OTA to take place in:","%lu seconds",exp-now);
         vTaskDelay(1000 / portTICK_RATE_MS);
    } 

}
void start_timed_ota(){


ESP_LOGI("timed_task:", "%s","started");
xTaskCreate(timed_task, "TIMED_TASK", 1024*2, NULL, 2, NULL);




}


