#include <TimerOne.h>
#include "TABLE_EIGHT.h"


#define clck_1 2
#define clck_2 3
#define out_1 4 // 16 0b00010000 on PORTD 
#define out_2 5 // 32 0b00100000 on PORTD
#define out_3 6
#define out_and 9
#define out_or 8
#define out_xor 7
#define reset 10
 
#define in_pot_1 A0
#define in_pot_2 A1

const uint16_t tempo = 100;

volatile uint16_t pot_1 = 0;
volatile uint16_t pot_2 = 0;

volatile uint16_t rythm_1 = 0;
volatile uint16_t rythm_2 = 0;
volatile uint16_t rythm_3 = 0;

volatile bool step_done_1 = false;
volatile bool step_done_2 = false;
volatile bool step_done_3 = false;
volatile bool reset_done = false;

volatile uint8_t step_count_1 = 1;
volatile uint8_t step_count_2 = 1;
volatile uint16_t step_count_3 = 1;


volatile uint8_t one_OR_two = 0;
volatile uint8_t one_AND_two = 0;
volatile uint8_t one_XOR_two = 0;



void setup() {
	// setup serial
	//Serial.begin(9600);           
	// Setup PINs
	pinMode(clck_1, INPUT_PULLUP);
	pinMode(clck_2, INPUT_PULLUP);
	pinMode(reset, INPUT);
	pinMode(out_1, OUTPUT);
  	pinMode(out_2, OUTPUT);
	pinMode(out_3, OUTPUT);	
	
	pinMode(out_and, OUTPUT);
	pinMode(out_xor, OUTPUT);
	pinMode(out_or, OUTPUT);

	pinMode(reset, INPUT_PULLUP);
	
//	Timer stuff
	Timer1.initialize(tempo);
	Timer1.attachInterrupt(check_all);
	Timer1.start();
	//Serial.println("There it goes");
	delay(1000);	
}

void loop() {

}

void check_all() {
	read_pots();
	do_reset();
	do_rythm_1();
	do_rythm_2();
	do_logic();
	// add reset here
}

void do_reset() {
	if (digitalRead(reset) & ! reset_done) {
		step_count_1 = 1;
		step_count_2 = 1;
		step_count_3 = 1;
		reset_done = true;
	}
	if (! digitalRead(reset) & reset_done) {
		reset_done = false;
	}
}

uint16_t do_mixing(uint8_t in_1, uint8_t in_2, uint8_t half = 8) {
	// with 0b11001 and 0b01101 should output 0b0111100011
	uint16_t mixed = 0;
	uint16_t j = 0;
    	uint16_t maskos = 0b1;
      
	for (uint8_t i = 0; i < half; i++) {
		mixed |= ((in_1 & (maskos << i)) > 0) << j;
		j += 1;
		mixed |= ((in_2 & (maskos << i)) > 0) << j;
		j += 1;
	}
	return mixed;
}

void read_pots() {
	pot_1 = 255 - (analogRead(in_pot_1) >> 2);
	pot_2 = 255 - (analogRead(in_pot_2) >> 2);
}

void do_rythm_1() {		
	rythm_1 = uint8_t pgm_read_word(&(table_eight[pot_1]));
	
	if (digitalRead(clck_1) == HIGH && step_done_1 == false) {
		/* rythm 1 */
		digitalWrite(out_1, (rythm_1 & step_count_1) > 0);
		//Serial.print("step : ");Serial.print(step_count_1);Serial.print("\t");Serial.println(step_count_1, BIN);
		//Serial.print("rythm_1: ");Serial.print(rythm_1, BIN); Serial.print(" ||| "); Serial.println(rythm_1);
		step_count_1 *=2;
		if (step_count_1 == 0) step_count_1 = 1;
		
		/* rythm 3 */
		rythm_3 = do_mixing(rythm_1, rythm_2);
		digitalWrite(out_3, (rythm_3 & step_count_3) > 0);
		//Serial.print("step : ");Serial.print(step_count_3);Serial.print("\t");
		//Serial.println(step_count_3, BIN);
		//Serial.print("rythm_3: "); Serial.print(rythm_3, BIN); Serial.print("|||"); Serial.println(rythm_3);		
		step_count_3 *=2;
		if (step_count_3 == 0) step_count_3 = 1;		
		step_done_1 = true;
		
	} else if (digitalRead(clck_1) == LOW && step_done_1 == true) {
		step_done_1 = false;
		digitalWrite(out_1, 0);
		digitalWrite(out_3, 0);
	}
}

void do_rythm_2() {

	
	rythm_2 = uint8_t pgm_read_word_near(&(table_eight[pot_2]));			
	if (digitalRead(clck_2) == HIGH && step_done_2 == false) {
		digitalWrite(out_2, rythm_2 & step_count_2);
		//Serial.print("rythm_2: ");Serial.print(rythm_2, BIN); Serial.print(" ||| "); Serial.println(rythm_2);
		//Serial.print("step : ");Serial.print(step_count_2);Serial.print("\t");Serial.println(step_count_2, BIN);
		step_count_2 *=2;
		if (step_count_2 == 0) step_count_2 = 1;
		step_done_2 = true;	
	} else if (digitalRead(clck_2) == LOW && step_done_2 == true) {
		step_done_2 = false;
		digitalWrite(out_2, 0);
	}
}

void do_logic() {
	one_OR_two =  (PORTD & 16) || (PORTD & 32);
	one_AND_two = (PORTD & 16) && (PORTD & 32);
	//Serial.print(one_OR_two); 	Serial.print("\t");Serial.println(one_AND_two);	
     //Serial.println(one_OR_two && (!one_AND_two));
	digitalWrite(out_or,  one_OR_two);
	digitalWrite(out_and,  one_AND_two);
	digitalWrite(out_xor, one_OR_two && !(one_AND_two));	
}

