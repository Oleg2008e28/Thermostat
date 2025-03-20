/* spi_unit.h
 * ������: 29.02.2024 23:21:43
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * ������������ ���� ���������� ������ ������ SPI, ������� ����������
 * ����� ������� ����� ������������.
 */

#ifndef SPI_UNIT_H_
#define SPI_UNIT_H_
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdio.h>
												//�������� ����� ������ SPI
#define SCLK_DDR DDRA							//����������� ������� �����,
#define SCLK_PORT PORTA							//����,
#define SCLK_PIN PA7							//����� ������ � �����.
												//����� ������ SPI ��� �������� ������:
#define MOSI_DDR DDRA							//����������� ������� �����,
#define MOSI_PORT PORTA							//����,
#define MOSI_PIN PA6							//����� ������ � �����.
												//���� ������ SPI ��� ������ ������:
#define MISO_DDR DDRC							//����������� ������� �����,
#define MISO_PORT PORTC							//����,
#define MISO_PORT_IN PINC						//������� ��� ������ ��������� �����,
#define MISO_PIN PC2							//����� ������ � �����.
												//����������� ��������� ��� �������� ������ �� ���� SPI
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
	void init();								//����� ������������� ������ SPI
	uint8_t receivByte();						//����� ������ �����
	uint16_t receivWord();						//����� ������ ������������� ��������
	void transmissionByte(uint8_t byte);		//����� �������� �����
};
#endif /* SPI_UNIT_H_ */
