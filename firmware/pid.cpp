/* pid.cpp
 * ���� ���������� ������ ��� ����������
 * ������: 16.03.2024 23:47:58
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 */ 

#include "pid.h"
#include "oddebug.h"

	PidRegula::PidRegula(volatile uint8_t & portOut, int8_t pinOut){				
		pidOutPort = & portOut;							//����������� ������� ��� ����������
		pidOutPin = pinOut;								//����������� ��� �����, ��� �����
		*(pidOutPort-1)|=(1<<pidOutPin);
	}
	void PidRegula::calculatePID(Max6675 & max6675){
		timer.delayStart(integDifferTime);				//������ ��� ����� �������� ����� ������� ��� ����������
		if (timer.delayEnd()){							//����������� ��������� ���������� ����������� ��������.
			if (max6675.convert()){						//����������� ��������� �������������������
				VP = max6675.getTemperature();			//� � �������������������� ������������� ������� �����������.
				pidError = sp - VP;						//����������� ������ ��� ����������, ��� ������� ����� ���������� 
				Proposh = Kp*pidError;					//� ������� ���������� �����������.
				if (Proposh > float(stepsPwm)){
					Proposh = float(stepsPwm);			//����������� ���������������� ������������ � �������������� ��
				}										//�������� ������������ ��������� ������ ���. 
				if (Proposh < -float(stepsPwm)){
					Proposh = -float(stepsPwm);
				}
				if(Ki > 0){								//���� ������������ ����������� ������ ����, �� 
					Integr = Integr + Ki*pidError;		//����������� ������������ ������������.
					if (Integr > float(stepsPwm)){		
						Integr = float(stepsPwm);		//�� �������� ��� �� ��������������
					}									//�� ��������.
					if (Integr < 0){					
						Integr = 0;						
					}
				}				
				else {									
					Integr=0;							//���� ���������������� ����������� ������ ����, �� ������������ ������������ ����� 0.		
				}									
				Differ = (pidError - pidErrorOld)*Kd;	//����������� ���������������� ������������

				if (Differ > float(stepsPwm)){			//���������������� ������������ ��� �� �������������� 
					Differ = float(stepsPwm);			//�� ��������.
				}
				if (Differ < -float(stepsPwm)){
					Differ = -float(stepsPwm);
				}			
				pidErrorOld = pidError;					//����������� �������� ������ ��� ���������� ����������
				backVeriablePoint = VP;					//����������� �������� ����������� ��� ���������� ����������	
				out = Proposh + Integr - Differ;		//����������� �������� �������� ����������
				if (out > float(stepsPwm)){				
					out = float(stepsPwm);				//��� ��� �� �������������� �� ��������.
				}
				if (out < 0){
					out = 0;							//���� �������� �������� ������ 0							
				}										//�������� �������� ����� 0
			}
			else {										
				out = 0;								
				Proposh = 0;							//���� �������� �������� ������� ����������� �� ����������
				Integr = 0;								//��� ��������� ��������� ����� ���������.
				Differ = 0;
			}
		}
		pwm();											//����������� �������� ������ ���
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
