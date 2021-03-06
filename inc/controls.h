#ifndef CONTROLS__H
#define CONTROLS__H

#include "stm32f4xx.h"
#include "timer.h"
#include "stm32f4xx_gpio.h"
#include "filter.h"

#define MAX_DELTA_COUNTER 8 //maximum number of count/average in integral delta

#define SLIDERS_DELTA_S 32 //Delta in a level filter algorithm
#define SLIDERS_DELTA_P 32
#define SLIDERS_DELTA_AT 64
#define SLIDERS_DELTA_PI 64
#define SLIDERS_DELTA_MOD 32
#define SLIDERS_DELTA_SEARCH 0x200

#define SLIDERS_S_GAP 2
#define SLIDERS_R_GAP 2
#define SLIDERS_P_GAP 2
#define SLIDERS_PI_GAP 3
#define SLIDERS_AT_GAP 3
#define SLIDERS_MOD_GAP 3

#define SLIDERS_DEAD 0
#define SLIDERS_PI_DEAD 20

#define ADC_MAX_VALUE 0x0FFF

#define SLIDERS_MUX_DELAY 40/TIMER_TIMPERIOD //Delay in cycles after multiplexors switch
#define SLIDERS_MEASURE_NUM 20
#define SLIDERS_AMOUNT 24  //Number of sliders in the piano
#define BUTTONS_AMOUNT 8  //Number of user buttons in the piano
#define CALIBRATE_COUNTER_MAX 3*MEDIAN_FILTER_SIZE

#define ENCODER_SPEED1 4000//number of timer ticks to estimate en encoder speed, low speed
#define ENCODER_SPEED2 1000 //average speed
#define ENCODER_SPEED3 200 //fast speed

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
#define SLIDER_P1 11
#define SLIDER_P2 2
#define SLIDER_P3 5
#define SLIDER_EMPTY 8

#define KNOBS_N 8
#define PEDALS_N 3
#define SLIDERS_N 9

#define SLIDER_S_MIN_IN 300
#define SLIDER_S_MAX_IN 4000
#define SLIDER_S_MIN_OUT 0
#define SLIDER_S_MAX_OUT 127


#define SLIDER_R_MIN_IN 300
#define SLIDER_R_MAX_IN 4000
#define SLIDER_R_MIN_OUT 0
#define SLIDER_R_MAX_OUT 127

#define SLIDER_P_MIN_IN 650
#define SLIDER_P_MAX_IN 3850
#define SLIDER_P_MIN_OUT 0
#define SLIDER_P_MAX_OUT 127


#define SLIDER_PITCH_MIN_IN 1419
#define SLIDER_PITCH_MAX_IN 2584
#define SLIDER_PITCH_MIN_OUT 0
#define SLIDER_PITCH_MAX_OUT 16383

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


#define BUTTON_MAX_STATE 50  //a button should be pressed during BUTTON_MAX_STATE cycles to send message
#define BUTTON_LEVEL 5

#define BUTTON_PAGEUP 0
#define BUTTON_PAGEDOWN 2
#define BUTTON_PANIC 4
#define BUTTON_EDIT 6
#define BUTTON_CHANNEL 1
#define BUTTON_BANK 3
#define BUTTON_PROGRAM 5
#define BUTTON_STORAGE 7
#define BUTTON_ENTER 10

#define BUTTON_LEFT 11
#define BUTTON_RIGHT 12
#define BUTTON_RECORD 13
#define BUTTON_PLAY 14
#define BUTTON_STOP 15

#define BUTTON_B1 16
#define BUTTON_B2 17
#define BUTTON_B3 18
#define BUTTON_B4 19
#define BUTTON_B5 20
#define BUTTON_B6 21
#define BUTTON_B7 22
#define BUTTON_B8 23

#define ENCODER_LEFT1 25
#define ENCODER_RIGHT1 26
#define ENCODER_LEFT2 27
#define ENCODER_RIGHT2 28
#define ENCODER_LEFT3 29
#define ENCODER_RIGHT3 30


#define BTN_TYPE_SIMPLE 0
#define BTN_TYPE_PUSH 1
#define BTN_TYPE_SWITCH 2

#define MES_SLIDER_SHOW 31
#define MES_SLIDER_EDGE 32
#define MES_SLIDER_FOUND 33
#define MES_SLIDER_MENU_FOUND 34
#define MES_KEY_SEEK 35
#define MES_SHOW_TEMP_MSG 36
#define MES_TIMER_END 37
#define MES_MSC_STOP 38
#define MES_MSCSTOP_TIMER_END 39

/*the events to send to controls FIFO*/
#define MES_REDRAW 40
#define MES_YES 41


/*************************************/


typedef enum {SLIDERS_WORK=0, SLIDERS_FOUND, SLIDERS_CALIBRATE, SLIDERS_EDGE, SLIDERS_MENU_SEARCH} sliders_state_t;

typedef enum {ENCODER_SLOW, ENCODER_AVERAGE, ENCODER_FAST} encoder_speed_t;

typedef struct {
	uint16_t min_in_value;
	uint16_t max_in_value;
	uint16_t delta;
	uint8_t gap;
	uint8_t dead;
}Calibration_slider_type;

typedef struct {
	uint8_t active;
	uint8_t reverse;
	uint8_t channel;
	uint8_t binary;
	uint8_t event;
	uint16_t min_out_value;
	uint16_t max_out_value;
} Slider_type;

typedef struct {
	GPIO_TypeDef * port;
	uint16_t pin;
} button_port_type;

typedef struct {
	uint8_t active;
	uint8_t type;
	uint8_t channel;// MIDI channel
	uint8_t event;
	uint8_t on;
	uint8_t off;
} Button_type;

void send_message(uint8_t mes);
uint16_t get_slider_event(void); //get slider event from FIFO, returns 0 if there is no events
uint16_t get_pitch_event(void);
/*!!! In all fuctions with pointer argument as Slider_type*  or Calibration_slider_type MUST point to an array of SLIDERS_AMOUNT*/
void slider_midi_send(uint16_t value, Slider_type* sliders, uint8_t analog);
void pitch_midi_send(uint16_t value, uint8_t channel, uint8_t analog);
void button_midi_send(uint16_t value, Button_type* buttons, uint8_t analog);
void ADC_init_all(void);
void read_controls(Slider_type* sliders, Calibration_slider_type* cal);
void read_buttons_state(void);
void slider_init_struct(Slider_type* sliders, Calibration_slider_type* sliders_calibr);
void calculate_sliders_constants(Slider_type* sliders, Calibration_slider_type* sliders_calibr);
/*Init all calibration parameters with default values.  */
void sliders_calibr_set_defaults(Calibration_slider_type* sliders_calibr);
void sliders_set_defaults(Slider_type* sliders, Calibration_slider_type* sliders_calibr); //Init all sliders parameters if they are not loaded from preset
void buttons_set_defaults(Button_type* but);
void checkSliders_events(Slider_type* sliders, uint8_t analog);
void checkButtons_events(Button_type* buttons, uint8_t analog);
void start_sliders_calibration(void);
void buttons_delay(void) __attribute__((optimize(0)));

#endif //CONTROLS__H
