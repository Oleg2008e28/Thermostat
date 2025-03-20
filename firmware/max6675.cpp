/* max6675.cpp
 * ������: 12.03.2024 21:02:22
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * �������������� ���� �������� �������������������� max6675
 */ 
#include "max6675.h"
Max6675::Max6675(SPI_UNIT & spi){
	CS1_DDR |= (1<<CS1_PIN);								//��������� ������
	CS1_PORT |= (1<<CS1_PIN);								//������ ���� �� ���� spi
	spi_pointer = & spi;									//���������� ��������� �� ������ spi, 
}															//����� ������� �������������� �����
void Max6675::select(){
	CS1_PORT &= ~(1<<CS1_PIN);								//������� ������ ������ ���� � �������� ���������
}
void Max6675::unselect(){
	CS1_PORT |= 1<<CS1_PIN;									//������� ������ ������ ���� � ���������� ���������
}
bool Max6675::convert(){
	select();												//����� ���� �� ����
	result = spi_pointer->receivWord();						//������ �� ���� ������ �� �����������
	unselect();												//������ ������ ���� �� ����
	if (((result >> 3) & 0xFFF) == 0xFFF )	return false;	//�������� ����������
	else    return true;									//���������� ��������������
}
uint16_t Max6675::getTemperature_10bit(){
	return ((result >> 5) & 0x3FF);
}
float Max6675::getTemperature(){
	return float((result >> 5) & 0xFFF) + ((float((result >> 3) & 0b11)*25)/100);
}
