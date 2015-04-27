#include "common.h"
#include "uart.h"//Don't use the UART! It disables my interrupts.
#include "oc.h"
#include "timer.h"
#include "ui.h"
#include <p24FJ128GB206.h>
#include "pin.h"
#include "keymap.h"

//breakbeam interrupts
#define PENNY 2
#define START 3
#define END 4

//actuator outputs
#define SERVO_1 5
#define SERVO_2 6

#define BALL_SERVO 7

#define MOTOR_1_R 8
#define MOTOR_1_L 9

#define MOTOR_2_R 10
#define MOTOR_2_L 11

#define MOTOR_3_R 12
#define MOTOR_3_L 13


//for TS53 servo, change if we change servos
#define INTERVAL .02
#define MAXWIDTH .003
#define MINWIDTH .0006

//motor speeds
#define SLOWER 0
#define FASTER 1

int flip(int flipped){
	if(flipped){
		pin_write(&D[SERVO],65534);
		led_on(&led2);
		return 0;
	}
	else{		
		led_off(&led2);
		pin_write(&D[SERVO],0);
		return 1;
	}
}

int slow(int speed){
	if(speed){
		pin_write(&D[MOTOR],65534);
		led_on(&led2);
		return 0;
	}
	else{		
		led_off(&led2);
		pin_write(&D[MOTOR],0);
		return 1;
	}
}

//interrupt globals
volatile int active = 1;//game in progress, controls active

void main(void){
	init_oc();
	init_timer();
	init_ui();
	init_pin();

	pin_digitalOut(&D[DISPLAY]);
	pin_digitalOut(&D[SERVO]);
	pin_digitalOut(&D[MOTOR]);
	pin_digitalOut(&D[BALL_SERVO]);
	//pin_clear(&D[MOTOR]);

	pin_digitalIn(&D[PENNY]);
	pin_digitalIn(&D[START]);
	pin_digitalIn(&D[END]);

	pin_digitalIn(&D[SERVO_IN]);

	//pin_analogIn(&A[MOTOR_IN]);

	int done = 0;

	//game start interrupt
	INTCON2bits.INT2EP = 1;//interrupt on negative edge
	//INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC1bits.INT2IE = 1;//enable interrupt
	RPINR1bits.INT2R = 0xA;//set interrupt to RPin 10 (digital pin 2)

	IFS1bits.INT2IF = 0;//clear flag

	//game end interrupt
	INTCON2bits.INT3EP = 1;//interrupt on negative edge
	//INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC3bits.INT3IE = 1;//enable interrupt
	RPINR1bits.INT3R = 0x11;//set interrupt to RPin17 (digital pin 3)

	IFS3bits.INT3IF = 0;//clear flag

	//penny insertion interrupt
	INTCON2bits.INT4EP = 1;//interrupt on negative edge
	//INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC3bits.INT4IE = 1;//enable interrupt
	RPINR2bits.INT4R = 0x1D;//set interrupt to RPin 23 (digital pin 4)

	IFS3bits.INT4IF = 0;//clear flag

	//servo values
	int flipped = 0;

	//servo pins
	oc_servo(&oc2,&D[SERVO], &timer1,INTERVAL,MINWIDTH,MAXWIDTH,0);//gameplay servo
	oc_servo(&oc3,&D[BALL_SERVO],&timer2,INTERVAL,MINWIDTH,MAXWIDTH,0);//ball dispensing servo
	// oc_servo(&oc4,&D[SERVO],&timer3,INTERVAL,MINWIDTH,MAXWIDTH,0);

	//motor driving OC
	oc_pwm(&oc1, &D[MOTOR], NULL, 1000, 0);
	int speed = 1;

	pin_digitalOut(&D[11]);
	pin_write(&D[11],0);

	//motor variables

	led_on(&led1);

	while(1){
		pin_set(&D[DISPLAY]);
		if(active){

			if(!pin_read(&D[MOTOR_IN])){
				speed = slow(speed);
			}
			if(!pin_read(&D[SERVO_IN])){
				flipped = flip(flipped);
			}
		}
	}
}


void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void){//penny insertion interrupt
		IFS1bits.INT2IF = 0;//clear flag
		led_toggle(&led1);
		pin_write(&D[BALL_SERVO],55534);
}

void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void){//game start interrupt

	IFS3bits.INT3IF = 0;//clear flag
	//active = 1;
	pin_clear(&D[DISPLAY]);
	led_toggle(&led2);
	pin_write(&D[BALL_SERVO],0);
}


void __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void){//game loss interrupt

	IFS3bits.INT4IF = 0;//clear flag
	pin_clear(&D[DISPLAY]);
	//active = 0;
	led_toggle(&led3);
}

	
