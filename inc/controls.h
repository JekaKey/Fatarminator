#ifndef CONTROLS__H
#define CONTROLS__H

#include "stm32f4xx.h"
#include "timer.h"
#include "stm32f4xx_gpio.h"

#define SLIDERS_DELTA_S 0x08 //Delta in a level filter algorithm
#define SLIDERS_DELTA_P 0x10
#define SLIDERS_DELTA_AT 0x10

#define SLIDERS_MUX_DELAY 80/TIMER_TIMPERIOD //Delay in cycles after multipexors switch
#define SLIDERS_MEASURE_NUM 20
#define SLIDERS_AMOUNT 24  //Number of sliders in the piano
#define BUTTONS_AMOUNT 13  //Number of user buttons in the piano

#define SLIDER_S1 14
#define SLIDER_S2 4
#define SLIDER_S3 10
#define SLIDER_S4 22
#define SLIDER_S5 16
#define SLIDER_S6 7
#define SLIDER_S7 13
#define SLIDER_S8 1
#define SLIDER_S9 19

#define SLIDER_R1 9
#define SLIDER_R2 0
#define SLIDER_R3 3
#define SLIDER_R4 6
#define SLIDER_R5 15
#define SLIDER_R6 21
#define SLIDER_R7 18
#define SLIDER_R8 12

#define SLIDER_AT 17
#define SLIDER_PITCH 23
#define SLIDER_MOD 20
#define SLIDER_P1 5
#define SLIDER_P2 2
#define SLIDER_P3 11
#define SLIDER_EMPTY 8


#define SLIDER_S_MIN_IN 0
#define SLIDER_S_MAX_IN 4050
#define SLIDER_S_MIN_OUT 0
#define SLIDER_S_MAX_OUT 127

#define SLIDER_R_MIN_IN 850
#define SLIDER_R_MAX_IN 3420
#define SLIDER_R_MIN_OUT 0
#define SLIDER_R_MAX_OUT 127

#define SLIDER_P_MIN_IN 400
#define SLIDER_P_MAX_IN 1250
#define SLIDER_P_MIN_OUT 0
#define SLIDER_P_MAX_OUT 127

#define SLIDER_PITCH_MIN_IN 500
#define SLIDER_PITCH_MAX_IN 2000
#define SLIDER_PITCH_MIN_OUT 0
#define SLIDER_PITCH_MAX_OUT 127

#define SLIDER_MOD_MIN_IN 500
#define SLIDER_MOD_MAX_IN 2000
#define SLIDER_MOD_MIN_OUT 0
#define SLIDER_MOD_MAX_OUT 127

#define SLIDER_AT_MIN_IN 0
#define SLIDER_AT_MAX_IN 4200
#define SLIDER_AT_MIN_OUT 0
#define SLIDER_AT_MAX_OUT 127

#define ENCODER1_PORT            GPIOD
#define ENCODER1_PIN        GPIO_Pin_0

#define ENCODER2_PORT            GPIOD
#define ENCODER2_PIN        GPIO_Pin_1

#define LED_EN_PORT              GPIOB
#define LED_EN_PIN         GPIO_Pin_7

#define BUTTON0_PORT              GPIOD
#define BUTTON0_PIN         GPIO_Pin_4

#define BUTTON1_PORT              GPIOD
#define BUTTON1_PIN         GPIO_Pin_5

#define BUTTON2_PORT              GPIOD
#define BUTTON2_PIN         GPIO_Pin_6


typedef struct {
	uint16_t min_in_value;
	uint16_t max_in_value;
	uint16_t delta;
}Calibration_slider_type;

typedef struct {
	uint8_t active;
	uint8_t reverse;
	uint8_t channel;
	uint8_t event;
	uint8_t value;
	uint8_t min_out_value;
	uint8_t max_out_value;
	double a; // a & b are the parameters in slider formula y=a*x+b;
	double b;
} Slider_type;

typedef struct {
	GPIO_TypeDef * port;
	uint16_t pin;
} button_port_type;

typedef struct {
	uint8_t active;
	uint8_t type; //0-CC, 1 -NOTE, 2 MMS
	uint8_t channel;// MIDI channel
	uint8_t toggle; //0-momentary, !=0 toggle (work only for ��)
	uint8_t event;
	uint8_t on;
	uint8_t off;
} Button_type;


/*!!! In all fuctions with pointer argument as Slider_type*  or Calibration_slider_type MUST poit to array of SLIDERS_AMOUNT*/
void slider_midi_send(uint16_t value, Slider_type* sliders);
void ADC_init_all(void);
void read_controls(Slider_type* sliders, Calibration_slider_type* cal);
/*Init all calibration parameters with default values.  */
void sliders_calibr_set_defaults(Calibration_slider_type* sliders_calibr);
void sliders_set_defaults(Slider_type* sliders, Calibration_slider_type* sliders_calibr); //Init all sliders parameters if they are not loaded from preset
void buttons_set_defaults(Button_type* but);
void checkSliders_events(Slider_type* sliders);

void buttons_delay(void) __attribute__((optimize(0)));

#endif //CONTROLS__H
