#include "common.h"
#include "uart.h"//Don't use the UART! It disables my interrupts.
#include "oc.h"
#include "timer.h"
#include "ui.h"
#include <p24FJ128GB206.h>
#include "pin.h"
#include "keymap.h"

#define MAX_OUTPUT 2^15

#define MOTOR_BASE_SPEED MAX_OUTPUT>>8

#define SERVO 8
#define KEYBOARD_CLOCK 7//green
#define KEYBOARD_DATA 12//blue
#define MOTOR 6

#define DISPLAY 9

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

// int flip(int flipped){
// 	if(flipped){
// 		pin_write(&D[SERVO],MAX_OUTPUT);
// 		return 0;
// 	}
// 	else{		
// 		pin_write(&D[SERVO],0);
// 		return 1;
// 	}
// }

// uint16_t calc_speed(uint16_t speed, int m_dir){
// 	if(m_dir && speed < MAX_OUTPUT){//go faster
// 		return speed << 1;
// 	}
// 	else if(!m_dir && speed > 0) {//go slower
// 		return speed >> 1;
// 	}
// 	else{
// 		return speed;
// 	}
// }

void main(void){
	// init_oc();
	// init_timer();
	init_ui();
	init_pin();
	//init_uart();

	//switch controller, shouldn't do anything
	// char input = 'a';

	// pin_digitalOut(&D[DISPLAY]);
	// pin_digitalOut(&D[SERVO]);
	// pin_digitalOut(&D[MOTOR]);
	// pin_clear(&D[MOTOR]);


	//keyboard pins
	pin_digitalIn(&D[KEYBOARD_CLOCK]);//clock input-blue
	pin_digitalIn(&D[KEYBOARD_DATA]);//data input-green

	//keyboard interrupt
	INTCON2bits.INT3EP = 1;//interrupt on negative edge
	//INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC3bits.INT3IE = 1;//enable interrupt
	RPINR1bits.INT3R = 0x17;//set interrupt to RPin 23 (digital pin 12)

	IFS3bits.INT3IF = 0;//clear flag

	//servo values
	// int flipped = 0;

	// //servo pins
	// oc_servo(&oc2,&D[SERVO], &timer2,INTERVAL,MINWIDTH,MAXWIDTH,0);

	// //motor driving OC
	// oc_pwm(&oc1, &D[MOTOR], &timer1, 500, 65536 >> 1);

	// //motor variables
	// uint16_t speed = 65536 >> 1;

	led_on(&led3);
	//uart_putc(&uart1,'h');
	while(1){
		//uart_putc(&uart1,'m');
		//read the keyboard
	// 	if(new_command){
	// 		input = keymap[data]; //grab character from array		
	// 		new_command = 0;
	// 		//uart_putc(&uart1,input);
	// 	}
	// 	switch(input){
	// 		case START:
	// 			//start the timer
	// 			pin_set(&D[DISPLAY]);
	// 			input = 'a';
	// 		case END:
	// 			//stop the timer
	// 			pin_clear(&D[DISPLAY]);
	// 			pin_set(&D[DISPLAY]);
	// 			input = 'a';
	// 		case FLIP_SERVO:
	// 			flipped = flip(flipped);
	// 			input = 'a';
	// 		case ACCEL_MOTOR:
	// 			speed = calc_speed(speed, FASTER);
	// 			pin_write(&D[MOTOR],speed);
	// 			input = 'a';
	// 		case SLOW_MOTOR:
	// 			speed = calc_speed(speed, SLOWER);
	// 			pin_write(&D[MOTOR],speed);
	// 			input = 'a';
	// 	}
	}
}

void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void){//keyboard interrupt

	IFS3bits.INT3IF = 0;//clear flag
	led_toggle(&led2);
	
	// if(!started){
	// 	if(!pin_read(&D[KEYBOARD_DATA])){
	// 		started = 1;
	// 		data = 0;
	// 		new_command = 0;
	// 		return;
	// 	}
	// }
	// else if(bit_count<=8){
	// 	data |=(pin_read(&D[KEYBOARD_DATA])<<bit_count);
	// 	bit_count++;
	// 	return;
	// }
	// else{
	// 	bit_count = 0;
	// 	started = 0;
	// 	if(keyup){
	// 		keyup = 0;
	// 		return;
	// 	}
	// 	if(data == 240){
	// 		keyup = 1;
	// 		return;
	// 	}
	// 	new_command = 1;
	// }
}

	
