/* button.h
 * Создан: 13.03.2024 23:51:30
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Объект этого класса обслуживает нажатие кнопки, как
 * в режиме  единичного нажатия так и в режиме перелистывания.
 */ 

#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/io.h>
#include <stddef.h>															//Здесь определена константа NULL
#include "timer.h"

#define RATTLING_TIME 50												//Время устранения дребезга контактов,ms
#define WAITING_TIME 1000												//Время включения режима перелистывания,ms
#define FLIPPING_TIME 200												//Время перелистывания изменяемого параметра,ms

class Button{
public:
	Button(volatile uint8_t & buttonPort, uint8_t buttonPin);				//Конструктор объекта кнопка
	void checkActiv(uint16_t & value, int8_t step, uint16_t minValue,		//Метод проверки, была ли активна кнопка
					uint16_t maxValue,  bool * flagPress = NULL);
	inline bool cheakStatus();												//Проверка нажата кнопка или нет
private:
	volatile uint8_t * port;												//Порт для подключения кнопки.
	uint8_t pin;															//Номер пина в порту
	bool flagActiv;															//Флаг нажатия на кнопку
	Time	timeRattling,													//Таймер устранения дребезга контактов
			timeWiting,														//Таймер включения режима перелистывания
			timeFlipping;													//Таймер скорости перелистывания значений
};
#endif /* BUTTON_H_ */