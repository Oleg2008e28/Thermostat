/* usb.cpp
 * ������: 03.04.2024 18:41:19
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * ������ ������ ������� �������� ���� ���������� � �������, 
 * ��������������� ��� �������� � ������������ usb ����������.
 */ 
#include "usb.h"
																// ���������� ��������� ��������� ������ ������ ��� ������
PROGMEM const char usbHidReportDescriptor[22] = {				// USB report descriptor         
	0x06, 0x00, 0xff,											// USAGE_PAGE (Generic Desktop)
	0x09, 0x01,													// USAGE (Vendor Usage 1)
	0xa1, 0x01,													// COLLECTION (Application)
	0x15, 0x00,													// LOGICAL_MINIMUM (0) - min. �������� ��� ������
	0x26, 0xff, 0x00,											// LOGICAL_MAXIMUM (255) - max. �������� ��� ������, 255 
	0x75, 0x08,													// REPORT_SIZE (8) - ���������� ���������� ��������, ��� ������ ������ "�������" 8 ���
	0x95, sizeof(dataexchange_t),								// REPORT_COUNT - ���������� ������ 
	0x09, 0x00,													// USAGE (Undefined)
	0xb2, 0x02, 0x01,											// FEATURE (Data,Var,Abs,Buf)
	0xc0														// END_COLLECTION
};

void cheackValidDataexchange(dataexchange_t & d){
	if (d.brightness > MAX_BRIGHTNESS){d.brightness = MAX_BRIGHTNESS;}					//������������ ����������� �������� ������� ���������
	if (d.brightness < MIN_BRIGHTNESS){d.brightness = MIN_BRIGHTNESS;}					//����������� ����������� �������� ������� ���������
	if (d.backLightTime < MIN_BAK_LIGHT_TIME){d.backLightTime = MIN_BAK_LIGHT_TIME;}		//����������� ������������ ������� ���������� ���������	
	if (d.Kp > MAX_KP){d.Kp = MAX_KP;}													//����������� ������������� ����������������� ������������ 
	if (d.Kp < MIN_KP){d.Kp = MIN_KP;}													//����������� ������������ ����������������� ������������ 	
	if (d.Ki > MAX_KI){d.Ki = MAX_KI;}													//����������� ������������� ������������� ������������ 
	if (d.Ki < MIN_KI){d.Ki = MIN_KI;}													//����������� ������������ ������������� ������������ 
	if (d.Kd > MAX_KD){d.Kd = MAX_KD;}													//����������� ������������� ����������������� ������������ 
	if (d.Kd < MIN_KD){d.Kd = MIN_KD;}													//����������� ������������ ����������������� ������������
	if (d.idTime < MIN_ID_TIME){d.idTime = MIN_ID_TIME;}								//����������� ������������ ������� ��������������	
	if (d.minPwmTime < MIN_MIN_PWM_TIME){d.minPwmTime = MIN_MIN_PWM_TIME;}					//����������� ����������� ������������ �������� ���
	if(d.stepsPWM < MIN_STEPS_PWM){d.stepsPWM = MIN_STEPS_PWM;}							//����������� ������������ ����� �������� ��� �� ������
	if(d.maxSetPoint > MAX_SET_POINT){d.maxSetPoint = MAX_SET_POINT;}					//����������� ������������ �������� �������
	if(d.minSetPoint > d.maxSetPoint){d.minSetPoint = d.maxSetPoint;}					//����������� ����������� �������� �������
	if(d.setPointTemperature > d.maxSetPoint){d.setPointTemperature = d.maxSetPoint;}	//����������� ������������� �������� ������� �������
	if(d.setPointTemperature < d.minSetPoint){d.setPointTemperature = d.minSetPoint;}	//����������� ������������ �������� ������� �������1
}

uchar   usbFunctionRead(uchar *data, uchar len){					//������� ���������� ����� ���� ����������� ������ ������ �� ����������.
    if(len > bytesRemaining){										//������������ ����� ���������, ������� ���������� ���������
        len = bytesRemaining;
	}
    uchar *buffer = (uchar*)&pdata;									//� ����� ������ ���������� ��������� �� ���������, �������� ��������� ����������
	if(!currentAddress){											//���� ������ �� ������, ����������� ������ ���� ���������.
		pdata.Kp = pid.getKp();
		pdata.Ki = pid.getKi();
		pdata.Kd = pid.getKd();
		pdata.pidError = pid.getPidError();
		pdata.Proposh = pid.getProposh();
		pdata.Integr = pid.getIntegr();
		pdata.Differ = pid.getDiffer();		
		pdata.out = pid.getOut();
		pdata.currentTemperature = pid.getCarentTemperature();
		pdata.setPointTemperature = pid.getSetPoint();
		pdata.idTime = pid.getIntegDifferTime();
		pdata.minPwmTime = pid.getMinPwmTime();
		pdata.stepsPWM = pid.getStepsPwm();
    } 
    uchar j;
    for(j=0; j<len; j++){
        data[j] = buffer[j+currentAddress];							//������ ������ � ����� ��� ��������
	}
    currentAddress += len;											//���������� �������� ������ ��� �������� ���������� ������
    bytesRemaining -= len;											//��������� �������� ���������� ����
    return len;										
}

uchar   usbFunctionWrite(uchar *data, uchar len){					//������� ���������� ����� ���� ���������� ������ ������ ����������.
    if(bytesRemaining == 0){										//��� ������ ������ ��� ��������
        return 1;													//����� ��������
	}
    if(len > bytesRemaining){										//���� ������� ������ �������, �� 
        len = bytesRemaining;										//����� �������� len ���������� ����
	}
    uchar *buffer = (uchar*)&pdata;									//�������� ��������� �� ��������� ������ pdata
    uchar j;
    for(j=0; j<len; j++){
        buffer[j+currentAddress] = data[j];							//������������� ������ �� ������ ������ � ��������� pdata
	}
    currentAddress += len;											//���������� ������ �������� ������ ��� ������ ���������� ������
    bytesRemaining -= len;											//���������� �������� �������� ���������� ����
    return bytesRemaining == 0;										// 0 ��������, ��� ���� ��� ������ 
}

usbMsgLen_t usbFunctionSetup(uchar data[8]){						//������� ��������� ��������� �� USB �����
	usbRequest_t    *rq = (usbRequest_t *)data;
	if (!flagUsbLableOn_1){											//���� ������������ ����������� �� USB ��������
		flagUsbLableOn_1 = true;
	}
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){  //���� ��������� � HID ���������� ��
        if(rq->bRequest == USBRQ_HID_GET_REPORT){					//���������� ��� �������, ���� ����� ����. �������� ������.
            bytesRemaining = sizeof(dataexchange_t);				//������������� � ���������� ������� ���������� ������������ ����
            currentAddress = 0;										//������� ����� ������ �������� ��������� ������ ������ (�� pdata)
            return USB_NO_MSG;										//������ ���������� usbFunctionRead() ��� �������� ������ �����
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){				//���� ���� ����� ��������� ������
            bytesRemaining = sizeof(dataexchange_t);				//��������������� �������� � ������� ���������� ���� ��� ��������
            currentAddress = 0;										//������� �����, ���� ����� ���������� ��������� ������ ������ (� pdata)
            return USB_NO_MSG;										//������ ���������� usbFunctionWrite() ��� ��������� ������ �� �����
        }
    }
    return 0;
}
