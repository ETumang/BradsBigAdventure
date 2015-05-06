#include "common.h"
#include "uart.h"//Don't use the UART! It disables my interrupts.
#include "oc.h"
#include "timer.h"
#include "ui.h"
#include <p24FJ128GB206.h>
#include "pin.h"
#include "keymap.h"
#include <time.h>

#define START 4//interrupt for game start
#define END 8//interrupt for game end

#define LIGHTS 5//turn on all the lights and the Ardumini

#define MOTOR_1 0
#define MOTOR_2 1
#define MOTOR_3 3

#define MOTOR_1_FAST 6
#define MOTOR_1_SLOW 7

#define MOTOR_2_FAST 12
#define MOTOR_2_SLOW 9

#define MOTOR_3_FAST 10
#define MOTOR_3_SLOW 11

volatile int active = 0;

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


void main(){
	init_oc();
	init_timer();
	init_ui();
	init_pin();

	pin_set(&D[LIGHTS]);

	INTCON2bits.INT2EP = 1;//interrupt on negative edge
	IEC1bits.INT2IE = 1;//enable interrupt
	RPINR1bits.INT2R = 0x1D;//interrupt on pin 4
	//INTCON1bits.NSTDIS = 1;

	IFS1bits.INT2IF = 0;//clear flag

	INTCON2bits.INT4EP = 0;//interrupt on positive edge
	IEC3bits.INT4IE = 1;//enable interrupt
	RPINR2bits.INT4R = 0x4;//interrupt on pin 8

	IFS3bits.INT4IF = 0;//clear fla

	//lights start and motors
	pin_digitalOut(&D[LIGHTS]);
	pin_set(&D[LIGHTS]);
	pin_digitalOut(&D[MOTOR_1]);
	pin_digitalOut(&D[MOTOR_2]);
	pin_digitalOut(&D[MOTOR_3]);

	//game start interrupt
	pin_digitalIn(&D[START]);
	pin_digitalIn(&D[END]);

	//motor controls
	pin_digitalIn(&D[MOTOR_1_FAST]);
	pin_digitalIn(&D[MOTOR_1_SLOW]);
	pin_digitalIn(&D[MOTOR_2_FAST]);
	pin_digitalIn(&D[MOTOR_2_SLOW]);
	pin_digitalIn(&D[MOTOR_3_FAST]);
	pin_digitalIn(&D[MOTOR_3_SLOW]);

	timer_setPeriod(&timer1, 0.1);

    oc_pwm(&oc7, &D[MOTOR_1], NULL, 100, 2000);
	oc_pwm(&oc7, &D[MOTOR_2], NULL, 100, 2000);
	oc_pwm(&oc7, &D[MOTOR_3], NULL, 100, 2000);

	while(1){
		// if (timer_flag(&timer1)){
            // timer_lower(&timer1);
			if(active){
				if(!pin_read(&D[MOTOR_1_FAST])){
					slow(1,MOTOR_1);
					led_on(&led3);

				}
				if(!pin_read(&D[MOTOR_1_SLOW])){
					slow(0,MOTOR_1);
					led_off(&led3);
				}

				if(!pin_read(&D[MOTOR_2_FAST])){
					slow(1,MOTOR_2);

				}
				if(!pin_read(&D[MOTOR_2_SLOW])){
					slow(0,MOTOR_2);
				}

				if(!pin_read(&D[MOTOR_3_FAST])){
					slow(1,MOTOR_3);

				}
				if(!pin_read(&D[MOTOR_3_SLOW])){
					slow(0,MOTOR_3);
				}
			// }
		}
	}
}


void __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void){//game start interrupt
	IFS3bits.INT4IF = 0;//clear flag
	led_on(&led1);
	timer_start(&timer1);
	pin_clear(&D[LIGHTS]);
	active = 1;
}

void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void){//game end interrupt
	IFS1bits.INT2IF = 0;//clear flag
	pin_clear(&D[LIGHTS]);
	pin_write(&D[MOTOR_1], 0);
	pin_write(&D[MOTOR_2], 0);
	pin_write(&D[MOTOR_3], 0);
	led_off(&led1);
	active = 0;
}
		
