/* 
 * File name      : app_main.c
 * Version        : 
 * History        :
 * Author         : 
 * Created On     : 
 *
 */

#include "ota_task.h"
#include "main.h"
#include "uart_task.h"

void app_main(void){

	printf("<===================== I AM BASE FIRMWARE!!!!!!!!!!! ===============================>");
	vTaskDelay(10000/portTICK_PERIOD_MS);
	//Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
	if(xTaskCreate(ota_task, "OTA_TASK", 1024*5, NULL, 3, NULL)!=pdPASS)
	{
		printf("OTA_TASK  creation failed aborting....!!!!");
		abort();
	}
	vTaskDelay(1000/ portTICK_PERIOD_MS);
	/*if(xTaskCreate(uart_task, "UART_TASK", 1024*2, NULL, 2, NULL)!=pdPASS)
	{
		printf("UART_TASK  creation failed aborting....!!!!");
		abort();
	}*/
	vTaskDelay(100/ portTICK_PERIOD_MS);
	wifi_init_sta();

}
//https://esp32test-en.s3.us-east-1.amazonaws.com/secure_ota_esp32_v1.bin?response-content-disposition=inline&X-Amz-Security-Token=IQoJb3JpZ2luX2VjEIL%2F%2F%2F%2F%2F%2F%2F%2F%2F%2FwEaCmFwLXNvdXRoLTEiSDBGAiEA4kDYsYVjbcVaj92MpS4H4%2BqVNSAHraYeywGT0BZCgOECIQDKdgQ1M3cNlFJunu5chVj7A3bbUMP%2FikaIT50rrrhB2yr7AghcEAAaDDc4MTE4MzA3ODQ2NyIMPDEorTb7X4xFIPwYKtgC%2FTRgdKW7jaNMtQY4Pr60crwREuiACl8lBaKoPJUure3I2zAsMajCf9YGGnVPQX9YR9ulMxEKoQTgV4rFXysh3DguFWFqOTW57wa5c%2FVv6PcE0hI2TtOhU3Thw4N5GRmQD223LAZeaaB9edycqcjARTAVo8tdhWhcQaJR9Zsb%2FwRK%2BBF6bdZXDQTv90axaFtGGnMF2qxUhOg0Gi0abR1TvSi%2FryEo4nflrKn3DcDslD4BQ2WDvBhmUE1irHobWKZder4c8Ahh%2FJLLcAh5GmQ4%2BP20CPStDlCe3bmajLdp%2BxZtBWrjedDsnY8s26S2cJiGVemIypUBynR4i%2BPI3O1V%2FTvdM7ZQljaVBSQYQtzrfIgpUzLl2vex8OghXyAKJeBoAE%2FTekVJwu6f2idq%2FujvKjeCmzDZnzmXNBXbZZw4U%2FZQNLXDeKuLNfsk3K8qBwm95tero%2BJL7Dkw%2B%2BT4kwY6sgKok9NC%2FXIQslYa4zpgga7ef1yQndA5Mbk57uRcw1WWMMGwSdxwkgnLAJJEEKxOhS1Hv0o3rdqBXKXKzpTQuWsIklJh1G6lSD81CUGeTrLG2siPDdsKspEQsmeV6EIqcrCfpSmraN%2BNzk28ECSJETiku10hJQIDeGKOCIW7HLH532eqX1XojJ7MYINndPrMy259IFHVtafIWgIC7DQTuQKjbMm4jW%2BUXwt6D7P3%2BZYFnj4gxexe3rw%2FZyUv964ZxYs%2FRwmzY0uVMGoMoayJvuq6CBHbVSu5kpLOYPq2t4LqlDmyqqYau%2FfzeEV9lsffL2c5RbILQFu%2B%2BSevBEHWi0QJav%2Bmx7SZq67Q9Q4NwZFvp%2FhyAx5eHzqEGTn6mQ9irXR6k32GRRlRHMnBMmKVpJJaP9Q%3D&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20220513T104159Z&X-Amz-SignedHeaders=host&X-Amz-Expires=3600&X-Amz-Credential=ASIA3LYRD2BBS6NO6FLI%2F20220513%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=0dbf787fd424f387f1128ecaa7d68ed81737c3d4446a42cef7c498062e019582