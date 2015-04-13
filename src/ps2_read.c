#include "common.h"
#include "uart.h"
#include "oc.h"
#include "timer.h"
#include "ui.h"
#include <p24FJ128GB206.h>
#include "pin.h"
#include "keymap.h"

void main(void){

	init_oc();
	init_uart();
	init_timer();
	init_ui();
	init_pin();

	volatile uint8_t data;
	volatile uint8_t new_command = 0;
	char input = 'a';
	uint8_t started, bits;

	pin_digitalIn(&D[7]);//clock input
	pin_digitalIn(&D[5]);//data input

	INTCON2bits.INT4EP = 1;//interrupt on negative edge
	INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC3bits.INT4IE = 1;//enable interrupt
	RPINR2bits.INT4R = 0xC;//set interrupt to RPin 7

	IFS3bits.INT4IF = 0;//clear flag

	pin_clear(&D[6]);

	while(1){	
		if(new_command){
			input =  pgm_read_byte(&(keymap[data])); //grab character from array
			new_command = 0;
		}
	}
}

void __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void){
	
	if(!started){
		if(!pin_read(&D[6])){
			started = 1;
			bit_count = 0;
			data = 0;
			return;
		}
	}
	else if(bit_count<=8){
		data |=(pin_read(&D[6])<<bit_count)
	
		bit_count++;
		return;
	}
	else{
		bit_count = 0;
		started = 0;
		new_command=1;
	}

	IFS3bits.INT4IF = 0;
}



