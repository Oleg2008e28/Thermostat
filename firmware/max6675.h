/* max6675.h
 * ������: 12.03.2024 21:02:02
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * ������������ ���� �������� �������������������� max6675
 */ 

#ifndef MAX6675_H_
#define MAX6675_H_
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include "SPI_Unit.h"
#define CS1_DDR DDRC					//��������� ��� ��������� ������
#define CS1_PORT PORTC					//������ ����, � ������� ������ 
#define CS1_PIN PC3						//�������� �� ���� SPI.
class Max6675 {
public:
	Max6675(SPI_UNIT & spi);			//����������� ������
	bool convert();						//�����, ����������� ����� max6675
	uint16_t getTemperature_10bit();	//����� ���������� ����������� � �������� ����
	float getTemperature();				//����� ���������� ����������� � ��������������� ����
private:
	void select();						//����� ������ ���� �� ����
	void unselect();					//����� ������ ������ ���� �� ����
	SPI_UNIT * spi_pointer;				//��������� �� spi ������
	uint16_t result;					//��������� ��������� �����������
	};
#endif /* MAX6675_H_ */
