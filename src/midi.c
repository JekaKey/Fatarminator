#include "midi.h"



void sendNoteOn(byte NoteNumber, word Velocity, byte Channel) {
  	FIFO_PUSH(midiMessagesArray, NoteOn ^ Channel);    //Событие и канал
    FIFO_PUSH(midiMessagesArray, NoteNumber);          //Номер ноты
    FIFO_PUSH(midiMessagesArray, (byte)(Velocity>>7)); //Скорость high 7 bit
}

void sendNoteOff(byte NoteNumber, word Velocity, byte Channel) {
    FIFO_PUSH(midiMessagesArray, NoteOff ^ Channel);   //Событие и канал
    FIFO_PUSH(midiMessagesArray, NoteNumber);          //Номер ноты
    FIFO_PUSH(midiMessagesArray, (byte)(Velocity>>7)); //Скорость high 7 bit
}



void sendControlChange(byte ControlNumber, byte ControlValue, byte Channel) {
    FIFO_PUSH(midiMessagesArray, ControlChange ^ Channel);      //Событие и канал
    FIFO_PUSH(midiMessagesArray, ControlNumber);                //Тип события
    FIFO_PUSH(midiMessagesArray, ControlValue);                 //Значение
}



/**
* Отправка миди данных из буффера
*/
void sendMidiData(void) {

		uint8_t test;
	
	  test = FIFO_COUNT(midiMessagesArray);
    if (test != 0) {
        if ((USART1->SR & 0x00000040)) {
            USART_SendData(USART1, FIFO_FRONT(midiMessagesArray));
            FIFO_POP(midiMessagesArray);

        }
    }
}