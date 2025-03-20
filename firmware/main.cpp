/* main.cpp
 * ������: 25.02.2024 02:37:40
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 */ 
#define F_CPU 12000000UL										//��������� ������� ����������.
#include <avr/io.h>
#include <math.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "timer.h"
#include "spi_unit.h"
#include "oled_ssd1306_128X64.h"
#include "max6675.h"
#include "button.h"
#include "checkchangevalue.h"
#include "pid.h"
#include "usb.h"
#include "usbdrv.h"
#include "oddebug.h"
#include "math.h"

#define START_DELAY 2000										//�������� ��� ��������� ������� 2 �������
#define INIT_MODULS_TIME 200									//�������� ��� ������������� ������� 0,2 �������
#define USB_RESET_TIME 300										//����� ������ USB ����
#define DISPLAY_UPDATA_TIME 1000								//������� ���������� ���������� �� �������
#define VUSB_MONITOR_ACTIV(); DDRD&=~(1<<VUSB_MONITOR_PIN);		//������ ��������� ������ ����� ��� �������� ���������� � USB �������
#define VUSB_MONITOR_PORT PIND									//���� ��� ������ ��������� ���������� USB �����
#define VUSB_MONITOR_PIN PD3									//����� ���� ��� �������� ����������
	
bool EEMEM	eeBackLightEnable = true;							//��������� �������� � ����������������� ������
uint8_t EEMEM eeBrightness = 70;
uint16_t EEMEM eeBackLightTime = 300;
float EEMEM eeKp = 0;
float EEMEM eeKi = 0;
float EEMEM eeKd = 0;
uint16_t EEMEM eeMinSetPoint = 0;
uint16_t EEMEM eeMaxSetPoint = 100;
uint16_t EEMEM eeSetPointTemperature = 50;
uint16_t EEMEM eeIDTime = 1000;
uint16_t EEMEM eeMinPwmTime = 20;
uint16_t EEMEM eeStepsPWM = 100;

uint32_t timeCounter = 0;										//���������� ���������� ��� ������ ������ Time
dataexchange_t pdata;											//��������� ������ ��� �������� ������� �������� ����������

PidRegula pid(PORTB, PB3);										//�������� ������� ��� ����������
bool flagButtonPress = false;									//���� ������� ������ ��� ���������� ����������
bool flagUsbLableOn_1 = false;									//���� ��� ���������� ������� USB
bool flagUsbLableOn_2 = false;									//������ ���� ��� ���������� ������� USB

int main(void){
	
	_delay_ms(START_DELAY );																	//�������� �� ��������� ��� ������ �������	
	
	eeprom_read_block(&pdata.backLightEnable, &eeBackLightEnable, sizeof(bool));				//������ �� EEPROM ��������� ���������� ���������� ��������� 
	pdata.brightness = eeprom_read_byte(&eeBrightness);											//������ �� EEPROM ������� ���������
	pdata.backLightTime = eeprom_read_word(&eeBackLightTime);									//������ �� EEPROM ������� ���������� ���������
	eeprom_read_block(&pdata.Kp, &eeKp, sizeof(float));											//������ �� EEPROM ����������������� ������������
	eeprom_read_block(&pdata.Ki, &eeKi, sizeof(float));											//������ �� EEPROM ������������� ������������
	eeprom_read_block(&pdata.Kd, &eeKd, sizeof(float));											//������ �� EEPROM ����������������� ������������
	pdata.minSetPoint = eeprom_read_word(&eeMinSetPoint);										//������ �� EEPROM ���������� ���������� �������� �������
	pdata.maxSetPoint = eeprom_read_word(&eeMaxSetPoint);										//������ �� EEPROM ���������� ���������� �������� �������
	pdata.setPointTemperature = eeprom_read_word(&eeSetPointTemperature);						//������ �� EEPROM �������
	pdata.idTime = eeprom_read_word(&eeIDTime);													//������ �� EEPROM ������� ��������������/�����������������
	pdata.minPwmTime = eeprom_read_word(&eeMinPwmTime);											//������ �� EEPROM ������������ ������� ���
	pdata.stepsPWM = eeprom_read_word(&eeStepsPWM);												//������ �� EEPROM ����� ���
	cheackValidDataexchange(pdata);																//�������� ���������� ������ ����������� �� ����������������� ������
	SPI_UNIT spi;																				//�������� ������������ ������� ���� SPI 
	spi.init();																					//������������� ������������ ������� ���� SPI
	Max6675 termo(spi);																			//�������� ������� ��� ������������
	Displey_ssd1306_128x64 display;																//�������� ������� �������
	display.init(spi);																			//������������� �������
	display.cleare();																			//������� �������
	_delay_ms(INIT_MODULS_TIME);																//�������� ��� ���������� ������������� ������� ���������
	display.printLogo();																		//������ �������� 
	Button button_up(PORTA, PA2);																//�������� ������� ������ "������"
	Button button_down(PORTB, PB4);																//�������� ������� ������ "������"
	Time timerDisplayUpdate,																	//������ ��� ���������� �������  
		 eepSaveTime,																			//������ ��� ���������� ������� � ����������������� ������
		 timerBackLightOff;																		//������ ���������� ���������

	CheckChangeValue<uint16_t>  updateSP(pdata.setPointTemperature),							//������ ��� �������� ��������� �������
								updateEepSP(pdata.setPointTemperature),							//��� ���������� � ����������������� ������,
								updatePidSP(pdata.setPointTemperature),							//��� �������� ��� ����������.
								updateBackLightTime(pdata.backLightTime);						//������ ��� �������� �� ���������� ������� ���������� �������
	CheckChangeValue<dataexchange_t> updatePData(pdata);										//������ ��� �������� ��������� �������� ����������
	{
		float vp;
		display.printSP(pdata.setPointTemperature);												//����� �������� ������� �� �����
		if (termo.convert()){																	//������ �� �������������������
			vp = termo.getTemperature();														//��������� ������� �����������
			display.printVP(uint16_t(round(vp)));												//���������� �������� � ����� �� �����
		}
		else{
				display.printVPerr();															//���� �� ������� ��������� �������������������, 
		}																						//�� ������� ������������ ������
		pid.setPidSP(pdata.setPointTemperature);
		pid.setStartVP(uint16_t(vp));
	}		
	display.setBrightness(pdata.brightness);													//��������������� �������� ������� �������
	pid.setKp(pdata.Kp);																		//��������� ����������������� ������������.
	pid.setKi(pdata.Ki);																		//��������� ������������� ������������.
	pid.setKd(pdata.Kd);																		//��������� ����������������� ������������.
	pid.setMinPwmTime(pdata.minPwmTime);														//��������� ������������ ������� ���.
	pid.setStepsPwm(pdata.stepsPWM);															//��������� ���������� ����� ���.
	pid.setTime(pdata.idTime);																	//��������� ������� ��������������/�����������������.
	usbInit();																					//������������� ���������� V-USB
 	cli();																						//������ ���� ����������
 	usbDeviceDisconnect();																		//�������������� ���������� �� �����
 	_delay_ms(USB_RESET_TIME);																	//����� ���������� �� �����, �� ����� 250 ms
 	usbDeviceConnect();																			// ����������� � �����
	sei();																						//���������� ���������� ����������
	VUSB_MONITOR_ACTIV();																		//��������� �������� ���������� �� USB �����

    while (1) {
		//***************************************************  ���� ������� ��� ������ � ����� usb ********************************************************//
		if ((VUSB_MONITOR_PORT&(1<<VUSB_MONITOR_PIN))!=0){										//���� ���������� ������� �� �����, 
			usbDeviceConnect();																	//�� ���� ������������ � ���� USB
		}
		else{
			usbDeviceDisconnect();																//���� ������� �������, ����� �����������
		}
			usbPoll();																			// ������� ��������� ��������� �� �����
		if(flagUsbLableOn_1){																	//����� ��������� ��� ���������� ������������ 
			if((VUSB_MONITOR_PORT&(1<<VUSB_MONITOR_PIN))==0){									//�������� USB ���� �� ������� ���
 				display.deleteUSBlable();														//����������� ���������� � �����.
				flagUsbLableOn_1 = false;														//���� ���������� �� ����������� ���� ���������,
				flagUsbLableOn_2 = false;														//���������� ����������� �� ���� � ������� ���������.
			}
			else{
				if (!flagUsbLableOn_2){															//���� ���������� �� ����������� ���� ����������
					display.printUSBlable();													//� ���������� ����� �������� ��� ������������� ����������
					flagUsbLableOn_2 = true;													//������� ���������� �� ������.
				}
				
			}							 
 		}
		//***************************************************  ����� ����� ������� ��� ������ � ����� usb ********************************************************//	
		
		//********************************************* ���� ���������� ���������� � ��������� ������� ������ *********************************************//
		int8_t k;																				//����������, ������������ ��� ��������� ���������
		if (updateBackLightTime.checkChange()){													//���� ���� ���������� ���������� ��������� ����������,
			timerBackLightOff.delayStop();														//������ ���������� ��������� ��������������� � ������������.
			if(pdata.backLightEnable){															//���� ���������� ��������� ���������,
				updateBackLightTime.updatValue();												//������ ���������� ��������� ����������� ������.
			}
			
		}
		if (pdata.backLightEnable && display.getBacklightStatus()){								//���� ���������� ���������� ��������� � ��������� ��������,
			timerBackLightOff.delayStart((uint32_t)(pdata.backLightTime) * 1000);				//����������� ������ �� ���������� ���������
		}
		if (pdata.backLightEnable && !display.getBacklightStatus()) {							//���� ���������� ��������� ��������� � ��������� ���������, ��� ��������� ��������� 
			k=0; }																				//��� ������ ����� ���� (��� ������� �������� �� ���������, �� ������� ������� ��������).
		else {																					//�� ���� ������ ������� ��� ����� 1, �������� ���� ������������� �� 1 ��� �����������
			k=1; }																				//������� ������� ������ ��� �� ������������.
		button_up.checkActiv(pdata.setPointTemperature, k, pdata.minSetPoint, 
							 pdata.maxSetPoint, &flagButtonPress);								//��������� ������� ������ "������".
		button_down.checkActiv(pdata.setPointTemperature, -k, pdata.minSetPoint, 
							   pdata.maxSetPoint, &flagButtonPress);							//��������� ������� ������ "������".
		if (!display.getBacklightStatus() && flagButtonPress){									//���� ��������� ������� ��������� � ���� ������� ������, ��
			display.onDispley();																//��������� ������� ����������.
			flagButtonPress = false;															//���� �������� ������� ������ ������������
			timerBackLightOff.delayStop();														//����������� � ������������ ������ ���������� ���������.
			timerBackLightOff.delayStart((uint32_t)(pdata.backLightTime) * 1000);				//������ ���������� ��������� ��������������� ������.
		}
		if(timerBackLightOff.delayEnd()){														//���� ������ ���������� ��������� ��������,
			display.offDispley();																//��������� �����������.
		}
		//****************************************** ����� ���� ���������� ���������� � ��������� ������� ������ ******************************************//
		
		//******************************************** ���� ���������� ��������, ���� ���� �������� ��� ������� *******************************************//
		if (updatePData.checkChange()){															//���� ��������� ���������� ����� �������� ����� USB ����
			cheackValidDataexchange(pdata);														//�������� ���������� ������ � ��������� 
			display.setBrightness(pdata.brightness);											//��������������� �������� ������� �������
			if (!pdata.backLightEnable){														//���� ���������� ������� ���������
				display.onDispley();															//������� ���������� ����� ��������� ���������.
			}
			pid.setKp(pdata.Kp);																//��������� ����������������� ������������.
			pid.setKi(pdata.Ki);																//��������� ������������� ������������.
			pid.setKd(pdata.Kd);																//��������� ����������������� ������������.
			pid.setPidSP(pdata.setPointTemperature);											//��������� ����� �������.
			pid.setMinPwmTime(pdata.minPwmTime);												//��������� ������������ ������� ���.
			pid.setStepsPwm(pdata.stepsPWM);													//��������� ���������� ����� ���.
			pid.setTime(pdata.idTime);															//��������� ������� ��������������/�����������������.
				
			eeprom_update_block(&pdata.backLightEnable, &eeBackLightEnable, sizeof(bool));		//���������� � EEPROM ��������� ���������� ���������� ��������� 
			eeprom_update_byte(&eeBrightness, pdata.brightness);								//���������� � EEPROM ������� ���������
			eeprom_update_word(&eeBackLightTime, pdata.backLightTime);							//���������� � EEPROM ������� ���������� ���������
			eeprom_update_block(&pdata.Kp, &eeKp, sizeof(float));								//���������� � EEPROM ����������������� ������������
			eeprom_update_block(&pdata.Ki, &eeKi, sizeof(float));								//���������� � EEPROM ������������� ������������
			eeprom_update_block(&pdata.Kd, &eeKd, sizeof(float));								//���������� � EEPROM ����������������� ������������
			eeprom_update_word(&eeMinSetPoint, pdata.minSetPoint);								//���������� � EEPROM ���������� ���������� �������� �������
			eeprom_update_word(&eeMaxSetPoint, pdata.maxSetPoint);								//���������� � EEPROM ���������� ���������� �������� �������
			//eeprom_update_word(&eeSetPointTemperature, pdata.setPointTemperature);				//���������� � EEPROM �������
			eeprom_update_word(&eeIDTime, pdata.idTime);										//���������� � EEPROM ������� ��������������/�����������������
			eeprom_update_word(&eeMinPwmTime, pdata.minPwmTime);								//���������� � EEPROM ������������ ������� ���
			eeprom_update_word(&eeStepsPWM, pdata.stepsPWM);									//���������� � EEPROM ����� ���
		}
		//**************************************** ����� ����� ���������� ��������, ���� ���� �������� ��� ������� ****************************************//
		
		//************************************************* ���� ���������� � EEPROM �������� ������� sp **************************************************//
		if (updateEepSP.checkChange()){															//���� ���� �������� �������� �������
			eepSaveTime.delayStop();															//��������������� ������ ���������� ������� � EEPROM
			eepSaveTime.delayStart(10000);														//������ ����������� ������.
			updateEepSP.updatValue();															//����������� �������� sp ��� ������������ ���������
		} else if (eepSaveTime.delayEnd()){														//���� �������� sp �� ���������� ������ ��� ����� ����������,
					eeprom_update_word(&eeSetPointTemperature, pdata.setPointTemperature);		//����������� ���������� �� �������� � EEPROM
			}
		//************************************************* ����� ����� ���������� � EEPROM �������� ������� sp **************************************************//
				
		//***************************************************************** ������� ��� ���������� ***************************************************************//
		pid.calculatePID(termo);																	//������ ������� ��� ������������� 
		//*************************************************************** ����� ����� ��� ���������� *************************************************************//
		
		//**************************************************************** ���� ���������� ������� ***************************************************************//
		timerDisplayUpdate.delayStart(DISPLAY_UPDATA_TIME);										//������ ������� ���������� �������
		if( timerDisplayUpdate.delayEnd() || updateSP.checkChange() ){							//���� ������ ������� �������� ��� �������� �������,
			if (termo.convert()){																//����������� ����������� �����������.
				display.printVP(uint16_t(round(termo.getTemperature())));						//���� ����������� �������, �������� ��������� �� �������.
			}
			else{
				display.printVPerr();															//���� ����������� �� ���������, ��������� ������
			}
			display.printSP(pdata.setPointTemperature);											//�������� ������� ��������� �� �������.
			updateSP.updatValue();																//����������� ����������� ��������.
		}		
		//************************************************************ ����� ����� ���������� ������� ***********************************************************//
		
		//********************************************************* ���� ���������� ������� ��� ���������� ******************************************************//	
		if (updatePidSP.checkChange()){															//���� �������� ������� ����������
			pid.setPidSP(pdata.setPointTemperature);											//����������� �������� ������� ��� ����������.
			updatePidSP.updatValue();															//����������� ����������� ��������.
		}
		//********************************************************* ���� ���������� ������� ��� ���������� ******************************************************//	
	}
}







