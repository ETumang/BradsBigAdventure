#include "ui.h"
#include "common.h"
#include "pin.h"
#include "oc.h"
#include "timer.h"
#include "uart.h"
#include <stdio.h>


#define FULL_DUTY_CYCLE 2^16
void main(void){

	int stateFlag = 0;

	init_pin();
	init_oc();
	init_ui();

/*Configure board:*/
	pin_clear(&D[1]);
	pin_set(&D[2]);
	pin_clear(&D[3]);
	pin_set(&D[4]);//enable
	pin_clear(&D[6]);
	pin_clear(&D[5]);
	pin_set(&D[7]);//speed
	pin_clear(&D[8]);//not inverting input

	pin_analogIn(&A[0]);

	oc_pwm(&oc1, &D[6], NULL, 500, 0);
	oc_pwm(&oc1, &D[5], NULL, 500, 0);

	while(1){

		if(sw_read(&sw1){
			stateFlag = (stateFlag++)%3;
		}

		switch(stateFlag){
			case(0):
				pin_write(&D[5],FULL_DUTY_CYCLE);
				pin_write(&D[6],0);
				break;
			case(1):
				pin_write(&D[5],FULL_DUTY_CYCLE);
				pin_write(&D[6],0);
				break;
			default:
				pin_write(&D[5],0);
				pin_write(&D[6],0);
				break;
		}
	}
}