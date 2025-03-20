/* usb.h
 * Создан: 03.04.2024 18:41:01
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Данный раздел проекта содержит типы переменных и функции, 
 * предназначенные для создания и обслуживания usb соединения.
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

#define MAX_BRIGHTNESS 100												//Максимальная яркость дисплея, %
#define MIN_BRIGHTNESS 10												//Минимальная яркость дисплея, %
#define MIN_BAK_LIGHT_TIME 10											//Минимальное время до выключения подсветки в секунда, сек
#define MAX_KP 1000 													//Максимальное значение пропорционального коэффициента
#define MIN_KP 0														//Минимальное значение пропорционального коэффициента
#define MAX_KI 1000 													//Максимальное значение интегрально коэффициента
#define MIN_KI 0														//Минимальное значение интегрального коэффициента
#define MAX_KD 1000 													//Максимальное значение дифференциально коэффициента
#define MIN_KD 0														//Минимальное значение дифференциально коэффициента
#define MIN_ID_TIME 100													//Минимальное время интегрирования/дифференцирования, мсек.
#define MIN_MIN_PWM_TIME 10												//Минимальная величина ШИМ, минимальное значение, мсек.
#define MIN_STEPS_PWM 10												//Минимально возможное количество ступеней ШИМ на период
#define MAX_SET_POINT 1000												//Максимальная величина уставки
#define MIN_SET_POINT 0													//Минимальная величина уставки

typedef struct dataexchange_t {											//Структура для передачи данных (лист настроек)
	bool backLightEnable;												//Разрешение отключения подсветки
    uint8_t brightness;													//Яркость подсветки
    uint16_t backLightTime;												//Время до отключения подсветки
    float Kp;															//Пропорциональный коэффициент ПИД-регулятора
    float Ki;															//Интегральный коэффициент ПИД-регулятора
    float Kd;															//Дифференциальный коэффициент ПИД-регулятора
    float pidError;														//Ошибка ПИД регулятора
	float Proposh;														//Пропорциональная составляющая ПИД регулятора
	float Integr;														//Интегральная составляющая ПИД регулятора
    float Differ;														//Дифференциальная составляющая ПИД регулятора
    float out;															//Выходной сигнал ШИМ
	float currentTemperature;											//Текущая температура
	uint16_t minSetPoint;												//Минимальная уставка
	uint16_t maxSetPoint;												//Максимальная уставка
	uint16_t setPointTemperature;										//Уставка температуры
	uint16_t idTime;													//Время интегрирования/дифференцирования
    uint16_t minPwmTime;												//Минимальное время импульса ШИМ
    uint16_t stepsPWM;													//Количество ступеней шим
	bool operator == (const dataexchange_t& a){							//Перегруженный оператор сравнения структур
		return (														//Сравнение производится по полям, которые содержат  
					(backLightEnable == a.backLightEnable) &&			//управляющие значения. Если поля структуры эти поля  
					(brightness == a.brightness) &&						//имеют одинаковые значения, то структуры равны. 
					(backLightTime == a.backLightTime) &&				//Информационные поля в сравнении не участвуют.
					(Kp == a.Kp) && (Ki == a.Ki) && (Kd == a.Kd) &&  
					(idTime == a.idTime) && 
					(minPwmTime == a.minPwmTime) && 
					(stepsPWM == a.stepsPWM) &&
					(setPointTemperature == a.setPointTemperature)
				);
	}
};
																		// Эти переменные хранят статус текущей передачи.
static uchar    currentAddress;											//Адрес начала непрочитанного блока.
static uchar    bytesRemaining;											//Счетчик непрочитанных из буфера данных.
extern dataexchange_t pdata;											//Внешняя переменная, хранящая настройки устройства при передаче данных
extern PidRegula pid;
extern bool flagUsbLableOn_1;
void cheackValidDataexchange(dataexchange_t & data);					//Функция проверки валидности данных в структуре
uchar   usbFunctionRead(uchar *data, uchar len, PidRegula const & pid);	//Вызывается когда хост запрашивает порцию данных от устройства
uchar   usbFunctionWrite(uchar *data, uchar len);						//Вызывается когда хост отправляет порцию данных к устройству
usbMsgLen_t usbFunctionSetup(uchar data[8]);							//Функция используется для настройки параметров соединения USB
#endif /* USB_H_ */
