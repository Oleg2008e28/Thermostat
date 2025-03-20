/* max6675.h
 * Создан: 12.03.2024 21:02:02
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Заголовочный файл драйвера термопреобразователя max6675
 */ 

#ifndef MAX6675_H_
#define MAX6675_H_
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include "SPI_Unit.h"
#define CS1_DDR DDRC					//Константы для настройки выхода
#define CS1_PORT PORTC					//выбора чипа, с которым мастер 
#define CS1_PIN PC3						//работает по шине SPI.
class Max6675 {
public:
	Max6675(SPI_UNIT & spi);			//Конструктор класса
	bool convert();						//Метод, выполняющий опрос max6675
	uint16_t getTemperature_10bit();	//Метод возвращает температуру в исходном виде
	float getTemperature();				//Метод возвращает температуру в преобразованном виде
private:
	void select();						//Метод выбора чипа на шине
	void unselect();					//Метод отмены выбора чипа на шине
	SPI_UNIT * spi_pointer;				//Указатель на spi модуль
	uint16_t result;					//Результат измерения температуры
	};
#endif /* MAX6675_H_ */
