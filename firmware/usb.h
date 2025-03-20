/* usb.h
 * ������: 03.04.2024 18:41:01
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * ������ ������ ������� �������� ���� ���������� � �������, 
 * ��������������� ��� �������� � ������������ usb ����������.
 */ 

#ifndef USB_H_
#define USB_H_
#include <avr/io.h>
#include <avr/eeprom.h>
#include <float.h>
#include <stdint.h>
#include "usbdrv.h"
#include "pid.h" 
#include "timer.h"
#include "oled_ssd1306_128X64.h"

#define MAX_BRIGHTNESS 100												//������������ ������� �������, %
#define MIN_BRIGHTNESS 10												//����������� ������� �������, %
#define MIN_BAK_LIGHT_TIME 10											//����������� ����� �� ���������� ��������� � �������, ���
#define MAX_KP 1000 													//������������ �������� ����������������� ������������
#define MIN_KP 0														//����������� �������� ����������������� ������������
#define MAX_KI 1000 													//������������ �������� ����������� ������������
#define MIN_KI 0														//����������� �������� ������������� ������������
#define MAX_KD 1000 													//������������ �������� ��������������� ������������
#define MIN_KD 0														//����������� �������� ��������������� ������������
#define MIN_ID_TIME 100													//����������� ����� ��������������/�����������������, ����.
#define MIN_MIN_PWM_TIME 10												//����������� �������� ���, ����������� ��������, ����.
#define MIN_STEPS_PWM 10												//���������� ��������� ���������� �������� ��� �� ������
#define MAX_SET_POINT 1000												//������������ �������� �������
#define MIN_SET_POINT 0													//����������� �������� �������

typedef struct dataexchange_t {											//��������� ��� �������� ������ (���� ��������)
	bool backLightEnable;												//���������� ���������� ���������
    uint8_t brightness;													//������� ���������
    uint16_t backLightTime;												//����� �� ���������� ���������
    float Kp;															//���������������� ����������� ���-����������
    float Ki;															//������������ ����������� ���-����������
    float Kd;															//���������������� ����������� ���-����������
    float pidError;														//������ ��� ����������
	float Proposh;														//���������������� ������������ ��� ����������
	float Integr;														//������������ ������������ ��� ����������
    float Differ;														//���������������� ������������ ��� ����������
    float out;															//�������� ������ ���
	float currentTemperature;											//������� �����������
	uint16_t minSetPoint;												//����������� �������
	uint16_t maxSetPoint;												//������������ �������
	uint16_t setPointTemperature;										//������� �����������
	uint16_t idTime;													//����� ��������������/�����������������
    uint16_t minPwmTime;												//����������� ����� �������� ���
    uint16_t stepsPWM;													//���������� �������� ���
	bool operator == (const dataexchange_t& a){							//������������� �������� ��������� ��������
		return (														//��������� ������������ �� �����, ������� ��������  
					(backLightEnable == a.backLightEnable) &&			//����������� ��������. ���� ���� ��������� ��� ����  
					(brightness == a.brightness) &&						//����� ���������� ��������, �� ��������� �����. 
					(backLightTime == a.backLightTime) &&				//�������������� ���� � ��������� �� ���������.
					(Kp == a.Kp) && (Ki == a.Ki) && (Kd == a.Kd) &&  
					(idTime == a.idTime) && 
					(minPwmTime == a.minPwmTime) && 
					(stepsPWM == a.stepsPWM) &&
					(setPointTemperature == a.setPointTemperature)
				);
	}
};
																		// ��� ���������� ������ ������ ������� ��������.
static uchar    currentAddress;											//����� ������ �������������� �����.
static uchar    bytesRemaining;											//������� ������������� �� ������ ������.
extern dataexchange_t pdata;											//������� ����������, �������� ��������� ���������� ��� �������� ������
extern PidRegula pid;
extern bool flagUsbLableOn_1;
void cheackValidDataexchange(dataexchange_t & data);					//������� �������� ���������� ������ � ���������
uchar   usbFunctionRead(uchar *data, uchar len, PidRegula const & pid);	//���������� ����� ���� ����������� ������ ������ �� ����������
uchar   usbFunctionWrite(uchar *data, uchar len);						//���������� ����� ���� ���������� ������ ������ � ����������
usbMsgLen_t usbFunctionSetup(uchar data[8]);							//������� ������������ ��� ��������� ���������� ���������� USB
#endif /* USB_H_ */
