#include "common.h"
#include "uart.h"//Don't use the UART! It disables my interrupts.
#include "oc.h"
#include "timer.h"
#include "ui.h"
#include <p24FJ128GB206.h>
#include "pin.h"
#include "keymap.h"

//signal display to start
#define PIC2 1

//breakbeam interrupts- don't forget to attach the END interrupt to the arduino
#define PENNY 8
#define START 4

//actuator outputs
#define SERVO_1 3
#define SERVO_2 5

#define BALL_SERVO 6

#define MOTOR_4 0

//Inputs
#define SERVO_1_IN 10
#define SERVO_2_IN 11

#define MOTOR_4_FAST 5
#define MOTOR_4_SLOW 7

//for TS53 servo, change if we change servos
#define INTERVAL .02
#define MAXWIDTH .003
#define MINWIDTH .0006

#define NUM_SERVOS 2

int flip(int flipped, int pin){
	if(flipped){
		pin_write(&D[pin],55534);
		led_on(&led2);
		return 0;
	}
	else{		
		led_off(&led2);
		pin_write(&D[pin],0);
		return 1;
	}
}

//Edit this
int slow(int speed, int pin){
	if(speed){
		pin_write(&D[pin],55534);
		led_on(&led2);
		return 0;
	}
	else{		
		led_off(&led2);
		pin_write(&D[pin],0);
		return 1;
	}
}

//interrupt globals
volatile int active = 0;//game in progress, controls active

void main(void){
	init_oc();
	init_timer();
	init_ui();
	init_pin();

	//interrupt triggered
	pin_digitalOut(&D[PIC2]);
	pin_clear(&D[PIC2]);
	pin_digitalOut(&D[BALL_SERVO]);

	//servos
	pin_digitalOut(&D[SERVO_1]);
	pin_digitalOut(&D[SERVO_2]);

	pin_digitalOut(&D[MOTOR_4]);

	//interrupts
	pin_digitalIn(&D[PENNY]);
	pin_digitalIn(&D[START]);

	pin_digitalIn(&D[SERVO_1_IN]);
	pin_digitalIn(&D[SERVO_2_IN]);

	pin_digitalIn(&D[MOTOR_4_SLOW]);
	pin_digitalIn(&D[MOTOR_4_FAST]);


	int servo_inputs[2] = {SERVO_1_IN, SERVO_2_IN};
	int servo_outputs[2] = {SERVO_1, SERVO_2};

	//game start interrupt
	INTCON2bits.INT2EP = 1;//interrupt on negative edge
	IEC1bits.INT2IE = 1;//enable interrupt
	RPINR1bits.INT2R = 0x1D;//set interrupt to

	IFS1bits.INT2IF = 0;//clear flag

	INTCON2bits.INT4EP = 1;//interrupt on negative edge
	IEC3bits.INT4IE = 1;//enable interrupt
	RPINR2bits.INT4R = 0x4;//set interrupt to RPin 22

	IFS3bits.INT4IF = 0;//clear flag

	//servo values
	int flipped[2] = {0,0};//neither servo is flipped

	//servo pins
	oc_servo(&oc1,&D[SERVO_1], NULL,INTERVAL,MINWIDTH,MAXWIDTH,0);//gameplay servo
	oc_servo(&oc2,&D[SERVO_2], NULL,INTERVAL,MINWIDTH,MAXWIDTH,0);
	oc_servo(&oc3,&D[BALL_SERVO],&timer2,INTERVAL,MINWIDTH,MAXWIDTH,0);//ball dispensing servo
	
	//motor driving OC
	oc_pwm(&oc7, &D[MOTOR_4], NULL, 100, 55534);

	int i;

	while(1){
		active = 1;
		if(active){
			//read servo inputs and react
			for(i = 0; i < NUM_SERVOS; i++){
				if(!pin_read(&D[(servo_inputs[i])])){
					flipped[i] = flip(flipped[i],servo_inputs[i]);
				}
			}

			if(!pin_read(&D[MOTOR_4_FAST])){
				slow(1,MOTOR_4);

			}
			if(!pin_read(&D[MOTOR_4_SLOW])){
				slow(0,MOTOR_4);
			}

		}
	}
}


void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void){//penny insertion interrupt
		IFS1bits.INT2IF = 0;//clear flag
		pin_write(&D[BALL_SERVO],55534);
		active = 0;
		led_off(&led3);
}

void __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void){//game start interrupt

	IFS3bits.INT4IF = 0;//clear flag
	pin_set(&D[PIC2]);
	pin_write(&D[BALL_SERVO],0);
	active = 1;
	led_on(&led3);
}



	
