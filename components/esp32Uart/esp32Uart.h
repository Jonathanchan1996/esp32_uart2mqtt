#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP32UART_H
#define ESP32UART_H

#include <stdint.h> 
#include <stdbool.h> 
struct esp32Uart {
    uint8_t uartPort;
    uint8_t TX_PIN; 
    uint8_t RX_PIN;
    int bandwidth;
};


void esp32UartInit(struct esp32Uart *myUart);

#endif // ifndef ESP32UART_H
#ifdef __cplusplus
}
#endif