/*
 * ota_task.h
 *
 * Created on:
 * Author: 
 */

#ifndef MAIN_OTA_TASK_H_
#define MAIN_OTA_TASK_H_

#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_http_client.h"

char ota_url[2048];
#define SIGNATURE_MAX_LENGTH 1024
enum {
	START_WEBSERVER = 0,
	STOP_WEBSERVER,
	INITIATE_CLOUD_OTA,
	INITIATE_JOBS_OTA
};
void ota_task(void *arg);
int16_t SignalOTAEvent(int event_in);
#endif /* MAIN_OTA_TASK_H_ */
