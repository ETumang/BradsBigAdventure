#include "common.h"
#include "uart.h"
#include "oc.h"
#include "timer.h"
#include "ui.h"
#include <p24FJ128GB206.h>
#include "pin.h"
#include "keymap.h"

#define MAX_OUTPUT 2^15

#define MOTOR_BASE_SPEED MAX_OUTPUT>>8

#define SERVO 8
#define KEYBOARD_CLOCK 12//green
#define KEYBOARD_DATA 7//blue
#define MOTOR 9

#define FANS_1 10
#define FANS_2 11
#define FANS_3 13


//for TS53 servo, change if we change servos
#define INTERVAL .02
#define MAXWIDTH .003
#define MINWIDTH .0006

//control constants
#define START 'b'
#define END 'c'
#define FLIP_SERVO 'd'
#define SLOW_MOTOR 'e'
#define ACCEL_MOTOR 'f'

//motor speeds
#define SLOWER 0
#define FASTER 1

//keyboard reading variables
volatile uint8_t data;
volatile uint8_t new_command = 0;
volatile uint8_t started, bits;
volatile int bit_count = 0;
volatile int keyup = 0;//was the last code a keyup code?

int flip(int flipped){
	if(flipped){
		pin_write(&D[SERVO],MAX_OUTPUT);
		return 0;
	}
	else{		
		pin_write(&D[SERVO],0);
		return 1;
	}
}

uint16_t calc_speed(uint16_t speed, int m_dir){
	if(m_dir && speed < MAX_OUTPUT){//go faster
		return speed << 1;
	}
	else if(!m_dir && speed > 0) {//go slower
		return speed >> 1;
	}
	else{
		return speed;
	}
}

int toggle_fans(int fanSet){
	pin_clear(&D[FANS_1]);
	pin_clear(&D[FANS_2]);
	pin_clear(&D[FANS_3]);

  	switch(fanSet){
  		case FANS_1:
  			pin_set(&D[FANS_1]);
  		case FANS_2:
  			pin_set(&D[FANS_2]);
  		case FANS_3:
  			pin_set(&D[FANS_3]);
    }
    return fanSet;
}

void main(void){
	init_oc();
	init_uart();
	init_timer();
	init_ui();
	init_pin();

	//switch controller, shouldn't do anything
	char input = 'a';

	//keyboard pins
	pin_digitalIn(&D[KEYBOARD_CLOCK]);//clock input-blue
	pin_digitalIn(&D[KEYBOARD_DATA]);//data input-green

	//keyboard interrupt
	INTCON2bits.INT4EP = 1;//interrupt on negative edge
	INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC3bits.INT4IE = 1;//enable interrupt
	RPINR2bits.INT4R = 0x17;//set interrupt to RPin 22

	IFS3bits.INT4IF = 0;//clear flag

	//servo values
	int flipped = 0;

	//servo pins
	pin_digitalOut(&D[SERVO]);
	pin_clear(&D[SERVO]);

	//motor driving OC
	oc_pwm(&oc1, &D[MOTOR], NULL, 500, MOTOR_BASE_SPEED);

	//motor constants
	uint16_t speed = MOTOR_BASE_SPEED;

	//fan outputs
	pin_digitalOut(&D[FANS_1]);
	pin_digitalOut(&D[FANS_2]);
	pin_digitalOut(&D[FANS_3]);

	pin_clear(&D[FANS_1]);
	pin_set(&D[FANS_2]);
	pin_clear(&D[FANS_3]);

	while(1){
		//read the keyboard
		if(new_command){
			input = keymap[data]; //grab character from array
			uart_putc(&uart1, input);			
			new_command = 0;
		}
		switch(input){
			case START:
				//start the timer
				pin_set(&D[8]);
				input = 'a';
			case END:
				//stop the timer
				pin_clear(&D[8]);
				pin_set(&D[8]);
				input = 'a';
			case FLIP_SERVO:
				flipped = flip(flipped);
				input = 'a';
			case ACCEL_MOTOR:
				speed = calc_speed(speed, FASTER);
				pin_write(&D[MOTOR],speed);
				input = 'a';
			case SLOW_MOTOR:
				speed = calc_speed(speed, SLOWER);
				pin_write(&D[MOTOR],speed);
				input = 'a';
		}
	}
}

void __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void){//keyboard interrupt
	
	if(!started){
		if(!pin_read(&D[KEYBOARD_DATA])){
			started = 1;
			data = 0;
			new_command = 0;
			return;
		}
	}
	else if(bit_count<=8){
		data |=(pin_read(&D[KEYBOARD_DATA])<<bit_count);
		bit_count++;
		return;
	}
	else{
		bit_count = 0;
		started = 0;
		if(keyup){
			keyup = 0;
			return;
		}
		if(data == 240){
			keyup = 1;
			return;
		}
		new_command = 1;
	}
}

	
