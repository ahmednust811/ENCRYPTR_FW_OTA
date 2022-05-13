/*Receives CO UART DATA
 * share the received byte data to lcd and network functions */


#include "main.h"
#include "uart_task.h"
#include "ota_task.h"
/* UART Driver initialization
 * We won't use a buffer for sending data.*/

/*@ FUNCTION initialize_CO_Communication
 * Initializes UART Driver
 * Configure UART Driver
*/

void init_uart0(){
	const uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};
	// We won't use a buffer for sending data.
	uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_0, &uart_config);
}


void uart_task(void *arg)
{
	static const char *RX_TASK_TAG = "uart_rx_task";		 //UART initialize for co controller communication
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
	uint8_t* Rx_Data = (uint8_t*) malloc(RX_BUF_SIZE+1);
	
	uint8_t rxBytes;
	init_uart0();
	while (1)
	{

		/* following is added for final test status reset */
		rxBytes = uart_read_bytes(UART_NUM_0, Rx_Data, RX_BUF_SIZE, 100/portTICK_RATE_MS);
		if(rxBytes>0){
			Rx_Data[rxBytes] = 0;
			ESP_LOGI(RX_TASK_TAG, "Read %d bytes from uart0: '%s'", rxBytes, Rx_Data);               //ESP  Log Entry
			ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, Rx_Data, rxBytes, ESP_LOG_INFO);
			if(strcmp((char *)Rx_Data,"start ota")==0){
				ESP_LOGI(RX_TASK_TAG,"Send signal for OTA");
    			SignalOTAEvent(INITIATE_JOBS_OTA);
			}
			else{
				printf("No comparisons found");
			}
		}
		/* end of final test status reset */

	}
	free(Rx_Data);
}


