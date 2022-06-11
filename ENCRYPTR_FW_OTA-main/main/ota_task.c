/*
 * ota_task.c
 *
 *  Created on: 
 *  Author:
 */

#include "ota_task.h"
#include "main.h"
#include "esp_http_server.h"


#define CLOUD_OTA_BUFFER_SIZE (4*1024)
#define CERT_LENGTH 1024



char signature[SIGNATURE_MAX_LENGTH];

//char ota_url[] = "https://esp32test-en.s3.us-east-1.amazonaws.com/secure_ota_esp32_v1.bin?response-content-disposition=inline&X-Amz-Security-Token=IQoJb3JpZ2luX2VjEIL%2F%2F%2F%2F%2F%2F%2F%2F%2F%2FwEaCmFwLXNvdXRoLTEiSDBGAiEA4kDYsYVjbcVaj92MpS4H4%2BqVNSAHraYeywGT0BZCgOECIQDKdgQ1M3cNlFJunu5chVj7A3bbUMP%2FikaIT50rrrhB2yr7AghcEAAaDDc4MTE4MzA3ODQ2NyIMPDEorTb7X4xFIPwYKtgC%2FTRgdKW7jaNMtQY4Pr60crwREuiACl8lBaKoPJUure3I2zAsMajCf9YGGnVPQX9YR9ulMxEKoQTgV4rFXysh3DguFWFqOTW57wa5c%2FVv6PcE0hI2TtOhU3Thw4N5GRmQD223LAZeaaB9edycqcjARTAVo8tdhWhcQaJR9Zsb%2FwRK%2BBF6bdZXDQTv90axaFtGGnMF2qxUhOg0Gi0abR1TvSi%2FryEo4nflrKn3DcDslD4BQ2WDvBhmUE1irHobWKZder4c8Ahh%2FJLLcAh5GmQ4%2BP20CPStDlCe3bmajLdp%2BxZtBWrjedDsnY8s26S2cJiGVemIypUBynR4i%2BPI3O1V%2FTvdM7ZQljaVBSQYQtzrfIgpUzLl2vex8OghXyAKJeBoAE%2FTekVJwu6f2idq%2FujvKjeCmzDZnzmXNBXbZZw4U%2FZQNLXDeKuLNfsk3K8qBwm95tero%2BJL7Dkw%2B%2BT4kwY6sgKok9NC%2FXIQslYa4zpgga7ef1yQndA5Mbk57uRcw1WWMMGwSdxwkgnLAJJEEKxOhS1Hv0o3rdqBXKXKzpTQuWsIklJh1G6lSD81CUGeTrLG2siPDdsKspEQsmeV6EIqcrCfpSmraN%2BNzk28ECSJETiku10hJQIDeGKOCIW7HLH532eqX1XojJ7MYINndPrMy259IFHVtafIWgIC7DQTuQKjbMm4jW%2BUXwt6D7P3%2BZYFnj4gxexe3rw%2FZyUv964ZxYs%2FRwmzY0uVMGoMoayJvuq6CBHbVSu5kpLOYPq2t4LqlDmyqqYau%2FfzeEV9lsffL2c5RbILQFu%2B%2BSevBEHWi0QJav%2Bmx7SZq67Q9Q4NwZFvp%2FhyAx5eHzqEGTn6mQ9irXR6k32GRRlRHMnBMmKVpJJaP9Q%3D&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20220513T152052Z&X-Amz-SignedHeaders=host&X-Amz-Expires=3600&X-Amz-Credential=ASIA3LYRD2BBS6NO6FLI%2F20220513%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=48e0596f42faf1b075d7c6ed732305d88d964c861b6af90d22fdf9c54e4ef57a";

const uint8_t starfield_cacert_aws_s3_start[] asm("_binary_ota_ca_starfield_crt_pem_start");
const uint8_t starfield_cacert_aws_s3_end[] asm("_binary_ota_ca_starfield_crt_pem_end");
const uint8_t baltimore_cacert_aws_s3_start[] asm("_binary_ota_ca_baltimore_crt_pem_start");
const uint8_t baltimore_cacert_aws_s3_end[] asm("_binary_ota_ca_baltimor_crt_pem_end");


httpd_handle_t server = NULL;

void get_running_partition(void)
{
	const esp_partition_t *runningPart = esp_ota_get_running_partition();
	esp_ota_img_states_t state = ESP_OTA_IMG_UNDEFINED;
	esp_err_t err;

	err = esp_ota_get_state_partition(runningPart, &state);
	ESP_LOGW(__func__, "\n##### RUNNING PARTITION #####\nlabel   : %s\ntype    : %s\nsubtype : %x\nstate   : %d", //\nsha256  : %s",
			runningPart->label, runningPart->type?"data":"app", runningPart->subtype, state/*, sha256_hash*/);
	ESP_LOGI(__func__, "subtype legend : 0x00 - factory, 0x1x - OTA_x");
	ESP_LOGI(__func__, "state legend   : 0x0 - new, 0x1 - pending, 0x2 - valid, 0x3 - invalid, 0x4 - aborted, -1 - undefined");

//If this flag is enabled while compiling bootloader on the device, then it must be enabled in subsequent app(s)
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
	//If current app is valid or undefined in case of factory app
	if(state == ESP_OTA_IMG_VALID || state == ESP_OTA_IMG_UNDEFINED)
		return;

	//If current app is pending verify after OTA set as valid
	if(state == ESP_OTA_IMG_PENDING_VERIFY && err == ESP_OK/*&& any other condition CCI?*/ )
	{
		if((err = esp_ota_mark_app_valid_cancel_rollback()) == ESP_OK)
		{
			ESP_LOGW(__func__, "current app %s marked as valid", runningPart->label);
			return;
		}
	}

	//If current app is valid do nothing
	ESP_LOGE(__func__, "initiating rollback");
	if((err = esp_ota_mark_app_invalid_rollback_and_reboot()) != ESP_OK)
		ESP_LOGE(__func__, "rollback failed, this should not happen");
#endif
	return;
}


static esp_err_t validate_image_header(esp_app_desc_t *new_app_info)
{
	printf("validate_image_header\n");
    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGI(__func__, "running firmware version: %s", running_app_info.version);
    }
#if 1
    char dummyver[32] = "0.2";
    if (memcmp(new_app_info->version, dummyver/*running_app_info.version*/, sizeof(new_app_info->version)) == 0) {
        ESP_LOGW(__func__, "current running version is the same as a new. we will not continue the update.");
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

int16_t process_cloud_ota(int type)
{

    esp_http_client_config_t config = {
        .cert_pem = (char *)starfield_cacert_aws_s3_start,
        .timeout_ms = 5000,
		.buffer_size = CLOUD_OTA_BUFFER_SIZE,
		.buffer_size_tx = 2048, //Because of signed url 2k is required
        .keep_alive_enable = true,
    };
	ESP_LOGI("OTA","%s",(char *)starfield_cacert_aws_s3_start);
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
	//ota_url ="https://esp32test-en.s3.amazonaws.com/secure_ota_esp32_v1.bin";
	config.url =OTA_URL;//"https://esp32test-en.s3.us-east-1.amazonaws.com/secure_ota_esp32_v1.bin?response-content-disposition=inline&X-Amz-Security-Token=IQoJb3JpZ2luX2VjECoaDmFwLXNvdXRoZWFzdC0xIkcwRQIhAJf5fO%2B1aYASEZmfWYkjnZ616hn0%2FMUvNtaTJu2h0M3HAiAyFZuRWKNogW3J%2FhNWG1A4hWKPRUU1agU64T6A3nPGHyr7AggzEAAaDDc4MTE4MzA3ODQ2NyIM6em2hGeiBJEGfYDwKtgC8eV9J4yAbZdbY5KpJ3THdCFyUgQ8VoHJ74deliAjPpwmI3wWDuW%2Bv%2FeS7TUk71m0VC4NTSMywjT5qUNaXc8fTB7DV1f2aKCD%2FO7%2BbBxc7vzViDXMcpQLY6xm19JYdfKiRX0q%2BHgLx6q626T3rpMl9znZpCeNdwyEqgx3pg7CGWddwpEPegxnNRy9Sk2BHJs2t8%2FS2KLlMiRMjw1mTCUaqGroBkZVXGsqaDkbFrsiiOnB55MI7lCdlqCT8pREM9AMZodlRl5d9G8uoph%2FOMlssnuU%2F2mQCbtP3JPiqMgk8PsS2yNklaLx9sUeQTRqsN%2BEToOZrQ3XMdL3%2Fe0rcNrs4GhzbeM7%2B5xkNZC3%2FEBPq1Jux5PxBzIKPj12u%2BSVlaHf6khd0zOURCawdUlGdlCUYJRywMP7ibIl2UnEVgjPjhWtDdc8rAE8xUiAd0fBwSWGPTcSWEnyGJ4wnYaOlQY6swK9FnAz2u0wsEzZesd6ccd%2BTahEte9UPX7YyJNBxb0QIfO4hBXDr%2BSQ6i%2FFp2DFyrVmwLUGqCnA1dBaCXJPCz4Jb5kLiB%2FdDGzfQKWp53Z5j6d9hsntafD51H1qplr3xJIBcuaZiGuTH7kFaS87g3a3GV2Ox4qpuypTXMN3e4SVCHN0ytlOWWKhGBdPV6oyuK9Y37r%2FChkSRgw9JdR73a7qZDrFcyCEkdnqhDPXAK30DO8Yc%2B5p1rR20MIgA5GRkwbFQ18k4Ge0HEaoKcqi3FcSewwhUcUqaARmA9kSoGRfhmupzUFIrKmvKcbePeIok4MooG%2F0MBAszyKeVCIQMwAMtYAS4nL5Oud%2BUBfEuzg2P7yswBvCuqXFsX7odqkJ6hZI6RybDUnb5coD9vhdP3fj2SPj&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20220610T174556Z&X-Amz-SignedHeaders=host&X-Amz-Expires=43200&X-Amz-Credential=ASIA3LYRD2BBZ4CY5SFG%2F20220610%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=7fe98e220ca8083aabff0fc870d549354d50fe23c00d2d76588eba3a8d4d539d";//(char*)ota_url;
	ESP_LOGI("OTA","url :  %s",OTA_URL);
	esp_https_ota_config_t ota_config = {
		.http_config = &config,
	};

	int image_len_read;
	esp_err_t err, ota_finish_err = ESP_FAIL;
	esp_https_ota_handle_t https_ota_handle = NULL;
	err = esp_https_ota_begin(&ota_config, &https_ota_handle);
	if (err != ESP_OK) {
		ESP_LOGE(__func__, "esp_https_ota_begin failed with starfield ca %d", err);
		https_ota_handle = NULL;
		goto ota_end;
		
	}
	printf("Heap available after ota begin : %d", esp_get_free_heap_size());

	esp_app_desc_t app_desc;
	err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
	if (err != ESP_OK) {
		ESP_LOGE(__func__, "esp_https_ota_get_img_desc failed %d", err);
		goto ota_end;
	}
	err = validate_image_header(&app_desc);
	if (err != ESP_OK) {
		ESP_LOGE(__func__, "validate_image_header failed %d", err);
		goto ota_end;
	}
	while (1) {
		err = esp_https_ota_perform(https_ota_handle);
		if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
			printf("break error %d\n",err);
			break;

		}
		// esp_https_ota_perform returns after every read operation which gives user the ability to
		// monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
		// data read so far.
		image_len_read = esp_https_ota_get_image_len_read(https_ota_handle);
		ESP_LOGI(__func__, "image bytes read: %d", image_len_read);
		if(image_len_read > (update_partition->size - CLOUD_OTA_BUFFER_SIZE/*bytes written in this loop*/))
		{
			ESP_LOGE(__func__, "image size is greater than partition size, aborting");
			err = ESP_FAIL;
			goto ota_end;
		}
	}

	if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
		// the OTA image was not completely received and user can customize the response to this situation.
		ESP_LOGE(__func__, "complete data was not received");
	}
	
	ota_end:
	if(err == ESP_OK)
		ota_finish_err = esp_https_ota_finish(https_ota_handle);
	else
		esp_https_ota_abort(https_ota_handle);
	if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
		ESP_LOGI(__func__, "esp_https_ota_finish successful... rebooting ...");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		esp_restart();
	} else {
		if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
			ESP_LOGE(__func__, "esp_https_ota_finish failed, image is corrupted");
		}
		ESP_LOGE(__func__, "esp_https_ota_finish failed %d", ota_finish_err);
	}
	return 0;
}

void ota_task(void *arg)
{
	int32_t status;
	int event;
	ESP_LOGI(__func__, "OTA thread start");
	get_running_partition();
	gOTAMQueue = xQueueCreate(5, sizeof(int));
	while(1)
	{
		status = xQueueReceive(gOTAMQueue, &event, (TickType_t)10);
		if(status == pdTRUE )
		{
			if(event == START_WEBSERVER)
			{
				printf("fill later");
			}
			else if(event == STOP_WEBSERVER)
			{
				printf("fill later");
			}
			else if(event == INITIATE_CLOUD_OTA)
			{
				printf("fill later");
			}
			else if(event == INITIATE_JOBS_OTA)
			{
				printf("Signal recieved in OTA task\n");
				process_cloud_ota(event);
			}
		}
		vTaskDelay(10/ portTICK_PERIOD_MS);
	}
}

int16_t SignalOTAEvent(int event_in)
{
	int event = event_in;
	int32_t status = 0;
    status = xQueueSend(gOTAMQueue, &event, NULL);
    if (status < 0)
    {
    	ESP_LOGE(__func__, "signal OTA to Q failed %d", status);
    }
    return 0;
}
