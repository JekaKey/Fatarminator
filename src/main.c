/*******************************************************************/

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define MAIN

#include "main.h"
#include "presets.h"
#include "fifo.h"
#include "keyboardscan.h"
#include "midi.h"
#include "hd44780.h"
#include "usb_init.h"
#include "controls.h"
#include "gpio_config.h"
#include "usb_midi_io.h"
#include "menu.h"

#include "ff.h"
#include "sd_fatfs.h"
#include "diskio.h"

#include "leds.h"
#include "log.h"

#include "bootloader.h"

extern presetType Preset;
extern currentStateType Current_state;
extern calibrationType Calibration;
extern uint8_t okIO;//if this flag is zero all I/O operations will be canceled.
extern uint32_t timerTicks;
extern timer_counter_t temp_msg_timer_counter; //defined in menu.c
extern timer_counter_t MSC_stop_timer_counter;

void delay(volatile uint32_t c) {
	while (--c) {
		__NOP();
	}
}

void delayms(volatile uint32_t c) {
	c++;
	while (--c) {
		delay(23080);
	}
}

/**
 * First init
 */
static void firstInit(void) {

	//Hardware init
	GPIO_init();
	USART1_init();//midi
	USART6_init();//debug
	usb_midi_init(); //Init everything for midiUSB

	ADC_init_all(); //ADC init
	SD_DMA_activate();
	//Display
	delayms(400);
	hd44780_init();
	hd44780_display( HD44780_DISP_ON, HD44780_DISP_CURS_OFF, HD44780_DISP_BLINK_OFF);
	hd44780_message_center(APP_NAME, 1);
	hd44780_message_center(APP_VERSION, 2);

}



//	Start key scan timer
static void Timer_init(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
	NVIC_EnableIRQ(TIM4_IRQn);
}




int main(void) {
	BootLoaderStart();
	firstInit();
    set_defaults_all(&Preset, &Calibration);
    delayms(3000);
    if (!start_load_all(&Preset, &Calibration))
		   set_okIOzero();
	interface_init(Current_state.preset_name);
	calibration_init(Current_state.calibration_name);

    Timer_init();

	//Main loop
	while (1) {

		//Check note array to calculate velocity
		checkNoteArray(&Preset);

		//TODO: why presets in arguments?
		checkSliders_events(Preset.sliders, Preset.AnalogMidiEnable);
		checkButtons_events(Preset.buttons, Preset.AnalogMidiEnable);
		//Send/receive midi data
		receiveMidiData();
		sendMidiData();
		checkTimer(&temp_msg_timer_counter, MES_TIMER_END);
		checkTimer(&MSC_stop_timer_counter, MES_MSCSTOP_TIMER_END);
	}
}

/**
 Timer 4 interrupt
 **/

void TIM4_IRQHandler() {

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {

		//TODO: Rewrite this w/o SPL
		//Clear interrupt bit
		timerTicks++;
//		readKeyChunk();
		readKeyState();
		read_controls(Preset.sliders, Calibration.calibr);
		read_buttons_state();
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}

}

