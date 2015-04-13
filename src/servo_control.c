#include "oc.h"
#include "ui.h"
#include "common.h"
#include "timer.h"
#include "pin.h"

#define INTERVAL .02
#define MAXWIDTH .0024
#define MINWIDTH .0006
#define MAXPOS 2^15



int flip(int flipped){
	if(flipped){
		pin_write(&D[7],MAXPOS);
		led_off(&led1);
		if(pin_read(&D[7])==MAXPOS){
			led_off(&led3);
		}
		return 0;
	}
	else{
		led_on(&led1);
		pin_write(&D[7],0);
		if(pin_read(&D[7])==0){
			led_on(&led3);
		}
		return 1;
	}
}

void main(void){
	init_oc();
	init_ui();
	init_timer();
	init_pin();

	int flipped = 1;

	pin_digitalOut(&D[7]);
	//pin_clear(&D[6]);
	oc_servo(&oc9, &D[7],&timer1,INTERVAL,MINWIDTH,MAXWIDTH,0);
	//oc_pwm(&oc1, &D[6], NULL, 500, 65536 >> 1);

	while(1){
	if(!sw_read(&sw1)){
	// 		flipped = flip(flipped);
		if(flipped){
		pin_write(&D[7],MAXPOS);
		led_off(&led1);
		if(pin_read(&D[7])==MAXPOS){
			led_off(&led3);
		}
		flipped =0;
	}
	else{
		led_on(&led1);
		pin_write(&D[7],0);
		if(pin_read(&D[7])==0){
			led_on(&led3);
		}
		flipped = 1;
	}
		}
	}
}
