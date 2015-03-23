#include "common.h"
#include "uart.h"
#include "oc.h"
#include "timer.h"
#include "ui.h"

#define CLOCK_FREQ 2//how many timer cycles = 1 "clock" cycle
#define TIMER_FREQ 100//timer frequency in hz
#define READ_FREQ CLOCK_FREQ/TIMER_FREQ//how many seconds between each keyboard check


//Not great, change to read-in-loop later. 
int bit_counter = 0;
int press_flag = 0;//currently clocking data?
unsigned int scan_code = 0;//the clocked in stuff

void read_keyboard(_TIMER *timer);

void main(void){

	init_oc();
	init_uart();
	init_timer();
	init_ui();

	void *readPointer;
	readPointer = &read_keyboard;

	pin_digitalOut(&D[13]);//clock output
	pin_clear(&D[13]);
	pin_digitalIn(&D[12]);//data input

	timer_setFreq(&timer1,TIMER_FREQ);//just to make data checking easier

	oc_pwm(&oc2, &D[13], &timer1, CLOCK_FREQ, 2^15);//make clock output

	timer_every(&timer1, CLOCK_FREQ/TIMER_FREQ, readPointer);//set up keyboard reading

	timer_start(&timer1);

	while(1){	
		if(press_flag){
			led_on(&led1);
		}
	}
}

void read_keyboard(_TIMER *timer){
	if(press_flag && bit_counter < 10){//clocking in code
		scan_code << 1;
		scan_code = scan_code && pin_read(&D[12]);
		bit_counter++;
	}

	else{
		if(bit_counter == 10){//finished clocking in code
			press_flag = 0;
			bit_counter = 0;
		}
		if (!pin_read(&D[12])&& !press_flag){//not currently clocking in code, code starts
			press_flag = 1;
		}
	}
}

