/* main.cpp
 * Создан: 25.02.2024 02:37:40
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 */ 
#define F_CPU 12000000UL										//Установка частоты процессора.
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

#define START_DELAY 2000										//Задержка при включении питания 2 секунды
#define INIT_MODULS_TIME 200									//Задержка для инициализации модулей 0,2 секунды
#define USB_RESET_TIME 300										//Время сброса USB шины
#define DISPLAY_UPDATA_TIME 1000								//Таймаут обновления информации на дисплее
#define VUSB_MONITOR_ACTIV(); DDRD&=~(1<<VUSB_MONITOR_PIN);		//Макрос настройки вывода порта для контроля напряжения с USB разъема
#define VUSB_MONITOR_PORT PIND									//Порт для чтения состояния напряжения USB хоста
#define VUSB_MONITOR_PIN PD3									//Номер пина для контроля напряжения
	
bool EEMEM	eeBackLightEnable = true;							//Начальные значения в энергонезависимой памяти
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

uint32_t timeCounter = 0;										//Глобальная переменная для работы класса Time
dataexchange_t pdata;											//Структура данных для хранения рабочих настроек регулятора

PidRegula pid(PORTB, PB3);										//Создание объекта ПИД регулятора
bool flagButtonPress = false;									//Флаг нажатия кнопки для управления подсветкой
bool flagUsbLableOn_1 = false;									//Флаг для управления значком USB
bool flagUsbLableOn_2 = false;									//Второй флаг для управления значком USB

int main(void){
	
	_delay_ms(START_DELAY );																	//Задержка на включение при подаче питания	
	
	eeprom_read_block(&pdata.backLightEnable, &eeBackLightEnable, sizeof(bool));				//Чтение из EEPROM параметра разрешения отключения подсветки 
	pdata.brightness = eeprom_read_byte(&eeBrightness);											//Чтение из EEPROM яркости подсветки
	pdata.backLightTime = eeprom_read_word(&eeBackLightTime);									//Чтение из EEPROM времени отключения подсветки
	eeprom_read_block(&pdata.Kp, &eeKp, sizeof(float));											//Чтение из EEPROM пропорционального коэффициента
	eeprom_read_block(&pdata.Ki, &eeKi, sizeof(float));											//Чтение из EEPROM интегрального коэффициента
	eeprom_read_block(&pdata.Kd, &eeKd, sizeof(float));											//Чтение из EEPROM дифференциального коэффициента
	pdata.minSetPoint = eeprom_read_word(&eeMinSetPoint);										//Чтение из EEPROM минимально допустимой величины уставки
	pdata.maxSetPoint = eeprom_read_word(&eeMaxSetPoint);										//Чтение из EEPROM минимально допустимой величины уставки
	pdata.setPointTemperature = eeprom_read_word(&eeSetPointTemperature);						//Чтение из EEPROM уставки
	pdata.idTime = eeprom_read_word(&eeIDTime);													//Чтение из EEPROM времени интегрирования/дифференцирования
	pdata.minPwmTime = eeprom_read_word(&eeMinPwmTime);											//Чтение из EEPROM минимального времени ШИМ
	pdata.stepsPWM = eeprom_read_word(&eeStepsPWM);												//Чтение из EEPROM шагов ШИМ
	cheackValidDataexchange(pdata);																//Проверка валидности данных прочитанных из энергонезависимой памяти
	SPI_UNIT spi;																				//Создание программного объекта шины SPI 
	spi.init();																					//Инициализация программного объекта шины SPI
	Max6675 termo(spi);																			//Создание объекта АЦП термодатчика
	Displey_ssd1306_128x64 display;																//Создание объекта дисплея
	display.init(spi);																			//Инициализация дисплея
	display.cleare();																			//Очистка дисплея
	_delay_ms(INIT_MODULS_TIME);																//Задержка для нормальной инициализации модулей периферии
	display.printLogo();																		//Печать логотипа 
	Button button_up(PORTA, PA2);																//Создание объекта кнопки "больше"
	Button button_down(PORTB, PB4);																//Создание объекта кнопки "меньше"
	Time timerDisplayUpdate,																	//Таймер для обновления дисплея  
		 eepSaveTime,																			//Таймер для сохранения уставки в энергонезависимую память
		 timerBackLightOff;																		//Таймер отключения подсветки

	CheckChangeValue<uint16_t>  updateSP(pdata.setPointTemperature),							//Объект для контроля изменения уставки
								updateEepSP(pdata.setPointTemperature),							//для сохранения в энергонезависимую память,
								updatePidSP(pdata.setPointTemperature),							//для передачи ПИД регулятору.
								updateBackLightTime(pdata.backLightTime);						//Объект для контроля за изменением времени отключения дисплея
	CheckChangeValue<dataexchange_t> updatePData(pdata);										//Объект для контроля изменения настроек устройства
	{
		float vp;
		display.printSP(pdata.setPointTemperature);												//Вывод значения уставки на экран
		if (termo.convert()){																	//Запрос на термопреобразование
			vp = termo.getTemperature();														//Получение текущей температуры
			display.printVP(uint16_t(round(vp)));												//Округление значения и вывод на экран
		}
		else{
				display.printVPerr();															//Если не удалось выполнить термопреобразование, 
		}																						//на дисплее отображается ошибка
		pid.setPidSP(pdata.setPointTemperature);
		pid.setStartVP(uint16_t(vp));
	}		
	display.setBrightness(pdata.brightness);													//Устанавливается значение яркости дисплея
	pid.setKp(pdata.Kp);																		//Установка пропорционального коэффициента.
	pid.setKi(pdata.Ki);																		//Установка интегрального коэффициента.
	pid.setKd(pdata.Kd);																		//Установка дифференциального коэффициента.
	pid.setMinPwmTime(pdata.minPwmTime);														//Установка минимального времени ШИМ.
	pid.setStepsPwm(pdata.stepsPWM);															//Установка количества шагов ШИМ.
	pid.setTime(pdata.idTime);																	//Установка времени интегрирования/дифференцирования.
	usbInit();																					//Инициализации библиотеки V-USB
 	cli();																						//Запрет всех прерываний
 	usbDeviceDisconnect();																		//Принудительное отключение от хоста
 	_delay_ms(USB_RESET_TIME);																	//Время отключения от хоста, не менее 250 ms
 	usbDeviceConnect();																			// Подключение к хосту
	sei();																						//Глобальное разрешение прерываний
	VUSB_MONITOR_ACTIV();																		//Активация контроля напряжения от USB хоста

    while (1) {
		//***************************************************  Блок функций для работы с шиной usb ********************************************************//
		if ((VUSB_MONITOR_PORT&(1<<VUSB_MONITOR_PIN))!=0){										//Если обнаружено питание от хоста, 
			usbDeviceConnect();																	//то пора подключаться к шине USB
		}
		else{
			usbDeviceDisconnect();																//Если питание пропало, нужно отключиться
		}
			usbPoll();																			// Функция обработки сообщений от хоста
		if(flagUsbLableOn_1){																	//Часть программы для управления отображением 
			if((VUSB_MONITOR_PORT&(1<<VUSB_MONITOR_PIN))==0){									//логотипа USB шины на дисплее при
 				display.deleteUSBlable();														//подключении устройства к хосту.
				flagUsbLableOn_1 = false;														//Если напряжение на контрольном пине пропадает,
				flagUsbLableOn_2 = false;														//устройство отключается от шины и логотип пропадает.
			}
			else{
				if (!flagUsbLableOn_2){															//Если напряжение на контрольном пине появляется
					display.printUSBlable();													//и происходит обмен репортам при инициализации устройства
					flagUsbLableOn_2 = true;													//логотип появляется на экране.
				}
				
			}							 
 		}
		//***************************************************  Конец блока функций для работы с шиной usb ********************************************************//	
		
		//********************************************* Блок управления подсветкой и обработки нажатий кнопок *********************************************//
		int8_t k;																				//Переменная, определяющая шаг изменения параметра
		if (updateBackLightTime.checkChange()){													//Если флаг разрешения отключения подсветки установлен,
			timerBackLightOff.delayStop();														//таймер отключения подсветки останавливается и сбрасывается.
			if(pdata.backLightEnable){															//Если отключение подсветки разрешено,
				updateBackLightTime.updatValue();												//таймер отключения подсветки запускается заново.
			}
			
		}
		if (pdata.backLightEnable && display.getBacklightStatus()){								//Если управление подсветкой разрешено и подсветка включена,
			timerBackLightOff.delayStart((uint32_t)(pdata.backLightTime) * 1000);				//Запускается таймер на отключение подсветки
		}
		if (pdata.backLightEnable && !display.getBacklightStatus()) {							//Если отключение подсветки разрешено и подсветка выключена, шаг изменения параметра 
			k=0; }																				//для кнопок равен нулю (при нажатии параметр не изменится, но признак нажатия вернется).
		else {																					//Во всех других случаях шаг равен 1, параметр либо увеличивается на 1 или уменьшается
			k=1; }																				//признак нажатия кнопки так же возвращается.
		button_up.checkActiv(pdata.setPointTemperature, k, pdata.minSetPoint, 
							 pdata.maxSetPoint, &flagButtonPress);								//Обработка нажатия кнопки "больше".
		button_down.checkActiv(pdata.setPointTemperature, -k, pdata.minSetPoint, 
							   pdata.maxSetPoint, &flagButtonPress);							//Обработка нажатия кнопки "меньше".
		if (!display.getBacklightStatus() && flagButtonPress){									//Если подсветка дисплея отключена и было нажатие кнопки, то
			display.onDispley();																//подсветка дисплея включается.
			flagButtonPress = false;															//Флаг признака нажатия кнопки сбрасывается
			timerBackLightOff.delayStop();														//Отключается и сбрасывается таймер отключения подсветки.
			timerBackLightOff.delayStart((uint32_t)(pdata.backLightTime) * 1000);				//Таймер отключения подсветки перезапускается заново.
		}
		if(timerBackLightOff.delayEnd()){														//Если таймер отключения подсветки сработал,
			display.offDispley();																//подсветка отключается.
		}
		//****************************************** Конец блок управления подсветкой и обработки нажатий кнопок ******************************************//
		
		//******************************************** Блок обновления настроек, если лист настроек был изменен *******************************************//
		if (updatePData.checkChange()){															//Если произошло обновление листа настроек через USB порт
			cheackValidDataexchange(pdata);														//Проверка валидности данных в структуре 
			display.setBrightness(pdata.brightness);											//Устанавливается значение яркости дисплея
			if (!pdata.backLightEnable){														//Если отключение дисплея запрещено
				display.onDispley();															//дисплей включается после изменения параметра.
			}
			pid.setKp(pdata.Kp);																//Установка пропорционального коэффициента.
			pid.setKi(pdata.Ki);																//Установка интегрального коэффициента.
			pid.setKd(pdata.Kd);																//Установка дифференциального коэффициента.
			pid.setPidSP(pdata.setPointTemperature);											//установка новой уставки.
			pid.setMinPwmTime(pdata.minPwmTime);												//Установка минимального времени ШИМ.
			pid.setStepsPwm(pdata.stepsPWM);													//Установка количества шагов ШИМ.
			pid.setTime(pdata.idTime);															//Установка времени интегрирования/дифференцирования.
				
			eeprom_update_block(&pdata.backLightEnable, &eeBackLightEnable, sizeof(bool));		//Обновление в EEPROM параметра разрешения отключения подсветки 
			eeprom_update_byte(&eeBrightness, pdata.brightness);								//Обновление в EEPROM яркости подсветки
			eeprom_update_word(&eeBackLightTime, pdata.backLightTime);							//Обновление в EEPROM времени отключения подсветки
			eeprom_update_block(&pdata.Kp, &eeKp, sizeof(float));								//Обновление в EEPROM пропорционального коэффициента
			eeprom_update_block(&pdata.Ki, &eeKi, sizeof(float));								//Обновление в EEPROM интегрального коэффициента
			eeprom_update_block(&pdata.Kd, &eeKd, sizeof(float));								//Обновление в EEPROM дифференциального коэффициента
			eeprom_update_word(&eeMinSetPoint, pdata.minSetPoint);								//Обновление в EEPROM минимально допустимой величины уставки
			eeprom_update_word(&eeMaxSetPoint, pdata.maxSetPoint);								//Обновление в EEPROM минимально допустимой величины уставки
			//eeprom_update_word(&eeSetPointTemperature, pdata.setPointTemperature);				//Обновление в EEPROM уставки
			eeprom_update_word(&eeIDTime, pdata.idTime);										//Обновление в EEPROM времени интегрирования/дифференцирования
			eeprom_update_word(&eeMinPwmTime, pdata.minPwmTime);								//Обновление в EEPROM минимального времени ШИМ
			eeprom_update_word(&eeStepsPWM, pdata.stepsPWM);									//Обновление в EEPROM шагов ШИМ
		}
		//**************************************** Конец блока обновления настроек, если лист настроек был изменен ****************************************//
		
		//************************************************* Блок сохранение в EEPROM значения уставки sp **************************************************//
		if (updateEepSP.checkChange()){															//Если была изменена величина уставки
			eepSaveTime.delayStop();															//Останавливается таймер сохранения уставки в EEPROM
			eepSaveTime.delayStart(10000);														//Таймер запускается заново.
			updateEepSP.updatValue();															//Обновляется величина sp для отслеживания изменений
		} else if (eepSaveTime.delayEnd()){														//Если величина sp не изменялась дольше чем время сохранения,
					eeprom_update_word(&eeSetPointTemperature, pdata.setPointTemperature);		//выполняется обновление ее значения в EEPROM
			}
		//************************************************* Конец блока сохранение в EEPROM значения уставки sp **************************************************//
				
		//***************************************************************** Запуска ПИД регулятора ***************************************************************//
		pid.calculatePID(termo);																	//Запуск функции ПИД регулирования 
		//*************************************************************** Конец блока ПИД регулятора *************************************************************//
		
		//**************************************************************** Блок обновления дисплея ***************************************************************//
		timerDisplayUpdate.delayStart(DISPLAY_UPDATA_TIME);										//Запуск таймера обновления дисплея
		if( timerDisplayUpdate.delayEnd() || updateSP.checkChange() ){							//Если таймер дисплея сработал или изменена уставка,
			if (termo.convert()){																//выполняется конвертация температуры.
				display.printVP(uint16_t(round(termo.getTemperature())));						//Если конвертация успешна, значение выводится на дисплей.
			}
			else{
				display.printVPerr();															//Если конвертация не выполнена, выводится авария
			}
			display.printSP(pdata.setPointTemperature);											//Значение уставки выводится на дисплей.
			updateSP.updatValue();																//Обновляется контрольное значение.
		}		
		//************************************************************ Конец блока обновления дисплея ***********************************************************//
		
		//********************************************************* Блок обновления уставки ПИД регулятора ******************************************************//	
		if (updatePidSP.checkChange()){															//Если значение уставки поменялось
			pid.setPidSP(pdata.setPointTemperature);											//обновляется значение уставки ПИД регулятора.
			updatePidSP.updatValue();															//Обновляется контрольное значение.
		}
		//********************************************************* Блок обновления уставки ПИД регулятора ******************************************************//	
	}
}







