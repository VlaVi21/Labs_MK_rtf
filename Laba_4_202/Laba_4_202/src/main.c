#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DIGITS 4
#define BUT1 0x04  
#define BUT2 0x08  

#define GREEN 0x20 // PD5
#define RED   0x80 // PD7

int array[DIGITS] = {0};
int digcnt = 0;

int adc_enabled = 0;
int display_mode = 0;
int adc_value = 0;

double val = 0;

void dec2digit(char dec) {
	switch(dec) {
		case 0: PORTC = 0b01011111; break;
		case 1: PORTC = 0b00000110; break;
		case 2: PORTC = 0b00111011; break;
		case 3: PORTC = 0b00101111; break;
		case 4: PORTC = 0b01100110; break;
		case 5: PORTC = 0b01101101; break;
		case 6: PORTC = 0b01111101; break;
		case 7: PORTC = 0b00000111; break;
		case 8: PORTC = 0b01111111; break;
		case 9: PORTC = 0b01101111; break;
		default: PORTC = 0b00000000;
	}
}

void display(int num) {
	for (int i = DIGITS - 1; i >= 0; i--) {
		array[i] = num % 10;
		num /= 10;
	}
}

ISR(TIMER2_COMP_vect) { //������ 2 ��� �������� ���������

	if (display_mode > 0) { // ���� ����� ����������� ��������
		PORTC = 0x00; // ��������� ��������
		PORTA = 0x00; // ��������� �������
			
		if (display_mode == 1) {
			val = adc_value; // ³��������� �������� ���
			} else if (display_mode == 2) {
			val = ((adc_value * 5.0) / 1023.0) * 1000; // �� ������ ������� 
		}
		display((int)val);
		dec2digit(array[digcnt]); // ������������ ����� � ��� ��� ����������� �������
		PORTA |= (1 << (digcnt + 4)); //��������� ������� �������
		digcnt = (digcnt + 1) % DIGITS;
		
		} else{
			PORTC = 0x00;
			PORTA = 0x00;
		}
		
}

ISR(ADC_vect) { // ����������� �� ��� - ��� ��������� ����������
	adc_value = ADCL;  // �������� �������� � ���
	adc_value |= ADCH << 8; //
}

ISR(TIMER1_COMPB_vect) { 
	  if (adc_enabled) { //����������, �� ���� �� ���������  �� ������ ����, (� ����� ������� �� ������ 1)
		  PORTD = GREEN; //�� ������� ������� ��������
		  
		  ADCSRA |= (1 << ADEN); // �������� ��� �������� ���
		  } else { //���� � ��� �� ������ �� ��� ���������, �� ������� �������� ��������
		  PORTD = RED;
		 	  	
		  ADCSRA &= ~(1 << ADEN); // �������� ��� ��������� ���
	  }
}

int main(void) {
	DDRA = 0xF0; // ������ ��� ����������
	DDRC = 0xFF; // ��������
	DDRD = 0xB0;

	//������� ��.210
	ADMUX |= (1 << REFS0) | (1 << MUX1); //����������� ���
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
				//-��������� -������� ������������ ������ ����� ����� -���� ������� �� ����������� �� ��������� �������� ������� ��� ���
	SFIOR |= (1 << ADTS2) | (1 << ADTS0);
	//��  � ����� ����� ������� ������� ������ ��� ��������� �, Timer Compare match B
	
	// ������2 ��� ���������
	TCCR2 |= (1 << WGM21) | (1 << CS22) | (1 << CS20); // CTC �����, ����������� 128
	OCR2 = 38; // ���� ��������� ��� ������� 2
	TIMSK |= (1 << OCIE2); // ����� ����������� �� ��������� ������� 2

	 TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	 OCR1A = 18;
	 OCR1B = 18;
	 TIMSK |= (1 << OCIE1B);

	sei(); // ����� ���������� ����������

	while (1) {
		if ((PIND & BUT1) == BUT1) {
			_delay_ms(11);
			adc_enabled ^= 1; // ���������� ���� ���
			while ((PIND & BUT1) == BUT1) _delay_ms(11);
		}
		
		if ((PIND & BUT2) == BUT2) {
			_delay_ms(11);
			display_mode++;
			display_mode %= 3; //������������ 3 �����
			while ((PIND & BUT2) == BUT2) _delay_ms(11);
		}
	}
}
