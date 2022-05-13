/* 
 * File name      : app_main.c
 * Version        : 
 * History        :
 * Author         : 
 * Created On     : 
 *
 */

#include "ota_task.h"

void app_main(void){

	if(xTaskCreate(ota_task, "OTA_TASK", 1024*5, NULL, 3, NULL)!=pdPASS)
	{
		printf("OTA_TASK  creation failed aborting....!!!!");
		abort();
	}

}
