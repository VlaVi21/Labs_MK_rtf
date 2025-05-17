#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//�������� ��������� �� ���� �� ��� ��������� �������������� � ������� �������� ���� ��� �������� ��� �������, � ��� ��������� �������� �� ������� � ����������� ������
//����� - ����� ��� ������, ���� ����� ����������������, � �������� ��� ���������, ���
// ����� - ���� ������ ������� - �� �����, 

#define BUT1 0x04  // PD2 - INT0
#define BUT2 0x08  // PD3 - INT1
#define DIGITS 4
#define LONG_PRESS 200  // 200 * 10�� = 2 ���

volatile int num = 0; //�������� ���� ��, � ��� � ����� ������� � ���� ���� ������
int array[DIGITS] = {0};
volatile uint8_t digcnt = 0;

volatile uint8_t btn1_held = 0; // ������ � ������ 1 �� �� ��������
volatile uint8_t btn2_held = 0; // ������ � ������ 2 �� �� ��������

void dec2digit(char dec) {
	switch(dec) { //�� � ��� ���� ������� ��� �������� �����
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
		case 0xA: PORTC = 0b01110111; break; //����� ����� �� 16 ������� ��������
		case 0xB: PORTC = 0b01111101; break; 
		case 0xC: PORTC = 0b01011001; break; 
		case 0xD: PORTC = 0b00111110; break; 
		case 0xE: PORTC = 0b01111001; break; 
		case 0xF: PORTC = 0b01110001; break; 
		default: PORTC = 0b00000000; //���� �������� ������
	}
}

void display(int num) {
	for (int i = DIGITS - 1; i >= 0; i--) {
		array[i] = num % 16; //������ 10-16 �������
		num /= 16; // ������ 10-16-����. ��������
	}
}

ISR(INT0_vect) {  // ������ "�����"
	if (!btn1_held) {
		num++;
		if (num > 0xFFFF) num = 0xFFFF; //0xFFFF - � ��������� 65535, �� �������� �������� � ������ �������� (���� ������� ��� Ҳ���� �� 16 ��������)
		display(num);
		btn1_held = 1; // ���������� ������ � ����������
	}
}

ISR(INT1_vect) {  // ������ "����"
	if (!btn2_held) {
		if (num > 0) num--;
		display(num);
		btn2_held = 1; // ���������� ������ � ����������
	}
}

ISR(TIMER2_COMP_vect) {
	// �������� ���������
	PORTC = 0x00;
	PORTA = 0x00;
	dec2digit(array[digcnt]); // ������������ ����� ��� ��������� �������
	PORTA |= (1 << (digcnt + DIGITS)); // ��������� ���������� ����������
	digcnt = (digcnt + 1) % DIGITS; // ������� �� ���������� ���������� �� ���������� �� 0 ���� 3 (�������)

	// �������� ������� ���������� (��������)
	static uint16_t counter1 = 0;
	static uint16_t counter2 = 0;

	if (PIND & BUT1) { // ������ 1: ���� ���������, �������� �����
		counter1++;
		if (counter1 >= LONG_PRESS) {
			num = 0; // ���������
			display(num);  // ������� �������
			counter1 = 0;
		}
		} else {
		counter1 = 0; // ���� ��������� � ��������
		btn1_held = 0; // ��������� �������� ����������
	}

	if (PIND & BUT2) {
		counter2++;
		if (counter2 >= LONG_PRESS) {
			num = 0;
			display(num);
			counter2 = 0;
		}
		} else {
		counter2 = 0;
		btn2_held = 0;
	}
}

int main(void) {
	display(num);

	// ������������ �����
	DDRD &= ~(BUT1 | BUT2); // PD2, PD3 �� ����
	DDRC = 0xFF;            // PORTC � �������� (�����)
	DDRA = 0xF0;            // PORTA [4..7] � ������� (�����)

	PORTC = 0x00;
	PORTA = 0xF0;

	/*������������ ������� ����������*/
	/*������� - External Interrupts(1 1 - �����. �����)*/
	MCUCR |= (1 << ISC00) | (1 << ISC01); // ����������� �� ����������� ����� ������� (�����, ���� ������ ��������� � ������ ���������� �� LOW �� HIGH).
	MCUCR |= (1 << ISC10) | (1 << ISC11);
	GICR |= (1 << INT0) | (1 << INT1); //������� ��� �����������1

	/*==============================================*/
	TCCR2 = (1 << WGM21) | (1 << CS22) | (1 << CS21); //������������ ������� 2 (�������)
	//����� CTC (�����. ��� ���� � OCR2)
	//���� �������� ������� (�S22 �� CS21)
	
	OCR2 = 12; //��������, � ���� ����������� ������ (�������� �� �������������)
	// f=40*4=160 �� => 1/160 = 0.00625 � => 0,00625 * (1*10^6/256)-1= 23,4 = 23 - 
	//4 - �������, 40 - ������ �������, ��� ������� ��������, �� ���� �������� ������� ���������
	
	TIMSK |= (1 << OCIE2); //����� ����������� �� ���� ������� 2 � OCR2
	
	//F_CPU = 8 ���, ������� ������� 256
	/*==============================================*/
	

	sei();  // ��������� ��������� ����������

	while (1) {
		
	}
}

