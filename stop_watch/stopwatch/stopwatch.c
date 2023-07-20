/*
   Author      : salah ramoud
   Description : Stop Watch System Project
   Date        : 20/09/2022
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*global variables to count time */
unsigned char seconds=0,minutes=0,hours=0;

ISR(TIMER1_COMPA_vect){
	seconds++;
	if(seconds==60){
		seconds=0;
		minutes++;
	}
	if(minutes==60){
		minutes=0;
		hours++;
	}
	if(hours==99){
		hours=0;
		seconds++;
	}
}

ISR(INT0_vect){
	/*reset stop watch*/
	seconds=0;
	minutes=0;
	hours=0;
}

ISR(INT1_vect){

	/* stop clock*/
	TCCR1B &=~(1<<CS12) &~ (1<<CS10);
}

ISR(INT2_vect){
	/*resume clock */
	TCCR1B|=(1<<CS12)|(1<<CS10);
}

void timer1_handler(){

	TCCR1A|=(1<<FOC1A);     //|(1<<FOC1B);
	//TCCR1A=0x

	TCCR1B|=(1<<WGM12)|(1<<CS10)|(1<<CS12);


	//Ftimer=(1*10^6)/1024=976.5625 hz
	//Timer=1/976.5625=1.024 ms
	//to reach second we need 1/1.024ms =976 ticks
	OCR1A=976;

	//make intial value equal zero
	TCNT1=0;

	// enable interrupt
	TIMSK=(1<<OCIE1A);
}

void int0_handler(){
	DDRD&=~(1<<2);   //make pin PD2 input pin
	PORTD|=(1<<2);   //use internal pull up resistor
	GICR|=(1<<INT0);       //enable interrubt0
	MCUCR|=(1<<ISC01);
}

void int1_handler(){
	DDRD&=~(1<<3);   //make pin PD3 input pin
	GICR|=(1<<INT1);      //enable interrupt
	MCUCR|=(1<<ISC11)|(1<<ISC10);//using rising edge
}

void int2_handler(){
	DDRB&=~(1<<2);   //make pin PB2 input pin
	PORTB|=(1<<2);   //use internal pull up resistor
	GICR|=(1<<INT2);     //enable interrupt
	MCUCSR&=~(1<<ISC2);//using falling edge
}


void display_seconds(unsigned char sec){
	/* enable PA0 */
	PORTA=((PORTA&(0xC0))|(1<<0));

	PORTC=(sec%10);
	_delay_us(2);

	/* enable PA1 */
	PORTA=((PORTA&(0xC0))|(1<<1));
	PORTC=(sec/10);
	_delay_us(2);
}

void display_minutes(unsigned char min){
		/* enable PA2 */
	PORTA=((PORTA&(0xC0))|(1<<2));

	/* to get first number*/
	PORTC=(min%10);
	_delay_us(2);

	/* enable PA3 */
	PORTA=((PORTA&(0xC0))|(1<<3));
	/*to get second number*/
	PORTC=(min/10);
	_delay_us(2);
}

void display_hours(unsigned char hour){
	/* enable PA4 */
	PORTA=((PORTA&(0xC0))|(1<<4));

	PORTC=(hour%10);
	_delay_us(2);

	/* enable PA5 */

	PORTA=((PORTA&(0xC0))|(1<<5));
	PORTC=(hour/10);
	_delay_us(2);
}

int main(void){
	DDRA=0x3F;       //make the enable of the 7 segment output pins (0011 1111)
	DDRC|=0X0F;     //make the inputs of the decoder outputs pins
	PORTC&=0xF0;   //1111 0000 make first 4 bits equals zero
	SREG |=(1<<7);   // Enable global interrupts
	int0_handler();
	int1_handler();
	int2_handler();
	timer1_handler();

	while(1){

		display_seconds(seconds);
		display_minutes(minutes);
		display_hours(hours);
	}
}


