#include "esp32Uart.h"
#include <driver/uart.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>

void esp32UartInit(struct esp32Uart *myUart){
//#define uart_num UART_NUM_2
    uart_config_t uart_config = {
        .baud_rate = myUart->bandwidth,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    //Tx
    uart_param_config(myUart->uartPort, &uart_config);
    uart_set_pin(myUart->uartPort, myUart->TX_PIN, myUart->RX_PIN, 18, 19); 
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    uart_driver_install(myUart->uartPort, uart_buffer_size,uart_buffer_size, 10, &uart_queue, 0);

}
