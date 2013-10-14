/*******************************************************************/
#define STM32F40XX

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define MAIN

#include "fifo.h"
#include "keyboardscan.h"
#include "midi.h"
#include "hd44780.h"
#include "timer.h"
#include "usb_init.h"
#include "usb_midi_io.h"

#define MIDI_BAUDRATE 31250                         //Midi speed baudrate

//USB_OTG_CORE_HANDLE USB_OTG_dev;

TIM_TimeBaseInitTypeDef timer;

uint16_t previousState;
uint8_t count;
uint16_t i;
uint16_t min = 0xFFFF;
uint16_t max = 0;
uint8_t mid = 0;

typedef struct {
    uint8_t channel;
    uint8_t event;
    uint8_t value;
    uint8_t min_in_value;
    uint8_t max_in_value;
    uint8_t min_out_value;
    uint8_t max_out_value;
    uint8_t reverse;
} Slider;

#define MAX_STRLEN 12 // this is the maximum string length of our string in characters

volatile char received_string[MAX_STRLEN + 1]; // this will hold the recieved string

void delay(volatile uint32_t c ) {
    while ( --c ) {
        __NOP();
    }
}


void delayms(volatile uint32_t c ) {
    c++;
    while ( --c ) {
        delay(23080);
    }
}

/*
* GPIO init
*/

void init_GPIO(void) {

    GPIO_InitTypeDef GPIO_InitStruct;

    /*Enable or disable the AHB1 peripheral clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /*Configure GPIO pin */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);


    /*Configure GPIO pin */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pin */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIOA->PUPDR |= 0x00005555;


    /* Р В РЎСљР В Р’В°Р РЋР С“Р РЋРІР‚С™Р РЋР вЂљР В РЎвЂўР В РІвЂћвЂ“Р В РЎвЂќР В Р’В° Р РЋРІР‚С™Р В Р’В°Р В РІвЂћвЂ“Р В РЎпїЅР В Р’ВµР РЋР вЂљР В Р’В° 4 */

    //Р В РІР‚вЂќР В Р’В°Р В РЎвЂ”Р В РЎвЂўР В Р’В»Р В Р вЂ¦Р РЋР РЏР В Р’ВµР В РЎпїЅ Р В РЎвЂ”Р В РЎвЂўР В Р’В»Р РЋР РЏ Р РЋР С“Р РЋРІР‚С™Р РЋР вЂљР РЋРЎвЂњР В РЎвЂќР РЋРІР‚С™Р РЋРЎвЂњР РЋР вЂљР РЋРІР‚в„– Р В РўвЂ�Р В Р’ВµР РЋРІР‚С›Р В РЎвЂўР В Р’В»Р РЋРІР‚С™Р В Р вЂ¦Р РЋРІР‚в„–Р В РЎпїЅР В РЎвЂ� Р В Р’В·Р В Р вЂ¦Р В Р’В°Р РЋРІР‚РЋР В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р РЋР РЏР В РЎпїЅР В РЎвЂ�
    TIM_TimeBaseStructInit(&timer);

    //Р В РІР‚в„ўР РЋРІР‚в„–Р РЋР С“Р РЋРІР‚С™Р В Р’В°Р В Р вЂ Р В Р’В»Р РЋР РЏР В Р’ВµР В РЎпїЅ Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР В РўвЂ�Р В РўвЂ�Р В Р’ВµР В Р’В»Р В РЎвЂ�Р РЋРІР‚С™Р В Р’ВµР В Р’В»Р РЋР Р‰, 100uS
    timer.TIM_Prescaler = TIMER_PRESCALER-1;

    //Р В РЎС›Р РЋРЎвЂњР РЋРІР‚С™ Р В Р’В·Р В Р вЂ¦Р В Р’В°Р РЋРІР‚РЋР В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ, Р В РўвЂ�Р В РЎвЂўР РЋР С“Р РЋРІР‚РЋР В РЎвЂ�Р РЋРІР‚С™Р В Р’В°Р В Р вЂ  Р В РўвЂ�Р В РЎвЂў Р В РЎвЂќР В РЎвЂўР РЋРІР‚С™Р В РЎвЂўР РЋР вЂљР В РЎвЂўР В РЎвЂ“Р В РЎвЂў Р РЋРІР‚С™Р В Р’В°Р В РІвЂћвЂ“Р В РЎпїЅР В Р’ВµР РЋР вЂљ Р РЋР С“Р В РЎвЂ“Р В Р’ВµР В Р вЂ¦Р В Р’ВµР РЋР вЂљР В РЎвЂ�Р РЋР вЂљР РЋРЎвЂњР В Р’ВµР РЋРІР‚С™ Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР РЋР вЂљР РЋРІР‚в„–Р В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ
    timer.TIM_Period = TIMER_TIMPERIOD-1;

    //Р В РЎСџР РЋР вЂљР В Р’ВµР В РўвЂ�Р В РўвЂ�Р В Р’ВµР В Р’В»Р В РЎвЂ�Р РЋРІР‚С™Р В Р’ВµР В Р’В»Р РЋР Р‰ Р РЋРІР‚С™Р В Р’В°Р В РІвЂћвЂ“Р В РЎпїЅР В Р’ВµР РЋР вЂљР В Р’В°
    timer.TIM_ClockDivision = 0;
    //Р В РїС—Р…Р В Р вЂ¦Р В РЎвЂ�Р РЋРІР‚В Р В РЎвЂ�Р В Р’В°Р В Р’В»Р В РЎвЂ�Р В Р’В·Р В РЎвЂ�Р РЋР вЂљР РЋРЎвЂњР В Р’ВµР В РЎпїЅ TIM4 Р В Р вЂ¦Р В Р’В°Р РЋРІвЂљВ¬Р В РЎвЂ�Р В РЎпїЅР В РЎвЂ� Р В Р’В·Р В Р вЂ¦Р В Р’В°Р РЋРІР‚РЋР В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р РЋР РЏР В РЎпїЅР В РЎвЂ�
    TIM_TimeBaseInit(TIM4, &timer);

}


void init_ADC() {
    ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef adc_init;
    /* Р РЋР вЂљР В Р’В°Р В Р’В·Р РЋР вЂљР В Р’ВµР РЋРІвЂљВ¬Р В Р’В°Р В Р’ВµР В РЎпїЅ Р РЋРІР‚С™Р В Р’В°Р В РЎвЂќР РЋРІР‚С™Р В РЎвЂ�Р РЋР вЂљР В РЎвЂўР В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ AР В Р’В¦Р В РЎСџ1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* Р РЋР С“Р В Р’В±Р РЋР вЂљР В Р’В°Р РЋР С“Р РЋРІР‚в„–Р В Р вЂ Р В Р’В°Р В Р’ВµР В РЎпїЅ Р В Р вЂ¦Р В Р’В°Р РЋР С“Р РЋРІР‚С™Р РЋР вЂљР В РЎвЂўР В РІвЂћвЂ“Р В РЎвЂќР В РЎвЂ� Р В РЎвЂ™Р В Р’В¦Р В РЎСџ */
    ADC_DeInit();

    /* Р В РЎвЂ™Р В Р’В¦Р В РЎСџ Р РЋР вЂљР В Р’В°Р В Р’В±Р В РЎвЂўР РЋРІР‚С™Р В Р’В°Р РЋР вЂ№Р РЋРІР‚С™ Р В Р вЂ¦Р В Р’ВµР В Р’В·Р В Р’В°Р В Р вЂ Р В РЎвЂ�Р РЋР С“Р В РЎвЂ�Р В РЎпїЅР В РЎвЂў */
    adc_init.ADC_Mode = ADC_Mode_Independent;
    adc_init.ADC_Prescaler = ADC_Prescaler_Div2;

    /* Р В Р вЂ Р РЋРІР‚в„–Р В РЎвЂќР В Р’В»Р РЋР вЂ№Р РЋРІР‚РЋР В Р’В°Р В Р’ВµР В РЎпїЅ scan conversion */
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    /* Р В РЎСљР В Р’Вµ Р В РўвЂ�Р В Р’ВµР В Р’В»Р В Р’В°Р РЋРІР‚С™Р РЋР Р‰ Р В РўвЂ�Р В Р’В»Р В РЎвЂ�Р РЋРІР‚С™Р В Р’ВµР В Р’В»Р РЋР Р‰Р В Р вЂ¦Р РЋРІР‚в„–Р В Р’Вµ Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР В РЎвЂўР В Р’В±Р РЋР вЂљР В Р’В°Р В Р’В·Р В РЎвЂўР В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р РЋР РЏ */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

    /* Р В РЎСљР В Р’В°Р РЋРІР‚РЋР В РЎвЂ�Р В Р вЂ¦Р В Р’В°Р РЋРІР‚С™Р РЋР Р‰ Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР В РЎвЂўР В Р’В±Р РЋР вЂљР В Р’В°Р В Р’В·Р В РЎвЂўР В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ Р В РЎвЂ”Р РЋР вЂљР В РЎвЂўР В РЎвЂ“Р РЋР вЂљР В Р’В°Р В РЎпїЅР В РЎпїЅР В Р вЂ¦Р В РЎвЂў, Р В Р’В° Р В Р вЂ¦Р В Р’Вµ Р В РЎвЂ”Р В РЎвЂў Р РЋР С“Р РЋР вЂљР В Р’В°Р В Р’В±Р В Р’В°Р РЋРІР‚С™Р РЋРІР‚в„–Р В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р РЋР вЂ№ Р РЋРІР‚С™Р РЋР вЂљР В РЎвЂ�Р В РЎвЂ“Р В Р’ВµР РЋР вЂљР В Р’В° */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = 0;
    /* 12 Р В Р’В±Р В РЎвЂ�Р РЋРІР‚С™Р В Р вЂ¦Р В РЎвЂўР В Р’Вµ Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР В РЎвЂўР В Р’В±Р РЋР вЂљР В Р’В°Р В Р’В·Р В РЎвЂўР В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ. Р РЋР вЂљР В Р’ВµР В Р’В·Р РЋРЎвЂњР В Р’В»Р РЋР Р‰Р РЋРІР‚С™Р В Р’В°Р РЋРІР‚С™ Р В Р вЂ  12 Р В РЎпїЅР В Р’В»Р В Р’В°Р В РўвЂ�Р РЋРІвЂљВ¬Р В РЎвЂ�Р РЋРІР‚В¦ Р РЋР вЂљР В Р’В°Р В Р’В·Р РЋР вЂљР РЋР РЏР В РўвЂ�Р В Р’В°Р РЋРІР‚В¦ Р РЋР вЂљР В Р’ВµР В Р’В·Р РЋРЎвЂњР В Р’В»Р РЋР Р‰Р РЋРІР‚С™Р В Р’В°Р РЋРІР‚С™Р В Р’В° */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

    /* Р В РЎвЂ�Р В Р вЂ¦Р В РЎвЂ�Р РЋРІР‚В Р В РЎвЂ�Р В Р’В°Р В Р’В»Р В РЎвЂ�Р В Р’В·Р В Р’В°Р РЋРІР‚В Р В РЎвЂ�Р РЋР РЏ */
    ADC_CommonInit(&adc_init);

    ADC_Init(ADC1, &ADC_InitStructure);
    /* Р В РІР‚в„ўР В РЎвЂќР В Р’В»Р РЋР вЂ№Р РЋРІР‚РЋР В Р’В°Р В Р’ВµР В РЎпїЅ Р В РЎвЂ™Р В Р’В¦Р В РЎСџ1 */
    ADC_Cmd(ADC1, ENABLE);

}

/*******************************************************************/

/* This funcion initializes the USART1 peripheral
 *
 * Arguments: baudrate --> the baudrate at which the USART is
 *                         supposed to operate
 */
void init_USART1(uint32_t baudrate) {

    /* This is a concept that has to do with the libraries provided by ST
     * to make development easier the have made up something similar to
     * classes, called TypeDefs, which actually just define the common
     * parameters that every peripheral needs to work correctly
     *
     * They make our life easier because we don't have to mess around with
     * the low level stuff of setting bits in the correct registers
     */
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    // NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

    /* enable APB2 peripheral clock for USART1
     * note that only USART1 and USART6 are connected to APB2
     * the other USARTs are connected to APB1
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* enable the peripheral clock for the pins used by
     * USART1, PB6 for TX and PB7 for RX
     */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* This sequence sets up the TX and RX pins
     * so they work correctly with the USART1 peripheral
     */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins 6 (TX) and 7 (RX) are used
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;           // the pins are configured as alternate function so the USART peripheral has access to them
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;      // this defines the IO speed and has nothing to do with the baudrate!
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         // this defines the output type as push pull mode (as opposed to open drain)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;           // this activates the pullup resistors on the IO pins
    GPIO_Init(GPIOB, &GPIO_InitStruct);                 // now all the values are passed to the GPIO_Init() function which sets the GPIO registers

    /* The RX and TX pins are now connected to their AF
     * so that the USART1 can take over control of the
     * pins
     */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    /* Now the USART_InitStruct is used to define the
     * properties of USART1
     */
    USART_InitStruct.USART_BaudRate = baudrate;             // the baudrate is set to the value we passed into this init function
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
    USART_InitStruct.USART_StopBits = USART_StopBits_1;     // we want 1 stop bit (standard)
    USART_InitStruct.USART_Parity = USART_Parity_No;        // we don't want a parity bit (standard)
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
    USART_Init(USART1, &USART_InitStruct);                  // again all the properties are passed to the USART_Init function which takes care of all the bit setting


    /* Here the USART1 receive interrupt is enabled
     * and the interrupt controller is configured
     * to jump to the USART1_IRQHandler() function
     * if the USART1 receive interrupt occurs
     */
    //  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

    //  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;        // we want to configure the USART1 interrupts
    //  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
    //  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       // this sets the subpriority inside the group
    //  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          // the USART1 interrupts are globally enabled
    //  NVIC_Init(&NVIC_InitStructure);                          // the properties are passed to the NVIC_Init function which takes care of the low level stuff

    // finally this enables the complete USART1 peripheral
    USART_Cmd(USART1, ENABLE);
}

void USART_puts(USART_TypeDef *USARTx, volatile char *s) {

    while (*s) {
        // wait until data register is empty
        while ( !(USARTx->SR & 0x00000040) );
        USART_SendData(USARTx, *s);
        *s++;
    }
}

/**
* First init
*/
void firstInit() {

    init_GPIO();                //GPIO init
    init_USART1(MIDI_BAUDRATE); //Midi init

    //       USBD_Init(&USB_OTG_dev,
    // #ifdef USE_USB_OTG_HS
    //             USB_OTG_HS_CORE_ID,
    // #else
    //             USB_OTG_FS_CORE_ID,
    // #endif
    //             &USR_desc,
    //             &AUDIO_cb,
    //             &USR_cb);

    //First port init, all for high
    GPIOB->BSRRL = 0xFC07;  // B0-B2, B10-B15
    GPIOC->BSRRL = 0x30;    // C4-C5
    GPIOD->BSRRL = 0x300;   // D8-D9
    GPIOE->BSRRL = 0xFF80;  // E7-E15

    //Р В РЎСљР В Р’В°Р РЋР С“Р РЋРІР‚С™Р РЋР вЂљР В Р’В°Р В РЎвЂ�Р В Р вЂ Р В Р’В°Р В Р’ВµР В РЎпїЅ Р РЋРІР‚С™Р В Р’В°Р В РІвЂћвЂ“Р В РЎпїЅР В Р’ВµР РЋР вЂљ Р В РўвЂ�Р В Р’В»Р РЋР РЏ Р В РЎвЂ“Р В Р’ВµР В Р вЂ¦Р В Р’ВµР РЋР вЂљР В Р’В°Р РЋРІР‚В Р В РЎвЂ�Р В РЎвЂ� Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР РЋР вЂљР РЋРІР‚в„–Р В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р РЋР РЏ Р В РЎвЂ”Р В РЎвЂў Р В РЎвЂўР В Р’В±Р В Р вЂ¦Р В РЎвЂўР В Р вЂ Р В Р’В»Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р РЋР вЂ№ (Р В РЎвЂ”Р В Р’ВµР РЋР вЂљР В Р’ВµР В РЎвЂ”Р В РЎвЂўР В Р’В»Р В Р вЂ¦Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р РЋР вЂ№)
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    //Р В РІР‚вЂќР В Р’В°Р В РЎвЂ”Р РЋРЎвЂњР РЋР С“Р В РЎвЂќР В Р’В°Р В Р’ВµР В РЎпїЅ Р РЋРІР‚С™Р В Р’В°Р В РІвЂћвЂ“Р В РЎпїЅР В Р’ВµР РЋР вЂљ
    TIM_Cmd(TIM4, ENABLE);

    //Р В Р’В Р В Р’В°Р В Р’В·Р РЋР вЂљР В Р’ВµР РЋРІвЂљВ¬Р В Р’В°Р В Р’ВµР В РЎпїЅ Р РЋР С“Р В РЎвЂўР В РЎвЂўР РЋРІР‚С™Р В Р вЂ Р В Р’ВµР РЋРІР‚С™Р РЋР С“Р РЋРІР‚С™Р В Р вЂ Р РЋРЎвЂњР РЋР вЂ№Р РЋРІР‚В°Р В Р’ВµР В Р’Вµ Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР РЋР вЂљР РЋРІР‚в„–Р В Р вЂ Р В Р’В°Р В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ
    NVIC_EnableIRQ(TIM4_IRQn);

}

FlagStatus ADC_GetFlagStatus1(ADC_TypeDef* ADCx, uint8_t ADC_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_GET_FLAG(ADC_FLAG));

  /* Check the status of the specified ADC flag */
  if ((ADCx->SR & ADC_FLAG) != (uint8_t)RESET)
  {
    /* ADC_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* ADC_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the ADC_FLAG status */
  return  bitstatus;
}




 uint16_t readADC1(uint8_t channel){
     ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_28Cycles); 
    //Р вЂ™Р’В Р В Р вЂ¦Р В Р’В°Р РЋРІР‚РЋР В РЎвЂ�Р В Р вЂ¦Р В Р’В°Р В Р’ВµР В РЎпїЅР вЂ™Р’В Р РЋР вЂљР В Р’В°Р В Р’В±Р В РЎвЂўР РЋРІР‚С™Р РЋРЎвЂњ
     ADC_SoftwareStartConv(ADC1); 
     //Р вЂ™Р’В Р В Р’В¶Р В РўвЂ�Р РЋРІР‚пїЅР В РЎпїЅР вЂ™Р’В Р В РЎвЂ”Р В РЎвЂўР В РЎвЂќР В Р’В°Р вЂ™Р’В Р В РЎвЂ”Р РЋР вЂљР В Р’ВµР В РЎвЂўР В Р’В±Р РЋР вЂљР В Р’В°Р В Р’В·Р РЋРЎвЂњР В Р’ВµР РЋРІР‚С™Р РЋР С“Р РЋР РЏР вЂ™Р’В Р В Р вЂ¦Р В Р’В°Р В РЎвЂ”Р РЋР вЂљР РЋР РЏР В Р’В¶Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р В Р’ВµР вЂ™Р’В Р В Р вЂ Р вЂ™Р’В Р В РЎвЂќР В РЎвЂўР В РўвЂ�
    while(((ADC1->SR & ADC_FLAG_EOC)==RESET)){__NOP();} 
     //Р вЂ™Р’В Р В РЎвЂўР РЋРІР‚РЋР В РЎвЂ�Р РЋРІР‚В°Р В Р’В°Р В Р’ВµР В РЎпїЅР вЂ™Р’В Р РЋР С“Р РЋРІР‚С™Р В Р’В°Р РЋРІР‚С™Р РЋРЎвЂњР РЋР С“
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);   //Р В РІР‚СњР В РЎвЂўР В Р’В±Р В Р’В°Р В Р вЂ Р В РЎвЂ�Р В Р’В»Р вЂ™Р’В Р В Р вЂ Р вЂ™Р’В Р В РЎвЂќР В Р’В°Р РЋРІР‚РЋР В Р’ВµР РЋР С“Р РЋРІР‚С™Р В Р вЂ Р В Р’ВµР вЂ™Р’В Р РЋРІР‚С™Р В Р’ВµР РЋР С“Р РЋРІР‚С™Р В Р’В°
     //Р вЂ™Р’В Р В Р вЂ Р В РЎвЂўР В Р’В·Р В Р вЂ Р РЋР вЂљР В Р’В°Р РЋРІР‚В°Р В Р’В°Р В Р’ВµР В РЎпїЅР вЂ™Р’В Р РЋР вЂљР В Р’ВµР В Р’В·Р РЋРЎвЂњР В Р’В»Р РЋР Р‰Р РЋРІР‚С™Р В Р’В°Р РЋРІР‚С™
    return ADC_GetConversionValue(ADC1); 
		}
	
	
/**DocID022945 Rev 4 29/33
AN4073 Averaging of N-X ADC samples: source code
 * @brief Sort the N ADC samples
 * @param ADC samples to be sorted
 * @param Numbre of ADC samples to be sorted
 * @retval None
 */
void Sort_tab(uint16_t tab[], uint8_t lenght) {
    uint8_t l = 0x00, exchange = 0x01;
    uint16_t tmp = 0x00;
    /* Sort tab */
    while (exchange == 1) {
        exchange = 0;
        for (l = 0; l < lenght - 1; l++) {
            if ( tab[l] > tab[l + 1] ) {
                tmp = tab[l];
                tab[l] = tab[l + 1];
                tab[l + 1] = tmp;
                exchange = 1;
            }
        }
    }
}

/**
 * @brief Get the average of N-X ADC samples
 * @param Numbre of ADC samples to be averaged
 * @param Numbre of ADC samples to be averaged
 * @retval The average value
 */

uint16_t ADC_last;

uint16_t ADC_GetSample(uint16_t delta) {
    uint16_t adc_sample, adc_change;
    adc_sample = readADC1(ADC_Channel_11);
    if (adc_sample > ADC_last) {
        adc_change = adc_sample - ADC_last;
    } else {
        adc_change = ADC_last - adc_sample;
    }
    if (adc_change > delta) {
        ADC_last = adc_sample;
        return adc_sample >> 5;
    } else {
        return ADC_last >> 5;
    }
}

uint16_t ADC_GetSampleAvgNDeleteX(uint8_t N , uint8_t X) {
    uint32_t avg_sample = 0x00;
    uint16_t adc_sample[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t index = 0x00;
    for (index = 0x00; index < N; index++) {
        /* ADC start conv */
        //        ADC_SoftwareStartConv(ADC1);
        /* Wait end of conversion */
        //        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        /* Store ADC samples */
        // adc_sample[index] = ADC_GetConversionValue(ADC1);
        adc_sample[index] = readADC1(ADC_Channel_11);
    }
    /* Sort the N-X ADC samples */
    Sort_tab(adc_sample, N);
    /* Add the N ADC samples */
    for (index = X / 2; index < N - X / 2; index++) {
        avg_sample += adc_sample[index];
    }
    /* Compute the average of N-X ADC sample */
    avg_sample /= N - X;
    /* Return average value */
    return avg_sample;
}


uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


Slider sliders;

int main(void) {


    firstInit();

    init_ADC();                                 //ADC init

    init_velocity();
	
	usb_init(); //Init everything for midiUSB


    delayms(400);
    hd44780_init();
    hd44780_display( HD44780_DISP_ON, HD44780_DISP_CURS_ON, HD44780_DISP_BLINK_OFF );


    hd44780_write_string("FATARMINATOR");
    hd44780_goto(2, 4);
    hd44780_write_string("PROJECT  v0.1");

    GPIO_SetBits(GPIOD, GPIO_Pin_15);
    /* Р В РЎвЂєР РЋР С“Р В Р вЂ¦Р В РЎвЂўР В Р вЂ Р В Р вЂ¦Р В РЎвЂўР В РІвЂћвЂ“ Р РЋРІР‚В Р В РЎвЂ�Р В РЎвЂќР В Р’В» Р В РЎвЂ”Р РЋР вЂљР В РЎвЂўР В РЎвЂ“Р РЋР вЂљР В Р’В°Р В РЎпїЅР В РЎпїЅР РЋРІР‚в„– */
    while (1) {

        //Р В РЎСџР РЋР вЂљР В РЎвЂўР В Р вЂ Р В Р’ВµР РЋР вЂљР РЋР РЏР В Р’ВµР В РЎпїЅ, Р В Р’ВµР РЋР С“Р В Р’В»Р В РЎвЂ� Р В Р’В»Р В РЎвЂ� Р РЋР С“Р РЋРІР‚РЋР В РЎвЂ�Р РЋРІР‚С™Р В Р’В°Р В Р вЂ¦Р В Р вЂ¦Р РЋРІР‚в„–Р В Р’Вµ Р В Р вЂ¦Р В РЎвЂўР РЋРІР‚С™Р РЋРІР‚в„–
        checkNoteArray();

        //Р В РЎСџР РЋР вЂљР В РЎвЂўР В Р вЂ Р В Р’ВµР РЋР вЂљР В РЎвЂќР В Р’В° Р В РЎвЂ� Р В РЎвЂўР РЋРІР‚С™Р В РЎвЂ”Р РЋР вЂљР В Р’В°Р В Р вЂ Р В РЎвЂќР В Р’В° Р В Р’В±Р РЋРЎвЂњР РЋРІР‚С›Р РЋРІР‚С›Р В Р’ВµР РЋР вЂљР В Р’В° midi Р РЋР С“Р В РЎвЂўР В РЎвЂўР В Р’В±Р РЋРІР‚В°Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р В РІвЂћвЂ“
        sendMidiData(); 

    }
}

// this is the interrupt request handler (IRQ) for ALL USART1 interrupts
void USART1_IRQHandler(void) {

    // check if the USART1 receive interrupt flag was set
    if ( USART_GetITStatus(USART1, USART_IT_RXNE) ) {

        static uint8_t cnt = 0; // this counter is used to determine the string length
        char t = USART1->DR; // the character from the USART1 data register is saved in t

        /* check if the received character is not the LF character (used to determine end of string)
         * or the if the maximum string length has been been reached
         */
        if ( (t != 'n') && (cnt < MAX_STRLEN) ) {
            received_string[cnt] = t;
            cnt++;
        } else { // otherwise reset the character counter and print the received string
            cnt = 0;
            USART_puts(USART1, received_string);
        }
    }
}

/**
Р В РЎС›Р В Р’В°Р В РІвЂћвЂ“Р В РЎпїЅР В Р’ВµР РЋР вЂљ Р В РўвЂ�Р В Р’В»Р РЋР РЏ Р РЋРІР‚РЋР РЋРІР‚С™Р В Р’ВµР В Р вЂ¦Р В РЎвЂ�Р РЋР РЏ Р РЋР С“Р В РЎвЂўР РЋР С“Р РЋРІР‚С™Р В РЎвЂўР РЋР РЏР В Р вЂ¦Р В РЎвЂ�Р РЋР РЏ Р В РЎвЂќР В Р’В»Р В Р’В°Р В Р вЂ Р В РЎвЂ�Р РЋРІвЂљВ¬ Р В РЎвЂ� Р В РЎвЂќР В РЎвЂўР В Р вЂ¦Р РЋРІР‚С™Р РЋР вЂљР В РЎвЂўР В Р’В»Р В Р’В»Р В Р’ВµР РЋР вЂљР В РЎвЂўР В Р вЂ 
**/

uint16_t ticks_counter = 0x0000;

void TIM4_IRQHandler() {
    uint32_t i;
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {

        //Р В РЎвЂєР РЋРІР‚РЋР В РЎвЂ�Р РЋРІР‚В°Р В Р’В°Р В Р’ВµР В РЎпїЅ Р В Р’В±Р В РЎвЂ�Р РЋРІР‚С™
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

        // GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
        //Р В Р Р‹Р РЋРІР‚РЋР В РЎвЂ�Р РЋРІР‚С™Р РЋРІР‚в„–Р В Р вЂ Р В Р’В°Р В Р’ВµР В РЎпїЅ Р РЋР С“Р В РЎвЂўР РЋР С“Р РЋРІР‚С™Р В РЎвЂўР РЋР РЏР В Р вЂ¦Р В РЎвЂ�Р В Р’Вµ Р В РЎвЂќР В Р’В»Р В Р’В°Р В Р вЂ Р В РЎвЂ�Р РЋРІвЂљВ¬
        readKeyState();
        ticks_counter++;
        if (ticks_counter > 1000) { //Р В РЎвЂќР В РЎвЂўР В Р вЂ¦Р РЋРІР‚С™Р РЋР вЂљР В РЎвЂўР В Р’В»Р В Р’В»Р В Р’ВµР РЋР вЂљР РЋРІР‚в„– Р В РЎвЂ”Р РЋР вЂљР В РЎвЂўР В Р вЂ Р В Р’ВµР РЋР вЂљР РЋР РЏР В РЎпїЅ Р РЋР вЂљР В Р’ВµР В Р’В¶Р В Р’Вµ, Р РЋРІР‚РЋР В Р’ВµР В РЎпїЅ Р В РЎвЂќР В Р’В»Р В Р’В°Р В Р вЂ Р В РЎвЂ�Р РЋРІвЂљВ¬Р В РЎвЂ�.
            ticks_counter = 0;
            i = ADC_GetSample(0x20);
            if (i != sliders.value) {
                __NOP();
                sliders.value = i;
                sendControlChange(64, i, 0);
                sendUSB_ControlChange(64, i, 0);							
            }

        }
    }

}