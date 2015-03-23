#include "oc.h"
#include "ui.h"
#include "common.h"

void flip(void){
	pin_write(&D[13],2^14);
	pin_write(&D[13],0);
}

void main(void){
	init_oc();
	init_ui();

	pin_digitalOut(&D[13]);
	pin_clear(&D[13]);
	oc_pwm(&oc2,&D[13], NULL, 500,0);

	while(1){
		if(!sw_read(&sw1)){
			led_toggle(&led1);
			flip();
		}
	}
}
