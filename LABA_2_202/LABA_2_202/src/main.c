#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//динамічна індикація це коли на наш індикатор мікроконтроллер з великою швидкістю подає або живлення або сигнали, і над індикатор виводить ці сигнали в послідовному вигляді
//плюси - менше пінв задіяно, більш менше енергоспоживання, і виводять дані послідовно, ШИМ
// мінуси - якщо низька частота - то блимає, 

#define BUT1 0x04  // PD2 - INT0
#define BUT2 0x08  // PD3 - INT1
#define DIGITS 4
#define LONG_PRESS 200  // 200 * 10мс = 2 сек

volatile int num = 0; //динамічна тому що, у нас є певна частота з якою вона працює
int array[DIGITS] = {0};
volatile uint8_t digcnt = 0;

volatile uint8_t btn1_held = 0; // Прапор — кнопка 1 ще не відпущена
volatile uint8_t btn2_held = 0; // Прапор — кнопка 2 ще не відпущена

void dec2digit(char dec) {
	switch(dec) { //це в нас набір значень щоб виводити цифри
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
		case 0xA: PORTC = 0b01110111; break; //додав букви до 16 системи числення
		case 0xB: PORTC = 0b01111101; break; 
		case 0xC: PORTC = 0b01011001; break; 
		case 0xD: PORTC = 0b00111110; break; 
		case 0xE: PORTC = 0b01111001; break; 
		case 0xF: PORTC = 0b01110001; break; 
		default: PORTC = 0b00000000; //одну чорточку проєбав
	}
}

void display(int num) {
	for (int i = DIGITS - 1; i >= 0; i--) {
		array[i] = num % 16; //замість 10-16 система
		num /= 16; // замість 10-16-сист. числення
	}
}

ISR(INT0_vect) {  // Кнопка "вгору"
	if (!btn1_held) {
		num++;
		if (num > 0xFFFF) num = 0xFFFF; //0xFFFF - в десятковій 65535, це фактично максимум у системі числення (МОЖУ ЗРОБИТИ ЩОБ ТІЛЬКИ ДО 16 РАХУВАЛО)
		display(num);
		btn1_held = 1; // Встановити прапор — утримується
	}
}

ISR(INT1_vect) {  // Кнопка "вниз"
	if (!btn2_held) {
		if (num > 0) num--;
		display(num);
		btn2_held = 1; // Встановити прапор — утримується
	}
}

ISR(TIMER2_COMP_vect) {
	// Динамічна індикація
	PORTC = 0x00;
	PORTA = 0x00;
	dec2digit(array[digcnt]); // Завантаження цифри для поточного розряду
	PORTA |= (1 << (digcnt + DIGITS)); // Увімкнення відповідного індикатора
	digcnt = (digcnt + 1) % DIGITS; // Перехід до наступного індикатора та Повернення до 0 після 3 (циклічно)

	// Перевірка довгого натискання (скидання)
	static uint16_t counter1 = 0;
	static uint16_t counter2 = 0;

	if (PIND & BUT1) { // Кнопка 1: якщо тримається, лічильник росте
		counter1++;
		if (counter1 >= LONG_PRESS) {
			num = 0; // Обнулення
			display(num);  // Оновити дисплей
			counter1 = 0;
		}
		} else {
		counter1 = 0; // Якщо відпустили — скидання
		btn1_held = 0; // Дозволити наступне натискання
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

	// Налаштування портів
	DDRD &= ~(BUT1 | BUT2); // PD2, PD3 як вхід
	DDRC = 0xFF;            // PORTC — сегменти (вихід)
	DDRA = 0xF0;            // PORTA [4..7] — позиції (вихід)

	PORTC = 0x00;
	PORTA = 0xF0;

	/*Налаштування зовнішніх переривань*/
	/*Даташин - External Interrupts(1 1 - зрост. фронт)*/
	MCUCR |= (1 << ISC00) | (1 << ISC01); // переривання на зростаючому фронті сигналу (тобто, коли кнопка натиснута й сигнал переходить із LOW до HIGH).
	MCUCR |= (1 << ISC10) | (1 << ISC11);
	GICR |= (1 << INT0) | (1 << INT1); //вмикаємо самі переривання1

	/*==============================================*/
	TCCR2 = (1 << WGM21) | (1 << CS22) | (1 << CS21); //налаштування таймера 2 (даташит)
	//режим CTC (обнул. при збігу з OCR2)
	//вибір такотової частоти (СS22 та CS21)
	
	OCR2 = 12; //Значення, з яким порівнюється таймер (інтервал між перериваннями)
	// f=40*4=160 Гц => 1/160 = 0.00625 с => 0,00625 * (1*10^6/256)-1= 23,4 = 23 - 
	//4 - розряди, 40 - задана частота, далі часовий інтервал, та саме значення регістру порівняння
	
	TIMSK |= (1 << OCIE2); //Вмикає переривання по збігу таймера 2 з OCR2
	
	//F_CPU = 8 МГц, частота дільника 256
	/*==============================================*/
	

	sei();  // Глобальне увімкнення переривань

	while (1) {
		
	}
}

