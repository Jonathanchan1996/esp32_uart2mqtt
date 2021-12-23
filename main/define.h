#ifndef __DEFINE_H
#define __DEFINE_H 

//Sensor Box 
//GPIO configuration 
#define funcBtn 0 //Functional button
#define IT0 8
#define IR0 15
#define IT1 6
#define IR1 7
#define IT2 10
#define IR2 11
#define IT3 13
#define IR3 9
#define IT4 22
#define IR4 23
#define IT5 18
#define IR5 19
#define IT6 17
#define IR6 5
#define IT7 4
#define IR7 16

#define IOA0 34
#define IOB0 35
#define IOC0 32
#define IOD0 33
#define IOA1 26
#define IOB1 25
#define IOC1 14
#define IOD1 12

#define LED 2 
#define fan 21

#define ndirUart UART_NUM_2
#define ndirTx IT6
#define ndirRx IR6
// #define ndirTx 27
// #define ndirRx 26

#define pmUart UART_NUM_1
#define pmTx IR4
#define pmRx IT4

#define hdcI2c I2C_NUM_0
#define I2C_SDA_PIN IR7
#define I2C_SCL_PIN IT7
// #define I2C_SDA_PIN 14
// #define I2C_SCL_PIN 12




//OLED PINOUT
#define CLK 21
#define DIN 19
#define RES 18
#define DC 5
#define CS 4

#define GPIO_OUTPUT_PIN_SEL ((1UL<<fan)) //(1ULL<<LED)|


//Functional defines
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

/*
//ADC pin designed to be used
ACD1_CH4 A4	GPIO 32	Free to use
ACD1_CH5 A5	GPIO 33	Free to use
ACD1_CH6 A6	GPIO 34	Free to use
ACD1_CH7 A7	GPIO 35	Free to use
*/

#define TWDT_TIMEOUT_S  10

#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("TWDT ERROR\n");                                \
                abort();                                               \
            }                                                          \
})

#endif

//LED toggle message (toggle time in ms)
#define LEDTog_wifiDisconnect 50
#define LEDTog_googleSheetDisconnect 100
#define LEDTog_normal 1000

//Soft WatchDog Timer threshold count
#define SWDT_ndirCo2 10
#define SWDT_tempRh 10
#define SWDT_pm 10
#define SWDT_wifiDisconnect 60
#define SWDT_googleSheetDisconnect 60
#define SWDT_innoairSerDisconnect 60

//func button function
#define pushBtnTolerance 3
#define isbuttonPush !gpio_get_level(funcBtn)
#define buttonPress(a,b) ((a)>(b)-pushBtnTolerance&&(a)<(b)+pushBtnTolerance?true:false)

#define pushBtnTime_rstWifi 10
#define pushBtnTime_httpOta 20
//Control 
#define co2Threshold 800

