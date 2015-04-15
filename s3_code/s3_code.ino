#include <Servo.h>

//#include "common.h"
// #include "uart.h"
// #include "oc.h"
// #include "timer.h"
// #include "ui.h"
// #include <p24FJ128GB206.h>
// #include "pin.h"
#include "keymap.h"

#define MAX_OUTPUT 2^15

#define MOTOR_BASE_SPEED MAX_OUTPUT>>8

#define SERVO 6
#define KEYBOARD_CLOCK 1//green
#define KEYBOARD_DATA 7//blue
#define MOTOR 9
#define PUMP 10

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
#define SWITCH_PUMP 'g'

//motor speeds
#define SLOWER 0
#define FASTER 1

//keyboard reading variables
volatile uint8_t data;
volatile boolean new_command = false;
volatile uint8_t started, bits;
volatile int bit_count = 0;

Servo s1;

int flip(int flipped){
	if(flipped){
		//pin_write(&D[SERVO],MAX_OUTPUT);
                s1.write(60);
                digitalWrite(13,HIGH);
		return 0;
	}
	else{		
		//pin_write(&D[SERVO],0);
                s1.write(0);
                digitalWrite(13,LOW);
		return 1;
	}
}


//uint16_t calc_speed(uint16_t speed, int m_dir){
//	if(m_dir && speed < MAX_OUTPUT){//go faster
//		return speed << 1;
//	}
//	else if(!m_dir && speed > 0) {//go slower
//		return speed >> 1;
//	}
//	else{
//		return speed;
//	}
//}

int calc_speed(int old_speed, int dir){
     int speed = old_speed;
     if(dir && speed < 255){
       speed = old_speed/2;
     }
     else if(!dir && speed >5){
       speed = old_speed*2;
     }
     return speed;
}

int flipped = 0;
char input = 'a';
uint16_t speed = MOTOR_BASE_SPEED;
int pumpState = 0;
int repeats_counter = 0;
volatile boolean keydown = false;
int state_set = 0;

int toggle_pump(int pumpState){

  if(pumpState){
         digitalWrite(PUMP, LOW);
         digitalWrite(13,HIGH);
         //Serial.println ("pump");
         return 0;
}
                                else{
                                  digitalWrite(PUMP,HIGH);
                                  digitalWrite(13,LOW);
                                  return 1;
                                }  
}
     

void setup(){
  Serial.begin(9600);
  Serial.println("Hello");
  pinMode(13,OUTPUT);
//	init_oc();
//	init_uart();
//	init_timer();
//	init_ui();
//	init_pin();

	//switch controller, shouldn't do anything
	//char input = 'a';

	//keyboard interruptINTCON2bits.INT4EP = 1;//interrupt on negative edge
	attachInterrupt(0,read_keyboard,FALLING);

	//servo values
	//int flipped = 0;

	//servo pins
//	pinMode(SERVO,output);
//	pinWrite(SERVO,0);
        s1.attach(5);

	//motor driving OC
	//oc_pwm(&oc1, &D[MOTOR], NULL, 500, MOTOR_BASE_SPEED);
        pinMode(MOTOR, OUTPUT);
        analogWrite(MOTOR, 172);
        
	//motor constants
	//uint16_t speed = MOTOR_BASE_SPEED;
        
        pinMode(PUMP,OUTPUT);
        Serial.println("beginning loop");
        //int pumpState;
}
   
void loop(){
		//read the keyboard
		if(new_command){
			input = keymap[data]; //grab character from array
			Serial.println(input);
                        //Serial.println(new_command);	
			new_command = false;
		}


		switch(input){//make this not a switch- move this to game state and make these to-be-executed
			case START:
				//start the timer
				digitalWrite(8, HIGH);
                                input = 'a';
			case END:
				//stop the timer
                                
				digitalWrite(8,LOW);
                                digitalWrite(8,HIGH);
                                input = 'a';
			case FLIP_SERVO:
				flipped = flip(flipped);
input = 'a';
			case ACCEL_MOTOR:
				speed = calc_speed(speed, FASTER);
				analogWrite(MOTOR,speed);
input = 'a';
			case SLOW_MOTOR:
				speed = calc_speed(speed, SLOWER);
				analogWrite(MOTOR,speed);
input = 'a';
                        case 'g':
                                pumpState = toggle_pump(pumpState);
                                input = 'a';
                          
                             
		}
}


void read_keyboard(){//keyboard interrupt
	if(!started){
                new_command = false;
		if(digitalRead(KEYBOARD_DATA) == LOW){
			started = 1;
			data = 0;
                        new_command = 0;
			return;
		}
	}
	else if(bit_count<=8){
		data |=(digitalRead(KEYBOARD_DATA)<<bit_count);
		bit_count++;
		return;
	}
	else{
		bit_count = 0;
		started = 0;
            if(keydown){
              keydown = false;
              return;
            }
            if(data ==240){
                keydown = true;
                return;
            }
            new_command = true;
            
	}
}

	


