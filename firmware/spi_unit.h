/* spi_unit.h
 * Создан: 29.02.2024 23:21:43
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Заголовочный файл реализации класса модуля SPI, который организует
 * обмен данными между устройствами.
 */

#ifndef SPI_UNIT_H_
#define SPI_UNIT_H_
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdio.h>
												//Тактовый выход модуля SPI
#define SCLK_DDR DDRA							//Управляющий регистр порта,
#define SCLK_PORT PORTA							//порт,
#define SCLK_PIN PA7							//номер вывода в порте.
												//Выход модуля SPI для передачи данных:
#define MOSI_DDR DDRA							//Управляющий регистр порта,
#define MOSI_PORT PORTA							//порт,
#define MOSI_PIN PA6							//номер вывода в порте.
												//Вход модуля SPI для приема данных:
#define MISO_DDR DDRC							//Управляющий регистр порта,
#define MISO_PORT PORTC							//порт,
#define MISO_PORT_IN PINC						//регистр для чтения состояний порта,
#define MISO_PIN PC2							//номер вывода в порте.
												//Организация таймаутов при передаче данных по шине SPI
#if (F_CPU > 1000000 && F_CPU <= 8000000)
	#define delay_spi asm volatile ("nop\n\t")
#elif (F_CPU > 8000000 && F_CPU <= 16000000)
	#define delay_spi asm volatile ("nop\n\tnop\n\t")
#elif (F_CPU > 16000000 && F_CPU <= 32000000)
	#define delay_spiasm asm volatile ("nop\n\tnop\n\tnop\n\tnop\n\t")
#elif (F_CPU > 32000000)
	#define delay_spiasm asm volatile ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\t")
#else
	#define delay_spi
#endif

class SPI_UNIT {
public:
	void init();								//Метод инициализации модуля SPI
	uint8_t receivByte();						//Метод приема байта
	uint16_t receivWord();						//Метод приема двухбайтового значения
	void transmissionByte(uint8_t byte);		//Метод отправки байта
};
#endif /* SPI_UNIT_H_ */
