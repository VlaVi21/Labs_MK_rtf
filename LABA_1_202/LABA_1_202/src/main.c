#include <asf.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define OFF 0x0

#define RED 0x80
#define GREEN 0x20
#define BLUE 0x10

#define BUT1 0x04
#define BUT2 0x08
#define BUT3 0x0C

bool state1 = false;
bool state2 = false;
bool state3 = false;

volatile int flag_int_0 = 0;
volatile int flag_0);
	PORTD = OFF;
	_delay_ms(10int_1 = 0;
void Zavd1(){
	PORTD = GREEN;
	_delay_ms(10000);
}

void Zavd2(){
	PORTD = GREEN;
	_delay_ms(1000);
	PORTD = OFF;
	_delay_ms(1000);
	PORTD = RED;
	_delay_ms(1000);
	PORTD = OFF;
	_delay_ms(1000);
	PORTD = BLUE;
	_delay_ms(1000);
	PORTD = OFF;
	_delay_ms(1000);
}

void Zavd3(){
	char but = 0;
	DDRD|=RED|GREEN|BLUE;
	PORTD = 0;
	
	if((PIND&BUT1)==BUT1)
	{
		_delay_ms(10); // �����������
		but ++;
		if(but % 2) ///��� �������� �� ������ ���������  (�������� �� ���)
		{
			PORTD |= BLUE; //������� ����
		}
		else
		{
			PORTD &= (~BLUE);
			
		}
		while((PIND&BUT1)==BUT1);
	}
}

void Zavd4(){
	bool state = false; //������, ��� ������������� ���� ������
	while (1){
		if ((PIND&BUT1)==BUT1){
			_delay_ms(15);
			if ((PIND&BUT1)==BUT1){
				state = !state; //����������� ����� ������
				if (state){
					PORTD |= BLUE;
				}
				else{
					PORTD &= ~BLUE;
				}
			}
			while ((PIND&BUT1)==BUT1);
		}
	}
}

Zavd5(){
	
	if ((PIND&BUT1)==BUT1){
		_delay_ms(15);
		if ((PIND&BUT1)==BUT1){
			state1 = !state1; //����������� ����� ������
			if (state1){
				PORTD = BLUE;
			}
			else{
				PORTD &= ~BLUE;
			}
		}
		while ((PIND&BUT1)==BUT1){
			if((PIND&BUT2)==BUT2){
				_delay_ms(15);
				if ((PIND&BUT2)==BUT2){
					
				state3 = !state3; //����������� ����� ������

				if (state3){
					//PORTD &= ~(BLUE | GREEN);
					PORTD = RED;
				}
				else{
					PORTD &= ~RED;
				}
				while ((PIND&BUT2)==BUT2);
				}
			}
			
	
		}
	}
	
	if ((PIND&BUT2)==BUT2){
		_delay_ms(15);
		if ((PIND&BUT2)==BUT2){
			state2 = !state2; //����������� ����� ������
			if (state2){
				PORTD = GREEN;
			}
			else{
				PORTD &= ~GREEN;
			}
		}
		while ((PIND&BUT2)==BUT2){
			if((PIND&BUT1)==BUT1){ //�� ��������� �� ���� ���������� ������ 1 ���������, ��������� �� � ������ 2, ���� ���, �� ������� ������� �������
				_delay_ms(15);
				if ((PIND&BUT1)==BUT1){
					
					state3 = !state3; //����������� ����� ������

					if (state3){
						//PORTD &= ~(BLUE | GREEN);
						PORTD = RED;
					}
					else{
						PORTD &= ~RED;
					}
					while ((PIND&BUT1)==BUT1);
				}
			}
			
			
		}
	}
	
	
}

ISR(INT0_vect){ //������ �����������
	 if (((PIND&BUT1)==BUT1)&&(PIND&BUT2)==(!BUT2)) { // ���������� �� �������� ������ 1, � �� �������� ������ 2
		 PORTD ^= BLUE;
	 }

	 if (((PIND&BUT1)==BUT1)&&(PIND&BUT2)==BUT2) { //���� ����� �������
		 PORTD ^= RED;
	 }
}

ISR(INT1_vect){
	if (((PIND&BUT2)==BUT2)&&(PIND&BUT1)==(!BUT1)) {  // ���������� �� �������� ������ 2, � �� �������� ������ 1
		PORTD ^= GREEN;
	}

	if (((PIND&BUT1)==BUT1)&&(PIND&BUT2)==BUT2) { //���� ����� �������
		PORTD ^= RED;
	}
}

Zavd6(){
		sei(); //��������� ��������� ����������
		//��� � �������� ��� ����������� ��������� � �� �������� ������ ����
}

Zavd7(){
	if ((PIND&BUT1)==BUT1){
		_delay_ms(15);
		if ((PIND&BUT1)==BUT1){
			state1 = !state1; //����������� ����� ������
			if (state1){
				PORTD |= BLUE; //�� ������ �� �������
			}
			else{
				PORTD &= ~BLUE;
			}
		}
		while ((PIND&BUT1)==BUT1){
			if((PIND&BUT2)==BUT2){
				_delay_ms(15);
				if ((PIND&BUT2)==BUT2){
					
					state3 = !state3; //����������� ����� ������

					if (state3){
						//PORTD &= ~(BLUE | GREEN);
						PORTD |= RED;
					}
					else{
						PORTD &= ~RED;
					}
					while ((PIND&BUT2)==BUT2);
				}
			}
			
			
		}
	}
	
	if ((PIND&BUT2)==BUT2){
		_delay_ms(15);
		if ((PIND&BUT2)==BUT2){
			state2 = !state2; //����������� ����� ������
			if (state2){
				PORTD |= GREEN;
			}
			else{
				PORTD &= ~GREEN;
			}
		}
		while ((PIND&BUT2)==BUT2){
			if((PIND&BUT1)==BUT1){ //�� ��������� �� ���� ���������� ������ 1 ���������, ��������� �� � ������ 2, ���� ���, �� ������� ������� �������
				_delay_ms(15);
				if ((PIND&BUT1)==BUT1){
					
					state3 = !state3; //����������� ����� ������

					if (state3){
						//PORTD &= ~(BLUE | GREEN);
						PORTD |= RED;
					}
					else{
						PORTD &= ~RED;
					}
					while ((PIND&BUT1)==BUT1);
				}
			}
			
			
		}
	}
	
}

int main (void)
{
	DDRD|=RED|GREEN|BLUE;

	PORTD = OFF; //������ �� ���������
	MCUCR |= (1 << ISC01) | (1 << ISC00); // ����������� ����������
	MCUCR |= (1 << ISC11) | (1 << ISC10);

	GICR  |= (1 << INT0) | (1 << INT1); // �������� ���������� �� INT0 �� INT1
	
	while (1){
		//Zavd1();
		//Zavd2();
		//Zavd3();
		//Zavd4();
		//Zavd5();
		Zavd6();
		//Zavd7();
	}
}

