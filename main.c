//Lab section:B21
//Name: Tszwing Choi
//Email: tchoi002@ucr.edu
//Assignment: custom lab: Juke Box
//I acknowledge all content contained herein, excluding template or example code, is my own original work.

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#include "Timer.h"
#include "keypad.h"
#include "pwm.h"

//frequency that used
#define c4	261.63
#define cs4 277.18
#define d4	293.66
#define e4	329.63
#define f4	349.23
#define g4	392.00
#define gs4	415
#define a4	440.00
#define as4	455
#define b	466 //for sw
#define b4	493.88 
#define c5	523.25
#define cs5	554
#define d5	587.33
#define ds5	622
#define e5	659.26
#define f5	698.46
#define fs5	740
#define g5	783.99
#define gs5	830
#define a5	880.00
#define b5	987.77
#define c6	1046.50
#define d6	1174.66
#define e6	1318.51
#define f6	1396.91
//size of each song
#define ttls_size 43//7*6
#define sw_size 44 //28
#define ij_size 40//80
#define mi_size 46
//#1 song ttls
const double ttls_melody [ttls_size] = {0,c4,c4,g4,g4,a4,a4,g4,/*1*/
	f4,f4,e4,e4,d4,d4,c4,/*2*/
g4,g4,f4,f4,e4,e4,d4,/*3*/
g4,g4,f4,f4,e4,e4,d4,/*4*/
c4,c4,g4,g4,a4,a4,g4,/*5*/
f4,f4,e4,e4,d4,d4,c4/*6*/};
const short ttls_playtime [ttls_size] = {0,400,400,400,400,400,400,800,/*1*/
	400,400,400,400,400,400,800,/*2*/
	400,400,400,400,400,400,800,/*3*/
	400,400,400,400,400,400,800,/*4*/
	400,400,400,400,400,400,800,/*5*/
	400,400,400,400,400,400,800/*6*/};
const short ttls_resttime[ttls_size] = {0,5,5,5,5,5,5,7,
	5,5,5,5,5,5,7,
	5,5,5,5,5,5,7,
	5,5,5,5,5,5,7,
	5,5,5,5,5,5,7,
	5,5,5,5,5,5,0};
//#2 song starwar
const double sw_melody [sw_size] = {0,a4,a4,a4,f4,c5,a4,f4,c5,a4,/*1.1*/
	e5,e5,e5,f5,c5,gs4,f4,c5,a4,/*first section*/
	a5,a4,a4,a5,gs5,g5,fs5,f5,fs5,/*2.1*/
	as4,ds5,d5,cs5,c5,b,c5,/*second section*/
	f4,gs4,f4,a4,c5,a4,c5,e5/*first variant*/};
const short sw_playtime [sw_size] = {0,500,500,500,350,150,500,350,150,650,
	500,500,500,350,150,500,350,150,650,/*1*/
	500,300,150,500,325,175,125,125,250,/*2.1*/
	250,500,325,175,125,125,250,/*2.2*/
	250,500,350,125,500,375,125,650/*3*/};
const short sw_resttime[sw_size] = {0,5,5,5,5,5,5,5,5,500,
	5,5,5,5,5,5,5,5,500,/*1*/
	5,5,5,5,5,5,5,5,325,/*2.1*/
	5,5,5,5,5,5,350,/*2.2*/
	5,5,5,5,5,5,5,0};
//#3 song indiana jones
const double ij_melody [ij_size] = {0,e5,f5,g5,c6,
	d5,e5,f5,
	g5,a5,b5,f6,
	a5,b5,c6,d6,e6,
	e5,f5,g5,c6,
	d6,e6,f6,
	g5,g5,e6,d6,g5,
	e6,d6,g5,
	e6,d6,g5,
	f6,e6,d6,c5};
const short ij_playtime [ij_size] = {0,200,200,400,800,
	200,200,800,
	200,200,400,800,
	200,200,400,400,400,
	200,200,400,800,
	200,200,800,
	200,200,400,200,200,
	400,200,200,
	400,200,200,
	400,200,200,800};
const short ij_resttime[ij_size] = {0,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,
	5,5,5,0};
//#4 mission impossible
const double mi_melody [mi_size] = {0,d4,d4,f4,g4,
	d4,d4,c4,cs4,
	d4,d4,f4,g4,
	d4,d4,c4,cs4,
	f5,d5,a4,
	f5,d5,gs4};
const double mi_playtime[mi_size] = {0,450,450,300,300,
	450,450,300,300,
	450,450,300,300,
	450,450,300,300,
	150,150,2100,
	150,150,2100};
const short mi_resttime[mi_size] =	{0,5,5,5,5,
	5,5,5,5,
	5,5,5,5,
	5,5,5,30,
	5,5,5,
	5,5,0};

unsigned char i = 0; //counter for the array
unsigned char songplayflag = 0; //to know which song to play 
unsigned char currentstate = 0x00; //for the left/right menu function
unsigned char stopflag = 0; //detect anyone wan to stop playing the song
unsigned char pauseflag = 0; //detect anyone want to pause the song 

//SM for menu display
enum d_states {welcome,instr, title1, title2, title3, title4, direction, left, right, waitsongend} d_state;
void display(){
	unsigned char button = ~PINB & 0x07;  
	switch(d_state){
		case welcome:
		LCD_ClearScreen();
		LCD_DisplayString(3, "Music Player");
		delay_ms(7000);
		d_state = instr;
		break;
		case instr:
		LCD_ClearScreen();
		LCD_DisplayString(1, "Enter the Numberto Play the Song");
		delay_ms(10000);
		d_state = title1;
		break;
		case title1:
		currentstate = 0x01;
		LCD_ClearScreen();
		LCD_DisplayString(1, "1.Twinkle Twinkle Little Star>");
		d_state = direction;
		break;
		case direction:
		if (songplayflag != 0){
			d_state = waitsongend;
		}
		else if (button == 0x02){//right
			d_state = right;
		}
		else if (button == 0x04){ //left
			d_state = left;
		}
		else{
			d_state = direction;
		}
		break;
		case left:
		if(button == 0x00){
			if (currentstate == 0x01){
				d_state = title1;
			}
			else if ( currentstate == 0x02){
				d_state = title1;
			}
			else if (currentstate == 0x03){
				d_state = title2;
			}
			else if (currentstate == 0x04){
				d_state = title3;
			}			
		}
		else{
			d_state = left;
		}
		break;
		case right:
		if(button == 0x00){
			if (currentstate == 0x01){
				d_state = title2;
			}
			else if ( currentstate == 0x02){
				d_state = title3;
			}
			else if (currentstate == 0x03){
				d_state = title4;
			}
			else if (currentstate == 0x04){
				d_state = title4;
			}			
		}
		else{
			d_state = right;
		}
		break;
		case title2:
		currentstate = 0x02;
		LCD_ClearScreen();
		LCD_DisplayString(1, "<2.Star War>");
		d_state = direction;
		break;
		case title3:
		currentstate = 0x03;
		LCD_ClearScreen();
		LCD_DisplayString(1, "<3.Indiana Jones>");
		d_state = direction;
		break;
		case title4:
		currentstate = 0x04;
		LCD_ClearScreen();
		LCD_DisplayString(1, "<4.Mission Impossible");
		d_state = direction;
		break;		
		case waitsongend:
		if (songplayflag != 0){
			d_state = waitsongend;
		}
		else if (songplayflag == 0){
			d_state = title1;
		}
		break;
		default:
		d_state = welcome;
		break;
	}
}

//SM to know which song is chosen and stop button
enum states { wait, waitplaysong, playsong, stop}state;
void button(){
	unsigned char stopbutton = ~PINB & 0x01; //any button to stop the song
	unsigned char keypadInput = GetKeypadKey(); //for the keypad input
	switch(state){
		case wait:
		PORTD = 0x00;
		stopflag = 0;		
		i = 0;
		if (keypadInput == '1'){ //#1 song
			songplayflag = 1; //which song is requested
			PWM_on();
			LCD_DisplayString (1, "Twinkle Twinkle Little Star");
			state = waitplaysong;
		}
		else if (keypadInput == '2'){ //#2 song
			songplayflag = 2; //song requested
			PWM_on();
			LCD_DisplayString (1, "Star War");
			state = waitplaysong;
		}
		else if (keypadInput == '3'){ //#3 song
			songplayflag = 3; //song requested
			PWM_on();
			LCD_DisplayString (1, "Indiana Jones");
			state = waitplaysong;
		}
		else if (keypadInput == '4'){ //#3 song
			songplayflag = 4; //song requested
			PWM_on();
			LCD_DisplayString (1, "Mission Impossible             ");
			state = waitplaysong;
		}		
		else if (keypadInput == '\0'){ // no song requested
			state = wait;
		}
		break;
		case waitplaysong:
		if (stopbutton == 0x00){
			state = playsong;
		}
		else{
			state = waitplaysong;
		}
		break;
		case playsong:
		if(songplayflag!=0){ //song is playing
			if (stopbutton == 0x00){
				state = playsong;//remain playing until song is end
			}
			else{
				state = stop;
			}
		}
		else if (songplayflag==0){//song is end
			state = wait;
		}
		break;
		case stop:
		if(stopbutton == 0x00){ //no one try to stop the song
			stopflag = 1;
			PWM_off();
			songplayflag = 0;
			pauseflag = 0;
			state = wait;
		}
		else{
			state = stop;
		}
		break;
		default:
		state =  wait;
		break;
	}
}

//SM to play the song 
unsigned short c = 0; //counter for duration and rest time
enum m_states{off,sw_note,sw_nextnote,sw_rest,sw_duration,
	ttls_note,ttls_nextnote,ttls_rest,ttls_duration,
ij_note,ij_nextnote,ij_rest,ij_duration}m_state;
void music(){
	switch(m_state){
		case off:
		if (pauseflag == 1){
			set_PWM(0);
			m_state = off;
		}
		else if(pauseflag ==0){
			if (songplayflag==0){
				m_state = off;
			}
			else if (songplayflag == 1){
				m_state = ttls_note;
			}
			else if(songplayflag == 2){
				m_state = sw_note;
			}
			else if(songplayflag == 3){
				m_state = ij_note;
			}
		}
		break;
		//#2 sw
		case sw_note:
		set_PWM(sw_melody[i]); //choose the note
		if (sw_melody[i] == f4 || sw_melody [i] == b){
			PORTD = 0x01;
		}
		else if (sw_melody[i] == gs4){
			PORTD = 0x03;
		}
		else if (sw_melody[i] == a4){
			PORTD = 0x07;
		}
		else if (sw_melody[i] == c5){
			PORTD = 0x0F;
		}		
		else if (sw_melody [i] == e5){
			PORTD = 0x1F;
		}
		else if (sw_melody [i] == f5){
			PORTD = 0x3F;
		}
		else if (sw_melody [i] == cs5 || sw_melody [i] == as4){
			PORTD = 0x02;
		}
		else if (sw_melody [i] == fs5 || sw_melody [i] == ds5){
			PORTD = 0x04;
		}
		else if (sw_melody [i] == g5 || sw_melody [i] == d5){
			PORTD = 0x08;
		}						
		else if (sw_melody [i] == gs5){
			PORTD = 0x10;
		}		
		else if (sw_melody [i] == a5){
			PORTD = 0x20;
		}		
		m_state = sw_duration;
		break;
		case sw_duration:
		if (stopflag ==1){
			m_state = off;
		}
		else if (pauseflag == 1){
			set_PWM(0);
			m_state = off;
		}
		else if (c < sw_playtime[i]){
			c++;
			m_state = sw_duration; //play for this long
		}
		else{
			c = 0;
			m_state = sw_rest;
		}
		break;
		case sw_rest:
		PORTD = 0x00;
		set_PWM(0);
		if (stopflag ==1){
			m_state = off;
		}
		else if (pauseflag == 1){
			m_state = off;
		}		
		else if (c < sw_resttime[i]){
			c++;
			m_state = sw_rest; //break for this long
		}
		else{
			c = 0;
			m_state = sw_nextnote;
		}
		break;
		case sw_nextnote:
		if (stopflag ==1){
			m_state = off;
		}
		else if (i >= sw_size-1){
			songplayflag = 0;
			PWM_off();
			i = 0;
			m_state = off;
		}
		else{
			i++;
			m_state = sw_note;
		}
		break;
		//#1 ttls
		case ttls_note:
		set_PWM(ttls_melody[i]); //choose the note
		if (ttls_melody[i] == a4){
			PORTD = 0x20;
		}
		else if (ttls_melody[i] == g4){
			PORTD = 0x10;
		}
		else if (ttls_melody[i] == f4){
			PORTD = 0x08;
		}
		else if (ttls_melody[i] == e4){
			PORTD = 0x04;
		}
		else if (ttls_melody[i] == d4){
			PORTD = 0x02;
		}
		else if (ttls_melody[i] == c4){
			PORTD = 0x01;
		}
		m_state = ttls_duration;
		break;
		case ttls_duration:
		if (stopflag ==1){
			m_state = off;
		}
		else if (pauseflag == 1){
			set_PWM(0);
			m_state = off;
		}		
		else if (c < ttls_playtime[i]){
			c++;
			m_state = ttls_duration; //play for this long
		}
		else{
			c = 0;
			m_state = ttls_rest;
		}
		break;
		case ttls_rest:
		set_PWM(0);
		PORTD = 0x00;
		if (stopflag ==1){
			m_state = off;
		}
		else if (pauseflag == 1){
			m_state = off;
		}		
		else if (c < ttls_resttime[i]){
			c++;
			m_state = ttls_rest; //break for this long
		}
		else{
			c = 0;
			m_state = ttls_nextnote;
		}
		break;
		case ttls_nextnote:
		if (stopflag ==1){
			m_state = off;
		}
		else if (i >= ttls_size-1){
			songplayflag = 0;
			PWM_off();
			i = 0;
			m_state = off;
		}
		else{
			i++;
			m_state = ttls_note;
		}
		break;
		//#3 ij
		case ij_note:
		set_PWM(ij_melody[i]); //choose the note
		if (ij_melody [i]==c5){
			PORTD = 0x03;
		}
		else if (ij_melody[i]== d5){
			PORTD = 0x01;
		}
		else if (ij_melody[i] == e5){
			PORTD = 0x02;
		}
		else if (ij_melody[i]==f5){
			PORTD = 0x07;
		}
		else if (ij_melody[i]==g5){
			PORTD = 0x0F;
		}
		else if (ij_melody[i]==a5){
			PORTD = 0x04;
		}
		else if (ij_melody[i]==b5){
			PORTD = 0x08;
		}
		else if (ij_melody[i]==c6){
			PORTD = 0x30;
		}
		else if (ij_melody[i]==d6){
			PORTD =0x18;
		}
		else if (ij_melody[i]==e6){
			PORTD =0x1C;
		}
		else if (ij_melody[i]==f6){
			PORTD =0x38;
		}
		m_state = ij_duration;
		break;
		case ij_duration:
		if (stopflag ==1){
			m_state = off;
		}
		else if (pauseflag == 1){
			set_PWM(0);
			m_state = off;
		}		
		else if (c < ij_playtime[i]){
			c++;
			m_state = ij_duration; //play for this long
		}
		else{
			c = 0;
			m_state = ij_rest;
		}
		break;
		case ij_rest:
		set_PWM(0);
		PORTD = 0x00;
		if (stopflag ==1){
			m_state = off;
		}
		else if (pauseflag == 1){
			m_state = off;
		}		
		else if (c < ij_resttime[i]){
			c++;
			m_state = ij_rest; //break for this long
		}
		else{
			c = 0;
			m_state = ij_nextnote;
		}
		break;
		case ij_nextnote:
		if (stopflag ==1){
			m_state = off;
		}
		else if (i >= ij_size-1){
			songplayflag = 0;
			PWM_off();
			i = 0;
			m_state = off;
		}
		else{
			i++;
			m_state = ij_note;
		}
		break;
		default:
		state = off;
		break;
	}
}

//SM to play mission impossible
enum mi_states{mi_off,mi_note,mi_nextnote,mi_rest,mi_duration}mi_state;
void missionimpossible (){
	switch(mi_state){
		case mi_off:
		if (pauseflag == 1){
			m_state = mi_off;
		}
		else if (pauseflag == 0){		
			if (songplayflag == 4){
				mi_state = mi_note;
			}
			else if(songplayflag == 0){
				mi_state = mi_off;
			}
		}
		break;
		case mi_note:
		set_PWM(mi_melody[i]);
		if (mi_melody[i] == c4){
			PORTD = 0x01;
		}
		else if (mi_melody[i] == cs4){
			PORTD = 0x02;
		}		
		else if (mi_melody[i] == d4){
			PORTD = 0x03;
		}
		else if (mi_melody[i] == f4){
			PORTD = 0x04;
		}
		else if (mi_melody[i] == g4){
			PORTD = 0x06;
		}
		else if (mi_melody[i] == gs4){
			PORTD = 0x0C;
		}		
		else if (mi_melody[i] == a4){
			PORTD = 0x08;
		}
		else if (mi_melody[i] == d5){
			PORTD = 0x10;
		}	
		else if (mi_melody[i] == f5){
			PORTD = 0x20;
		}			
		mi_state = mi_duration;
		break;
		case mi_duration:
		if(stopflag == 1){
			mi_state = mi_off;
		}
		else if (pauseflag == 1){
			set_PWM(0);
			mi_state = mi_off;
		}
		else if (c < mi_playtime[i]){
			c++;
			mi_state = mi_duration;
		}
		else{
			c = 0;
			mi_state = mi_rest;
		}
		break;
		case mi_rest:
		set_PWM(0);
		PORTD = 0x00;
		if(stopflag == 1){
			mi_state = mi_off;
		}
		else if (pauseflag == 1){
			mi_state = mi_off;
		}
		else if (c < mi_resttime[i]){
			c++;
			mi_state = mi_rest; //break for this long
		}
		else{
			c = 0;
			mi_state = mi_nextnote;
		}
		break;	
		case mi_nextnote:
		if (stopflag ==1){
			mi_state = mi_off;
		}
		else if (i >= mi_size-1){
			songplayflag = 0;
			PWM_off();
			i = 0;
			mi_state = mi_off;
		}
		else{
			i++;
			mi_state = mi_note;
		}
		break;
		default:
		state = mi_off;
		break;			
	}
}

//SM for pause button. if pause button is pressed raise pauseflag. if unpressed, lowered the pauseflag.
enum p_states {waitpause, pause, waitplay,unpause} p_state;
void pausebutton(){
	unsigned char  pausebutton = ~PINB & 0x08; //mask pause button
	switch(p_state){
		case waitpause:
		if (pausebutton == 0x00){
			p_state = waitpause;
		}
		else if (pausebutton == 0x08){
			pauseflag = 1;
			p_state = pause;
		}
		break;
		case pause:
		if(pausebutton == 0x08){
			p_state = pause;
		}
		else if (pausebutton == 0x00){
			p_state = waitplay;
		}
		break;
		case waitplay:
		if(pausebutton == 0x00){
			p_state = waitplay;
		}
		else if (pausebutton == 0x08){
			//playflag = 1;
			pauseflag = 0;
			p_state = unpause;
		}
		break;
		case unpause:
		if(pausebutton == 0x08){
			p_state = unpause;
		}
		else if (pausebutton == 0x00){
			p_state = waitpause;
		}
		break;
		default:
		p_state = waitpause;
		break;
	}
}

int main(void)
{
	DDRA = 0xF0; PORTA = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRB = 0xF0; PORTB = 0x0F; //speaker must be connected to PB6
	DDRC = 0xFF; PORTC = 0x00; //LCD
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	unsigned long timerPeriod = 1;
	d_state = welcome;
	state = wait;
	m_state = off;
	mi_state = mi_off;
	p_state = waitpause;
	LCD_init();
	TimerSet(timerPeriod);
	TimerOn();
	while (1)
	{
		display();
		button();
		music();
		pausebutton();
		missionimpossible();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}