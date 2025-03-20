/* button.cpp
 * ������: 13.03.2024 23:51:44
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * ������ ����� ������ ����������� ������� ������, ���
 * � ������  ���������� ������� ��� � � ������ ��������������.
 */ 

#include "button.h"

Button::Button(volatile uint8_t & buttonPort, uint8_t buttonPin){						//����������� �������
	port = & buttonPort;																//���������� ����� ������
	pin = buttonPin;																	//���������� ������ ���� ������
	*(port-1) &= ~(1 << pin);															//��������� ����� ����� �� �����
	*(port) |= 1 << pin;																//����������� �������������� ��������� � �����
	flagActiv = false;
}
void Button::checkActiv(uint16_t & value, int8_t step, uint16_t minValue,				//�����, ������������� ������� ������
						uint16_t maxValue, bool * flagPress){
	if ( cheakStatus() && flagActiv == false){											//���� ��������� ������ ������, � ���� ��������� ��������� �� ����������
		flagActiv = true;																//��������������� ���� �������
		timeRattling.delayStart(RATTLING_TIME);											//����������� ������ ���������� �������� ���������
	}																					//����  ������ ��� ������ ����� � �� ����� ���������� 
	else if (flagActiv == true && cheakStatus() && 	timeRattling.delayEnd()){			//�������� ��������� �� ��������� �� ����������,														 
		if (flagPress){																	//���� ��������� �� ������� ���� ������� ������, �� �� �������,
			*flagPress = true;															// ��� ������ ���� ������ � �� �� ������� ��������� ��������.							
		}
		if ( (step > 0 && maxValue - step >= value) ||									//����������� ��������� ���������, ����������� �� ������ value
			 (step < 0 && minValue - step <= value) ){									//c ������ �������� ��������� ����������� ��������.
				value += step;
		}	
		timeWiting.delayStart(WAITING_TIME);											//����������� ������, ������� �������� ��������� �����
	}
	else if (flagActiv == true && !cheakStatus()){										//���� ���������� ��� ������ ���� ����� ������ � ��� ��������,
		flagActiv = false;																//���� ������� ������ ������������
		timeRattling.delayStop();														//��� ������� ���������������,
		timeWiting.delayStop();															//��������� ������� ������ ������������ � �������� ���������.
		timeFlipping.delayStop();
	}
	else if (flagActiv == true && cheakStatus() && timeWiting.delayEnd()){				//���� ������ ��� ��� ������ � �������� ������ �������������� ������
		if (flagPress){																	//���� ��������� �� ������� ���� ������� ������, �� �� �������,
			*flagPress = true;															// ��� ������ ���� ������ � �� �� ������� ��������� ��������.
		}
		if (  (step > 0 && maxValue - step >= value) ||									//���������� ����� ��������������, ������� �������� ��������  
		      (step < 0 && minValue - step <= value) ) value += step;					//��������� � ��������� �������
		timeFlipping.delayStart(FLIPPING_TIME);											//������������ ���� ��������.
	}
	else if (flagActiv == true && cheakStatus() && timeFlipping.delayEnd()){			//���� ������ ��� ��� ������ � �������� �������������� ��������� ������:
				if (flagPress){															//���� ��������� �� ������� ���� ������� ������, �� �� �������,
			*flagPress = true;															// ��� ������ ���� ������ � �� �� ������� ��������� ��������.
		}
	if ( (step > 0 && maxValue - step >= value) ||										//���������� ��������� ��������� � 
		 (step < 0 && minValue - step <= value) )										//������ ��������� ���������� ��������.
			value += step;
	timeFlipping.delayStart(FLIPPING_TIME);												//����� ����������� ������ ��������������, 
	}																					//� ��� ����� ������������, ���� ������ �� ����� ��������.
}
inline bool Button::cheakStatus(){														//�����, ������������ �������� ��������� �� ���� ����������� ������
	if(( *(port-2) & (1 << pin))==0) return true;										//���� ������ �������� (�� ���� ������ 0 �����), ���������� true.
	else return false;
}


