/* pid.cpp
 * Файл реализации класса ПИД регулятора
 * Создан: 16.03.2024 23:47:58
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 */ 

#include "pid.h"
#include "oddebug.h"

	PidRegula::PidRegula(volatile uint8_t & portOut, int8_t pinOut){				
		pidOutPort = & portOut;							//Конструктор объекта ПИД регулятора
		pidOutPin = pinOut;								//Настраивает пин порта, как выход
		*(pidOutPort-1)|=(1<<pidOutPin);
	}
	void PidRegula::calculatePID(Max6675 & max6675){
		timer.delayStart(integDifferTime);				//Каждый раз когда истекает время выборки ПИД регулятора
		if (timer.delayEnd()){							//запускается процедура вычисления необходимой мощности.
			if (max6675.convert()){						//Запускается процедура термопреобразования
				VP = max6675.getTemperature();			//и у термопреобразователя запрашивается текущая температура.
				pidError = sp - VP;						//Вычисляется ошибка ПИД регулятора, это разница между предыдущим 
				Proposh = Kp*pidError;					//и текущим значениями температуры.
				if (Proposh > float(stepsPwm)){
					Proposh = float(stepsPwm);			//Вычисляется пропорциональная составляющая и ограничивается ее
				}										//величина максимальной величиной выхода ШИМ. 
				if (Proposh < -float(stepsPwm)){
					Proposh = -float(stepsPwm);
				}
				if(Ki > 0){								//Если интегральный коэффициент больше нуля, то 
					Integr = Integr + Ki*pidError;		//вычисляется интегральная составляющая.
					if (Integr > float(stepsPwm)){		
						Integr = float(stepsPwm);		//Ее значения так же ограничиваются
					}									//по величине.
					if (Integr < 0){					
						Integr = 0;						
					}
				}				
				else {									
					Integr=0;							//Если пропорциональный коэффициент меньше нуля, то интегральная составляющая равна 0.		
				}									
				Differ = (pidError - pidErrorOld)*Kd;	//Вычисляется дифференциальная составляющая

				if (Differ > float(stepsPwm)){			//Дифференциальная составляющая так же ограничивается 
					Differ = float(stepsPwm);			//по величине.
				}
				if (Differ < -float(stepsPwm)){
					Differ = -float(stepsPwm);
				}			
				pidErrorOld = pidError;					//Сохраняется значение ошибки для следующего вычисления
				backVeriablePoint = VP;					//Сохраняется значение температуры для следующего вычисления	
				out = Proposh + Integr - Differ;		//Вычисляется выходная величина регулятора
				if (out > float(stepsPwm)){				
					out = float(stepsPwm);				//Она так же ограничивается по величине.
				}
				if (out < 0){
					out = 0;							//Если выходная величина меньше 0							
				}										//выходная величина равна 0
			}
			else {										
				out = 0;								
				Proposh = 0;							//Если получить значение текущей температуры не получилось
				Integr = 0;								//ПИД регулятор отключает выход полностью.
				Differ = 0;
			}
		}
		pwm();											//Формируется выходной сигнал ШИМ
	}
	inline bool PidRegula::cheackPwmPinActive(){
		return *(pidOutPort) & (1 << pidOutPin);
	}
	float PidRegula::getCarentTemperature(){
		return VP;
	}
	float PidRegula::getDiffer(){
		return Differ;
	}
	uint16_t PidRegula::getIntegDifferTime(){
		return integDifferTime;
	}
	float PidRegula::getIntegr(){
		return Integr;
	}
	float PidRegula::getKd(){
		return Kd;
	}
	float PidRegula::getKi(){
		return Ki;
	}
	float PidRegula::getKp(){
		return Kp;
	}
	uint16_t PidRegula::getMinPwmTime(){
		return minPwmTime;
	}	
	float PidRegula::getOut(){
		return out;
	}	
	float PidRegula::getPidError(){
		return pidError;
	}
	float PidRegula::getProposh(){
		return Proposh;
	}
	uint16_t PidRegula::getStepsPwm(){
		return stepsPwm;
	}
	uint16_t PidRegula::getSetPoint(){
		return sp;
	}
	void PidRegula::pwm(){
		timerPwmSteps.delayStart(minPwmTime);					
		if (timerPwmSteps.delayEnd()){
			if (counterPWM < stepsPwm) counterPWM++;
				else counterPWM =0;
			if (counterPWM > out && cheackPwmPinActive()){
				*(pidOutPort)&=~(1<<pidOutPin);
			} 
			if (counterPWM < out && !cheackPwmPinActive()){
				*(pidOutPort) |= 1<<pidOutPin;
			}
		}
	}
	void PidRegula::setKd(float KD){
		Kd=KD;
	}
	void PidRegula::setKi(float KI){
		Ki = KI;
	}
	void PidRegula::setKp(float KP){
		Kp=KP;
	}
	void PidRegula::setMinPwmTime(uint16_t MinPwmTime){
		minPwmTime = MinPwmTime;
	}
	void PidRegula::setPidSP(uint16_t setPointNew){
		sp = setPointNew;
	}	
	void PidRegula::setStartVP(uint16_t value){
		backVeriablePoint = value;
	}
	void PidRegula::setStepsPwm(uint16_t StepsPwm){
		stepsPwm = StepsPwm;
	}
	void PidRegula::setTime(uint16_t IntegDifferTime){
		integDifferTime = IntegDifferTime;
	}
