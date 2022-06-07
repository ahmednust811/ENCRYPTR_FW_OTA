
#define TXD_PIN (GPIO_NUM_32)
#define RXD_PIN (GPIO_NUM_35)


static const int RX_BUF_SIZE = 1024;
void uart_task(void *arg);

//void print_debug_data(char data);
