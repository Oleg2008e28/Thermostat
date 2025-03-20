/* max6675.cpp
 * Создан: 12.03.2024 21:02:22
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Исполнительный файл драйвера термопреобразователя max6675
 */ 
#include "max6675.h"
Max6675::Max6675(SPI_UNIT & spi){
	CS1_DDR |= (1<<CS1_PIN);								//Настройка выхода
	CS1_PORT |= (1<<CS1_PIN);								//выбора чипа на шине spi
	spi_pointer = & spi;									//Сохранение указателя на модуль spi, 
}															//через который осуществляется связь
void Max6675::select(){
	CS1_PORT &= ~(1<<CS1_PIN);								//Перевод вывода выбора чипа в активное состояние
}
void Max6675::unselect(){
	CS1_PORT |= 1<<CS1_PIN;									//Перевод вывода выбора чипа в неактивное состояние
}
bool Max6675::convert(){
	select();												//Выбор чипа на шине
	result = spi_pointer->receivWord();						//Запрос от него данных по температуре
	unselect();												//отмена выбора чипа на шине
	if (((result >> 3) & 0xFFF) == 0xFFF )	return false;	//Проверка валидности
	else    return true;									//результата преобразования
}
uint16_t Max6675::getTemperature_10bit(){
	return ((result >> 5) & 0x3FF);
}
float Max6675::getTemperature(){
	return float((result >> 5) & 0xFFF) + ((float((result >> 3) & 0b11)*25)/100);
}
