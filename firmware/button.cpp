/* button.cpp
 * Создан: 13.03.2024 23:51:44
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Объект этого класса обслуживает нажатие кнопки, как
 * в режиме  единичного нажатия так и в режиме перелистывания.
 */ 

#include "button.h"

Button::Button(volatile uint8_t & buttonPort, uint8_t buttonPin){						//Конструктор объекта
	port = & buttonPort;																//Назначение порта кнопки
	pin = buttonPin;																	//Назначение номера пина кнопки
	*(port-1) &= ~(1 << pin);															//Настройка линии порта на прием
	*(port) |= 1 << pin;																//Подключение подтягивающего резистора к плюсу
	flagActiv = false;
}
void Button::checkActiv(uint16_t & value, int8_t step, uint16_t minValue,				//Метод, обслуживающий нажатие кнопки
						uint16_t maxValue, bool * flagPress){
	if ( cheakStatus() && flagActiv == false){											//Если состояние кнопки нажато, и флаг активного состояния не установлен
		flagActiv = true;																//Устанавливается флаг нажатия
		timeRattling.delayStart(RATTLING_TIME);											//Запускается таймер устранения дребезга контактов
	}																					//Если  кнопка уже нажата ранее и за время устранения 
	else if (flagActiv == true && cheakStatus() && 	timeRattling.delayEnd()){			//дребезга контактов ее состояние не изменилось,														 
		if (flagPress){																	//Если указатель на внешний флаг передан методу, то он покажет,
			*flagPress = true;															// что кнопка была нажата и по ее нажатию выполнено действие.							
		}
		if ( (step > 0 && maxValue - step >= value) ||									//выполняется изменение параметра, переданного по ссылке value
			 (step < 0 && minValue - step <= value) ){									//c учетом контроля интервала разрешенных значений.
				value += step;
		}	
		timeWiting.delayStart(WAITING_TIME);											//Запускается таймер, который разрешит листающий режим
	}
	else if (flagActiv == true && !cheakStatus()){										//Если обнаружено что кнопка была ранее нажата и уже отпущена,
		flagActiv = false;																//Флаг нажатия кнопки сбрасывается
		timeRattling.delayStop();														//Все таймеры останавливаются,
		timeWiting.delayStop();															//состояние объекта кнопка возвращается в исходное состояние.
		timeFlipping.delayStop();
	}
	else if (flagActiv == true && cheakStatus() && timeWiting.delayEnd()){				//Если кнопка все еще нажата и выдержка режима перелистывания прошла
		if (flagPress){																	//Если указатель на внешний флаг передан методу, то он покажет,
			*flagPress = true;															// что кнопка была нажата и по ее нажатию выполнено действие.
		}
		if (  (step > 0 && maxValue - step >= value) ||									//Включается режим перелистывания, который изменяет значение  
		      (step < 0 && minValue - step <= value) ) value += step;					//параметра с выдержкой времени
		timeFlipping.delayStart(FLIPPING_TIME);											//определенной этим таймером.
	}
	else if (flagActiv == true && cheakStatus() && timeFlipping.delayEnd()){			//Если кнопка все еще нажата и выдержка перелистывания параметра прошла:
				if (flagPress){															//если указатель на внешний флаг передан методу, то он покажет,
			*flagPress = true;															// что кнопка была нажата и по ее нажатию выполнено действие.
		}
	if ( (step > 0 && maxValue - step >= value) ||										//Происходит изменение параметра с 
		 (step < 0 && minValue - step <= value) )										//учетом интервала допустимых значений.
			value += step;
	timeFlipping.delayStart(FLIPPING_TIME);												//Снова запускается таймер перелистывания, 
	}																					//и так будет продолжаться, пока кнопка не будет отпущена.
}
inline bool Button::cheakStatus(){														//Метод, определяющий активное состояние на пине подключения кнопки
	if(( *(port-2) & (1 << pin))==0) return true;										//если кнопка замкнута (на пине кнопки 0 вольт), возвращает true.
	else return false;
}


