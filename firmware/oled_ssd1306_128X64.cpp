/* oled_ssd1306_128X64.cpp
 * ������: 09.03.2024 22:25:28
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * �������������� ���� �������� oled 0,96" �������, ������������ ��
 * ����������� ssd1306. ������ ������ 128x64 �������.
 * ����������� ������� �� ���������������� ���� SPI.
 */ 
#include "oled_ssd1306_128X64.h"

void Displey_ssd1306_128x64::init(SPI_UNIT & spi){
	CS0_DDR |= 1 << CS0_PIN;							//��������� ����� CS �� �����
	select();											//����� ���������� ��� ���������
	RESET_DDR |= 1 << RESET_PIN;						//��������� ����� RESET �� �����
	reset();											//����� ����������� �������
	DATA_COMMAND_DDR |= 1<<DATA_COMMAND_PIN;			//��������� ����� DC �� �����
	command();											//��������� ������ �������� ������	
	this->spi_pointer = & spi;							//���������� ������ �� ������ spi
	const uint8_t settings[]={
									0xA8,				//������� ��������� ������������ �������������������
									0x3F,				//�������� ������������ 3F

									0xD3,				//������� ��������� ������������� ��������
									0x00,				//��������� ��������� ������

									0xA1,				//��������� ������ ������� 127    

									0xC8,				//��������� ������ ������������: �������� �����

									0xDA,				//������������ ���������������� ������� COM
									0x12,				//������������
	
									0x81,				//������� ��������� ������������� �������
									0x3F,				//������������� �������
												
									0xA4,				//����������� ����������� ����������� RAM

									0xA6,				//����� ������ ����������� ����������/���������: ����������

									0xD5,				//��������� ������������ ������� ���������� ����������� �������
									0x80,				//����������� ������� � ������� ����������

									0x20,				//��������� ��������� ������
									0x00,				//0x00 -  �������������� 0x01 - ������������ 0x02 - ������������

									0x8D,				//������� ��������� Pump ���������
									0x14,				//����������� ������� � ������� ����������

									0xAF				//������� ��������� �������
							};
	for(int i =0; i < sizeof(settings); i++){
		spi_pointer->transmissionByte(settings[i]);		//�������� ������� �������
		_delay_us(EXICUTION_DELAY);						//�������� ��� ���������� ������� ��������
	}
	flagBackLightOn = true;								//������ ��������� ���������� ��������� �������
	unselect();
}
bool Displey_ssd1306_128x64::getBacklightStatus(){ 
	return flagBackLightOn;								//����� ���������� ������� ��������� ���������
}
void Displey_ssd1306_128x64::onDispley(){
	select();
	command();
	spi_pointer->transmissionByte(0xAF);				//������� ���������
	flagBackLightOn = true;								//������ ��������� ���������� ��������� �������
	_delay_us(EXICUTION_DELAY);
	unselect();
}
void Displey_ssd1306_128x64::offDispley(){
	select();
	command();
	spi_pointer->transmissionByte(0xAE);				//������� ����������
	flagBackLightOn = false;							//������ ��������� ���������� ��������� �������
	_delay_us(EXICUTION_DELAY);
	unselect();
}
void Displey_ssd1306_128x64::setBrightness(uint8_t brightness){
	select();
	command();
	if (brightness > 100) brightness=100;
	uint16_t tmp = brightness;
	tmp = tmp * 255 / 100;
	brightness = tmp;
	spi_pointer->transmissionByte(0x81);				//������� ���������
	_delay_us(EXICUTION_DELAY);
	spi_pointer->transmissionByte(brightness);			//�������� ������������ 3F
	_delay_us(EXICUTION_DELAY);
	unselect();
}
void Displey_ssd1306_128x64::data(){
	if ( (DATA_COMMAND_PORT & (1 << DATA_COMMAND_PIN) ) == 0 ){		
		_delay_us(TRIGGER_DELAY);									//�������� ��������� ������,		
		DATA_COMMAND_PORT |= 1<<DATA_COMMAND_PIN;					// ���� �������� ������ �� ��������			
		_delay_us(TRIGGER_DELAY);									//�� �������� ������.
	}
}
void Displey_ssd1306_128x64::command(){
	if ( (DATA_COMMAND_PORT & (1<<DATA_COMMAND_PIN) ) != 0 ){		
		_delay_us(TRIGGER_DELAY);									//�������� ��������� ������,
		DATA_COMMAND_PORT &= ~(1<<DATA_COMMAND_PIN);				//���� �������� ������ �� �������� 
		_delay_us(TRIGGER_DELAY);									//�� �������� ������.
	}
}
void Displey_ssd1306_128x64::select(){
	_delay_us(TRIGGER_DELAY);
	CS0_PORT &= ~(1<<CS0_PIN);										//��������� ������ ������ ������� �� ���� SPI
	_delay_us(TRIGGER_DELAY);
}
void Displey_ssd1306_128x64::unselect(){
	_delay_us(TRIGGER_DELAY);
	CS0_PORT |= 1<<CS0_PIN;											//������ ������ ������ ������� �� ���� SPI
	_delay_us(TRIGGER_DELAY);
}
void Displey_ssd1306_128x64::reset(){
	_delay_us(TRIGGER_DELAY);
	RESET_PORT &= ~(1<<RESET_PIN);									//������ ������� �� ����� �������
	_delay_us(EXICUTION_DELAY);
	RESET_PORT |= 1<<RESET_PIN;										//������ ������� ������ �������
}
void Displey_ssd1306_128x64::cleare(){
	select();														//����� ������� �� ���� SPI
	data();															//��������� ������ �������� ������
	for (int i=0; i<128*8; i++){
		spi_pointer->transmissionByte(0x00);						//�������� ������ ������
		_delay_us(EXICUTION_DELAY);									//��� ������ �� �������
	}
	unselect();														//������ ������ ������� �� ���� SPI
}
void Displey_ssd1306_128x64::printLogo(){
	select();														//����� ������� �� ���� SPI
	command();														//��������� ������ �������� ������
	setFrame(0x00, 0x02, 0x00, 0x47);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for (int i= 0; i<144; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&logo[i]));
	}
	unselect();														//������ ������ ������� �� ���� SPI
}
void Displey_ssd1306_128x64::printUSBlable(){
	select();														//����� ������� �� ���� SPI
	command();														//��������� ������ �������� ������
	setFrame(0x00, 0x02, 0x50, 0x7F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for (int i= 0; i<96; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&usb_logo[i]));
	}
	unselect();														//������ ������ ������� �� ���� SPI
}
void Displey_ssd1306_128x64::deleteUSBlable(){
	select();														//����� ������� �� ���� SPI
	command();														//��������� ������ �������� ������
	setFrame(0x00, 0x02, 0x50, 0x7F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for (int i= 0; i<96; i++){
		spi_pointer->transmissionByte(0x00);
	}
	unselect();														//������ ������ ������� �� ���� SPI
}

void Displey_ssd1306_128x64::printSP(uint16_t n){
	select();														//����� ������� �� ���� SPI
	uint8_t dig_0, dig_1, dig_2, dig_3;								//����� ���������� �������� ����� �� �������
	dig_3 = n / 1000;
	dig_2 = (n % 1000)/100;
	dig_1 = (n % 100)/10;
	dig_0 = n % 10;
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x00, 0x0F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0C*48]));//����� �� ������� ����� S
	}
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x10, 0x1F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0B*48]));//����� �� ������� ����� P
	}
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x20, 0x2F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	if (dig_3==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);					//���� ������ ����, ������� ������� 
		}															//������ ��� ������ �����
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_3*48])); //������ ��������� �����
		}
	}
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x30, 0x3F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	if (dig_3 == 0 && dig_2==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);					//���� ������ ����, � ���������� ���� ����, 
		}															//������� ������� ������ ��� ������ �����
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_2*48])); //������ ������ �����
		}
	}
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x40, 0x4F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	if (dig_3==0 && dig_2==0 && dig_1==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);					//���� ������ ����, � ���������� ���� �����, 
		}															//������� ������� ������ ��� ����� �����
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_1*48])); //������ ������� �����
		}
	}
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x50, 0x5F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_0*48])); //��������� ����� ���������� ������
	}
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x60, 0x6F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0E*48]));//������ ������ ������
	}
	command();														//��������� ������ �������� ������
	setFrame(0x02, 0x04, 0x70, 0x7F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0D*48])); //������ �
	}
	unselect();														//������ ������ ������� �� ���� SPI
}
void Displey_ssd1306_128x64::printVPerr(){
	select();														//����� ������� �� ���� SPI
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x00, 0x0F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0A*48]));//������ V
	}
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x10, 0x1F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0B*48]));//������ P
	}
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x20, 0x2F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//������ ����
	}
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x30, 0x3F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//������ ����
	}
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x40, 0x4F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//������ ����
	}
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x50, 0x5F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//������ ����
	}
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x60, 0x6F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0E*48]));//������ ������ ��������
	}
	command();														//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x70, 0x7F);								//��������� ������� ��� ������ ������
	data();															//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0D*48]));//������ �
	}
	unselect();														//������ ������ ������� �� ���� SPI
}
void Displey_ssd1306_128x64::printVP(uint16_t n){
	select();															//����� ������� �� ���� SPI
	uint8_t dig_0, dig_1, dig_2, dig_3;
	dig_3 = n / 1000;													//�������� ����� �� �������
	dig_2 = (n % 1000)/100;
	dig_1 = (n % 100)/10;
	dig_0 = n % 10;
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x00, 0x0F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0A*48]));	//������ V
	}
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x10, 0x1F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0B*48]));	//������ P
	}
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x20, 0x2F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	if (dig_3==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);						//��������� ������ ������ ����� ����������  
		}																//���� ��� � � printSP()
	}
	else{
			for(int i = 0; i < 48; i++){
				spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_3*48]));
			}
	}
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x30, 0x3F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	if (dig_3 == 0 && dig_2 == 0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);						//��������� ������ ������ ����� ���������� 
		}																//���� ��� � � printSP()
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_2*48]));
		}
	}
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x40, 0x4F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	if (dig_3 == 0 && dig_2 == 0 && dig_1 == 0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);						//��������� ������ ������� ����� ���������� 
		}																//���� ��� � � printSP()
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_1*48]));
		}
	}		
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x50, 0x5F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_0*48]));	//��������� ������ ��������� ����� ���������� 
	}																	//���� ��� � � printSP()
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x60, 0x6F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0E*48]));	//������ ������ ��������
	}
	command();															//��������� ������ �������� ������
	setFrame(0x05, 0x07, 0x70, 0x7F);									//��������� ������� ��� ������ ������
	data();																//��������� ������ �������� ������
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0D*48]));    //������ ������� C
	}
	unselect();
}
void Displey_ssd1306_128x64::setFrame(uint8_t startRow, uint8_t endRow, uint8_t startColom, uint8_t endColom){
	spi_pointer->transmissionByte(0x22);		//���������� ��������� � �������� ������
	spi_pointer->transmissionByte(startRow);	//���������� ��������� ������
	spi_pointer->transmissionByte(endRow);		//���������� �������� ������
	spi_pointer->transmissionByte(0x21);		//���������� ��������� � �������� �������
	spi_pointer->transmissionByte(startColom);	//���������� ��������� �������
	spi_pointer->transmissionByte(endColom);	//��������� �������� �������
}
