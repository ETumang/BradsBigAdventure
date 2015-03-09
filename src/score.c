#include "ui.h"
#include "common.h"
#include "pin.h"
#include <p24FJ128GB206.h>

volatile int score = 0;

void main(void){
	init_pin();
	init_ui();
	pin_digitalIn(&D[12]);
	//pin_digitalOut(&D[13]);
	score = 0;
	//pin_set(&D[13]);

	INTCON2bits.INT4EP = 1;//interrupt on negative edge
	INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC3bits.INT4IE = 1;//enable interrupt
	RPINR2bits.INT4R = 0x17;//set interrupt to RPin 22

	IFS3bits.INT4IF = 0;//clear flag
	led_on(&led2);

	while(1);//wait for interrupts

}

//ISR

void __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void){
	led_toggle(&led1);
	score++;
	IFS3bits.INT4IF = 0;
}
