#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"

#define DutyCycle 65536 >> 1

void main(void){
	init_pin();
	init_oc();
	init_ui();
	init_timer();

	int i;
	int direction = 0;//0 for left, 1 for right

	for(i = 1; i < 9; i++){
		pin_digitalOut(&D[i]);
	}

	pin_digitalIn(&D[0]);

	/*Configure board:*/
	pin_clear(&D[1]);
	pin_set(&D[2]);
	pin_clear(&D[3]);
	pin_set(&D[4]);//enable
	pin_clear(&D[6]);
	pin_clear(&D[5]);
	pin_set(&D[7]);//speed
	pin_clear(&D[8]);//not inverting input

	oc_pwm(&oc1, &D[6], NULL, 500, DutyCycle);
	int current_power = DutyCycle;

	while(1){
		if (!(sw_read(&sw1))){
	
			direction = !direction;
			if(direction){
				oc_free(&oc1);
				oc_pwm(&oc1, &D[6], NULL, 100, DutyCycle >> 2);
				pin_clear(&D[5]);
				led_on(&led2);
				led_off(&led3);
			}
			if(!direction){
				oc_free(&oc1);
				oc_pwm(&oc1, &D[6], NULL, 100, DutyCycle >> 3);
				pin_clear(&D[6]);
				led_on(&led3);
				led_off(&led2);

			}
		}

	}

	led_on(&led1);
}
