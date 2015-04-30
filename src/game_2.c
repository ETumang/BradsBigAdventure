#include "common.h"
#include "uart.h"//Don't use the UART! It disables my interrupts.
#include "oc.h"
#include "timer.h"
#include "ui.h"
#include <p24FJ128GB206.h>
#include "pin.h"
#include "keymap.h"

//signal display to start
#define DISPLAY 1

//breakbeam interrupts- don't forget to attach the END interrupt to the arduino
#define PENNY 2
#define START 3

//actuator outputs
#define SERVO_1 4
#define SERVO_2 5

#define BALL_SERVO 6

#define MOTOR_1 7

#define MOTOR_2 8

#define MOTOR_3 9

#define MOTOR_4 0

//Inputs
#define SERVO_1_IN 10
#define SERVO_2_IN 11

#define MOTOR_1_FAST 12
#define MOTOR_1_SLOW 13
//repurposed analog pins
#define MOTOR_2_FAST 0
#define MOTOR_2_SLOW 1
#define MOTOR_3_FAST 2
#define MOTOR_3_SLOW 3
#define MOTOR_4_FAST 4
#define MOTOR_4_SLOW 5

//for TS53 servo, change if we change servos
#define INTERVAL .02
#define MAXWIDTH .003
#define MINWIDTH .0006

#define NUM_SERVOS 2
#define NUM_MOTORS_ANALOG 3

int flip(int flipped, int pin){
	if(flipped){
		pin_write(&D[pin],65534);
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
		pin_write(&D[pin],65534);
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
volatile int active = 1;//game in progress, controls active

void main(void){
	init_oc();
	init_timer();
	init_ui();
	init_pin();

	//interrupt triggered
	pin_digitalOut(&D[DISPLAY]);
	pin_digitalOut(&D[BALL_SERVO]);

	//servos
	pin_digitalOut(&D[SERVO_1]);
	pin_digitalOut(&D[SERVO_2]);

	//motors
	pin_digitalOut(&D[MOTOR_1]);
	pin_digitalOut(&D[MOTOR_2]);
	pin_digitalOut(&D[MOTOR_3]);
	pin_digitalOut(&D[MOTOR_4]);

	//interrupts
	pin_digitalIn(&D[PENNY]);
	pin_digitalIn(&D[START]);

	pin_digitalIn(&D[SERVO_1_IN]);
	pin_digitalIn(&D[SERVO_2_IN]);

	pin_digitalIn(&D[MOTOR_1_SLOW]);
	pin_digitalIn(&D[MOTOR_1_FAST]);

	pin_digitalIn(&A[MOTOR_2_SLOW]);
	pin_digitalIn(&A[MOTOR_2_FAST]);
	pin_digitalIn(&A[MOTOR_3_SLOW]);
	pin_digitalIn(&A[MOTOR_3_FAST]);
	pin_digitalIn(&A[MOTOR_4_SLOW]);
	pin_digitalIn(&A[MOTOR_4_FAST]);

	int servo_inputs[2] = {SERVO_1_IN, SERVO_2_IN};
	int servo_outputs[2] = {SERVO_1, SERVO_2};

	int motor_inputs[6] = {MOTOR_2_FAST,MOTOR_2_SLOW, MOTOR_3_FAST,MOTOR_3_SLOW,MOTOR_4_FAST,MOTOR_4_SLOW};
	//does not include motor 1 because motor 1 reads from a digital pin
	int motor_outputs[3] = {MOTOR_2, MOTOR_3, MOTOR_4};

	//game start interrupt
	INTCON2bits.INT2EP = 1;//interrupt on negative edge
	//INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC1bits.INT2IE = 1;//enable interrupt
	RPINR1bits.INT2R = 0xA;//set interrupt to RPin 10 (digital pin 2)

	IFS1bits.INT2IF = 0;//clear flag

	//penny insertion interrupt
	INTCON2bits.INT4EP = 1;//interrupt on negative edge
	//INTCON1bits.NSTDIS = 1;//no nested interrupts
	IEC3bits.INT4IE = 1;//enable interrupt
	RPINR2bits.INT4R = 0x1D;//set interrupt to RPin 23 (digital pin 4)

	IFS3bits.INT4IF = 0;//clear flag

	//servo values
	int flipped[2] = {0,0};//neither servo is flipped

	//servo pins
	oc_servo(&oc1,&D[SERVO_1], NULL,INTERVAL,MINWIDTH,MAXWIDTH,0);//gameplay servo
	oc_servo(&oc2,&D[SERVO_1], NULL,INTERVAL,MINWIDTH,MAXWIDTH,0);
	oc_servo(&oc3,&D[BALL_SERVO],&timer2,INTERVAL,MINWIDTH,MAXWIDTH,0);//ball dispensing servo
	// oc_servo(&oc4,&D[SERVO],&timer3,INTERVAL,MINWIDTH,MAXWIDTH,0);

	//motor driving OC
	oc_pwm(&oc4, &D[MOTOR_1], NULL, 1000, 0);
	oc_pwm(&oc5, &D[MOTOR_2], NULL, 1000, 0);
	oc_pwm(&oc6, &D[MOTOR_3], NULL, 1000, 0);
	oc_pwm(&oc7, &D[MOTOR_4], NULL, 1000, 0);

	//motor variables
	int motor_speeds[4] = {0,0,0,0};
	led_on(&led1);


	int i = 0;

	while(1){
		pin_set(&D[DISPLAY]);
		if(active){
			//read servo inputs and react
			for(i = 0; i < NUM_SERVOS; i++){
				if(!pin_read(&D[(servo_inputs[i])])){
					flipped[i] = flip(flipped[i],servo_inputs[i]);
				}
			}

			//read motor 2-4 inputs and react
			for(i = 0; i < NUM_MOTORS_ANALOG; i++){
				if(!pin_read(&A[motor_inputs[i]])){
					if(i%2 == 0){
						//division maps each input pin to correct output, since there are 2 inputs/motor
						motor_speeds[i] = slow(1,motor_inputs[((int)(i/2))]);
					}
					else{
						motor_speeds[i] = slow(0, motor_inputs[((int)(i/2))]);
					}
				}
			}

			//read motor 1 and react
			if(!pin_read(&D[MOTOR_1_FAST])){
				motor_speeds[3] = slow(1,MOTOR_1);
			}
			if(!pin_read(&D[MOTOR_1_SLOW])){
				motor_speeds[3] = slow(0, MOTOR_1);
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
	active = 1;
}



	
