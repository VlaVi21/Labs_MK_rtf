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

ISR(TIMER2_COMP_vect) { //Таймер 2 для динамічної індикації

	if (display_mode > 0) { // Якщо режим відображення активний
		PORTC = 0x00; // Вимкнення сегментів
		PORTA = 0x00; // Вимкнення позицій
			
		if (display_mode == 1) {
			val = adc_value; // Відображаємо значення АЦП
			} else if (display_mode == 2) {
			val = ((adc_value * 5.0) / 1023.0) * 1000; // ми міряємо напругу 
		}
		display((int)val);
		dec2digit(array[digcnt]); // Перетворення цифри в код для сегментного дисплея
		PORTA |= (1 << (digcnt + 4)); //Активація поточної позиції
		digcnt = (digcnt + 1) % DIGITS;
		
		} else{
			PORTC = 0x00;
			PORTA = 0x00;
		}
		
}

ISR(ADC_vect) { // Переривання від АЦП - при завершенні вимірювання
	adc_value = ADCL;  // Зберігаємо значення з АЦП
	adc_value |= ADCH << 8; //
}

ISR(TIMER1_COMPB_vect) { 
	  if (adc_enabled) { //перевіряжмо, що якщо ми натиснули  на кнопку вірну, (в можму випадку це кнопка 1)
		  PORTD = GREEN; //ми вмикаємо зелений світлодіод
		  
		  ADCSRA |= (1 << ADEN); // Ввімкнути АЦП активуємо ацп
		  } else { //якщо в нас ця кнопка ще раз натиснута, ми вмикаємо червоний світлодіод
		  PORTD = RED;
		 	  	
		  ADCSRA &= ~(1 << ADEN); // Вимкнути АЦП зупуняємо ацп
	  }
}

int main(void) {
	DDRA = 0xF0; // Виводи для індикаторів
	DDRC = 0xFF; // Сегменти
	DDRD = 0xB0;

	//Даташит ст.210
	ADMUX |= (1 << REFS0) | (1 << MUX1); //налаштовуємо ацп
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
				//-активацію -вмикаємо актоматичний запуск через трігер -даємо дозволіл на переривання ми налаштуємо трактову частоту для АЦП
	SFIOR |= (1 << ADTS2) | (1 << ADTS0);
	//ми  в цьому пункті обираємо джерело трігера для співпадіння В, Timer Compare match B
	
	// Таймер2 для індикації
	TCCR2 |= (1 << WGM21) | (1 << CS22) | (1 << CS20); // CTC режим, переддільник 128
	OCR2 = 38; // Поріг порівняння для таймера 2
	TIMSK |= (1 << OCIE2); // Дозвіл переривання по порівнянню таймера 2

	 TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	 OCR1A = 18;
	 OCR1B = 18;
	 TIMSK |= (1 << OCIE1B);

	sei(); // Дозвіл глобальних переривань

	while (1) {
		if ((PIND & BUT1) == BUT1) {
			_delay_ms(11);
			adc_enabled ^= 1; // Перемикаємо стан АЦП
			while ((PIND & BUT1) == BUT1) _delay_ms(11);
		}
		
		if ((PIND & BUT2) == BUT2) {
			_delay_ms(11);
			display_mode++;
			display_mode %= 3; //Максимальний 3 режим
			while ((PIND & BUT2) == BUT2) _delay_ms(11);
		}
	}
}
