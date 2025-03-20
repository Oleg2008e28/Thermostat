/* button.h
 * ������: 13.03.2024 23:51:30
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * ������ ����� ������ ����������� ������� ������, ���
 * � ������  ���������� ������� ��� � � ������ ��������������.
 */ 

#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/io.h>
#include <stddef.h>															//����� ���������� ��������� NULL
#include "timer.h"

#define RATTLING_TIME 50												//����� ���������� �������� ���������,ms
#define WAITING_TIME 1000												//����� ��������� ������ ��������������,ms
#define FLIPPING_TIME 200												//����� �������������� ����������� ���������,ms

class Button{
public:
	Button(volatile uint8_t & buttonPort, uint8_t buttonPin);				//����������� ������� ������
	void checkActiv(uint16_t & value, int8_t step, uint16_t minValue,		//����� ��������, ���� �� ������� ������
					uint16_t maxValue,  bool * flagPress = NULL);
	inline bool cheakStatus();												//�������� ������ ������ ��� ���
private:
	volatile uint8_t * port;												//���� ��� ����������� ������.
	uint8_t pin;															//����� ���� � �����
	bool flagActiv;															//���� ������� �� ������
	Time	timeRattling,													//������ ���������� �������� ���������
			timeWiting,														//������ ��������� ������ ��������������
			timeFlipping;													//������ �������� �������������� ��������
};
#endif /* BUTTON_H_ */