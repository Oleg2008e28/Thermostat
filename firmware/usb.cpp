/* usb.cpp
 * Создан: 03.04.2024 18:41:19
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Данный раздел проекта содержит типы переменных и функции, 
 * предназначенные для создания и обслуживания usb соединения.
 */ 
#include "usb.h"
																// Дескриптор описывает структуру пакета данных для обмена
PROGMEM const char usbHidReportDescriptor[22] = {				// USB report descriptor         
	0x06, 0x00, 0xff,											// USAGE_PAGE (Generic Desktop)
	0x09, 0x01,													// USAGE (Vendor Usage 1)
	0xa1, 0x01,													// COLLECTION (Application)
	0x15, 0x00,													// LOGICAL_MINIMUM (0) - min. значение для данных
	0x26, 0xff, 0x00,											// LOGICAL_MAXIMUM (255) - max. значение для данных, 255 
	0x75, 0x08,													// REPORT_SIZE (8) - информация передается порциями, это размер одного "репорта" 8 бит
	0x95, sizeof(dataexchange_t),								// REPORT_COUNT - количество порций 
	0x09, 0x00,													// USAGE (Undefined)
	0xb2, 0x02, 0x01,											// FEATURE (Data,Var,Abs,Buf)
	0xc0														// END_COLLECTION
};

void cheackValidDataexchange(dataexchange_t & d){
	if (d.brightness > MAX_BRIGHTNESS){d.brightness = MAX_BRIGHTNESS;}					//Максимальное ограничение значения яркости подсветки
	if (d.brightness < MIN_BRIGHTNESS){d.brightness = MIN_BRIGHTNESS;}					//Минимальное ограничение значения яркости подсветки
	if (d.backLightTime < MIN_BAK_LIGHT_TIME){d.backLightTime = MIN_BAK_LIGHT_TIME;}		//Ограничение минимального времени отключения подсветки	
	if (d.Kp > MAX_KP){d.Kp = MAX_KP;}													//Ограничение максимального пропорционального коэффициента 
	if (d.Kp < MIN_KP){d.Kp = MIN_KP;}													//Ограничение минимального пропорционального коэффициента 	
	if (d.Ki > MAX_KI){d.Ki = MAX_KI;}													//Ограничение максимального интегрального коэффициента 
	if (d.Ki < MIN_KI){d.Ki = MIN_KI;}													//Ограничение минимального интегрального коэффициента 
	if (d.Kd > MAX_KD){d.Kd = MAX_KD;}													//Ограничение максимального дифференциального коэффициента 
	if (d.Kd < MIN_KD){d.Kd = MIN_KD;}													//Ограничение минимального дифференциального коэффициента
	if (d.idTime < MIN_ID_TIME){d.idTime = MIN_ID_TIME;}								//Ограничение минимального времени интегрирования	
	if (d.minPwmTime < MIN_MIN_PWM_TIME){d.minPwmTime = MIN_MIN_PWM_TIME;}					//Ограничение минимальной длительности импульса ШИМ
	if(d.stepsPWM < MIN_STEPS_PWM){d.stepsPWM = MIN_STEPS_PWM;}							//Ограничение минимального числа ступеней ШИМ на период
	if(d.maxSetPoint > MAX_SET_POINT){d.maxSetPoint = MAX_SET_POINT;}					//Ограничение максимальной величины уставки
	if(d.minSetPoint > d.maxSetPoint){d.minSetPoint = d.maxSetPoint;}					//Ограничение минимальной величины уставки
	if(d.setPointTemperature > d.maxSetPoint){d.setPointTemperature = d.maxSetPoint;}	//Ограничение максимального значения текущей уставки
	if(d.setPointTemperature < d.minSetPoint){d.setPointTemperature = d.minSetPoint;}	//Ограничение минимального значения текущей уставки1
}

uchar   usbFunctionRead(uchar *data, uchar len){					//Функция вызывается когда хост запрашивает порцию данных от устройства.
    if(len > bytesRemaining){										//Определяется длина сообщения, которое необходимо отправить
        len = bytesRemaining;
	}
    uchar *buffer = (uchar*)&pdata;									//В адрес буфера передается указатель на структуру, хранящую настройки устройства
	if(!currentAddress){											//Пока чтение не начато, заполняются нужные поля структуры.
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
        data[j] = buffer[j+currentAddress];							//Запись данных в буфер для передачи
	}
    currentAddress += len;											//Вычисление текущего адреса для передачи следующего пакета
    bytesRemaining -= len;											//Изменение счетчика оставшихся байт
    return len;										
}

uchar   usbFunctionWrite(uchar *data, uchar len){					//Функция вызывается когда хост отправляет порцию данных устройству.
    if(bytesRemaining == 0){										//Нет больше данных для передачи
        return 1;													//конец передачи
	}
    if(len > bytesRemaining){										//Если остался только хвостик, то 
        len = bytesRemaining;										//будет передано len оставшихся байт
	}
    uchar *buffer = (uchar*)&pdata;									//Передача указателя на структуру данных pdata
    uchar j;
    for(j=0; j<len; j++){
        buffer[j+currentAddress] = data[j];							//Переписывание данных из буфера приема в структуру pdata
	}
    currentAddress += len;											//Вычисление нового значения адреса для приема следующего пакета
    bytesRemaining -= len;											//Уменьшение значения счетчика оставшихся байт
    return bytesRemaining == 0;										// 0 означает, что есть еще данные 
}

usbMsgLen_t usbFunctionSetup(uchar data[8]){						//Функция обработки сообщений от USB хоста
	usbRequest_t    *rq = (usbRequest_t *)data;
	if (!flagUsbLableOn_1){											//Если сигнализатор подключения по USB выключен
		flagUsbLableOn_1 = true;
	}
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){  //Если обращение к HID устройству то
        if(rq->bRequest == USBRQ_HID_GET_REPORT){					//Определяем тип репорта, чего хочет хост. Получить данные.
            bytesRemaining = sizeof(dataexchange_t);				//Устанавливаем в переменную счетчик количество передаваемых байт
            currentAddress = 0;										//Текущий адрес начала отправки очередной порции данных (из pdata)
            return USB_NO_MSG;										//Дальше вызывается usbFunctionRead() для отправки данных хосту
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){				//Если хост хочет отправить данные
            bytesRemaining = sizeof(dataexchange_t);				//Устанавливается значение в счетчик оставшихся байт для передачи
            currentAddress = 0;										//Текущий адрес, куда нужно переписать очередную порцию данных (в pdata)
            return USB_NO_MSG;										//Дальше вызывается usbFunctionWrite() для получения данных от хоста
        }
    }
    return 0;
}
