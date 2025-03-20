/* oled_ssd1306_128X64.cpp
 * Создан: 09.03.2024 22:25:28
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Исполнительный файл драйвера oled 0,96" дисплея, построенного на
 * контроллере ssd1306. Размер экрана 128x64 пикселя.
 * Управляется дисплей по четырехпроводной шине SPI.
 */ 
#include "oled_ssd1306_128X64.h"

void Displey_ssd1306_128x64::init(SPI_UNIT & spi){
	CS0_DDR |= 1 << CS0_PIN;							//Настройка порта CS на вывод
	select();											//Выбор устройства как активного
	RESET_DDR |= 1 << RESET_PIN;						//Настройка порта RESET на вывод
	reset();											//Сброс контроллера дисплея
	DATA_COMMAND_DDR |= 1<<DATA_COMMAND_PIN;			//Настройка порта DC на вывод
	command();											//Установка режима передачи команд	
	this->spi_pointer = & spi;							//Сохранение ссылки на объект spi
	const uint8_t settings[]={
									0xA8,				//Команда установки коэффициента мультиплексирования
									0x3F,				//Значение коэффициента 3F

									0xD3,				//Команда установки вертикального смещения
									0x00,				//Установка начальной строки

									0xA1,				//Установка адреса столбца 127    

									0xC8,				//Установка режима сканирования: обратный режим

									0xDA,				//Конфигурация последовательных выводов COM
									0x12,				//Конфигурация
	
									0x81,				//Команда установки контрастности дисплея
									0x3F,				//Контрастность дисплея
												
									0xA4,				//Возобновить отображение содержимого RAM

									0xA6,				//Выбор режима отображение Нормальный/Инверсный: нормальный

									0xD5,				//Установка коэффициента деления осцилятора контроллера дисплея
									0x80,				//Коэффициент деления и частота генератора

									0x20,				//Установка адресации памяти
									0x00,				//0x00 -  горизонтальная 0x01 - вертикальная 0x02 - постраничная

									0x8D,				//Команда активация Pump регистров
									0x14,				//Коэффициент деления и частота генератора

									0xAF				//Команда включения дисплея
							};
	for(int i =0; i < sizeof(settings); i++){
		spi_pointer->transmissionByte(settings[i]);		//Отправка команды дисплею
		_delay_us(EXICUTION_DELAY);						//Задержка для исполнения команды дисплеем
	}
	flagBackLightOn = true;								//Объект сохраняет включенное состояние дисплея
	unselect();
}
bool Displey_ssd1306_128x64::getBacklightStatus(){ 
	return flagBackLightOn;								//Метод возвращает текущее состояние подсветки
}
void Displey_ssd1306_128x64::onDispley(){
	select();
	command();
	spi_pointer->transmissionByte(0xAF);				//Команда включения
	flagBackLightOn = true;								//Объект сохраняет включенное состояние дисплея
	_delay_us(EXICUTION_DELAY);
	unselect();
}
void Displey_ssd1306_128x64::offDispley(){
	select();
	command();
	spi_pointer->transmissionByte(0xAE);				//Команда отключения
	flagBackLightOn = false;							//Объект сохраняет включенное состояние дисплея
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
	spi_pointer->transmissionByte(0x81);				//Команда установки
	_delay_us(EXICUTION_DELAY);
	spi_pointer->transmissionByte(brightness);			//Значение коэффициента 3F
	_delay_us(EXICUTION_DELAY);
	unselect();
}
void Displey_ssd1306_128x64::data(){
	if ( (DATA_COMMAND_PORT & (1 << DATA_COMMAND_PIN) ) == 0 ){		
		_delay_us(TRIGGER_DELAY);									//Проверка состояния выхода,		
		DATA_COMMAND_PORT |= 1<<DATA_COMMAND_PIN;					// если передача команд то поменять			
		_delay_us(TRIGGER_DELAY);									//на передачу данных.
	}
}
void Displey_ssd1306_128x64::command(){
	if ( (DATA_COMMAND_PORT & (1<<DATA_COMMAND_PIN) ) != 0 ){		
		_delay_us(TRIGGER_DELAY);									//Проверка состояния выхода,
		DATA_COMMAND_PORT &= ~(1<<DATA_COMMAND_PIN);				//если передача данных то поменять 
		_delay_us(TRIGGER_DELAY);									//на передачу команд.
	}
}
void Displey_ssd1306_128x64::select(){
	_delay_us(TRIGGER_DELAY);
	CS0_PORT &= ~(1<<CS0_PIN);										//Установка режима выбора дисплея на шине SPI
	_delay_us(TRIGGER_DELAY);
}
void Displey_ssd1306_128x64::unselect(){
	_delay_us(TRIGGER_DELAY);
	CS0_PORT |= 1<<CS0_PIN;											//Отмена режима выбора дисплея на шине SPI
	_delay_us(TRIGGER_DELAY);
}
void Displey_ssd1306_128x64::reset(){
	_delay_us(TRIGGER_DELAY);
	RESET_PORT &= ~(1<<RESET_PIN);									//Подача команды на сброс дисплея
	_delay_us(EXICUTION_DELAY);
	RESET_PORT |= 1<<RESET_PIN;										//Отмена команды сброса дисплея
}
void Displey_ssd1306_128x64::cleare(){
	select();														//Выбор дисплея на шине SPI
	data();															//Установка режима передачи данных
	for (int i=0; i<128*8; i++){
		spi_pointer->transmissionByte(0x00);						//Отправка пустых данных
		_delay_us(EXICUTION_DELAY);									//для печати на дисплей
	}
	unselect();														//Отмена выбора дисплея на шине SPI
}
void Displey_ssd1306_128x64::printLogo(){
	select();														//Выбор дисплея на шине SPI
	command();														//Установка режима передачи команд
	setFrame(0x00, 0x02, 0x00, 0x47);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for (int i= 0; i<144; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&logo[i]));
	}
	unselect();														//Отмена выбора дисплея на шине SPI
}
void Displey_ssd1306_128x64::printUSBlable(){
	select();														//Выбор дисплея на шине SPI
	command();														//Установка режима передачи команд
	setFrame(0x00, 0x02, 0x50, 0x7F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for (int i= 0; i<96; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&usb_logo[i]));
	}
	unselect();														//Отмена выбора дисплея на шине SPI
}
void Displey_ssd1306_128x64::deleteUSBlable(){
	select();														//Выбор дисплея на шине SPI
	command();														//Установка режима передачи команд
	setFrame(0x00, 0x02, 0x50, 0x7F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for (int i= 0; i<96; i++){
		spi_pointer->transmissionByte(0x00);
	}
	unselect();														//Отмена выбора дисплея на шине SPI
}

void Displey_ssd1306_128x64::printSP(uint16_t n){
	select();														//Выбор дисплея на шине SPI
	uint8_t dig_0, dig_1, dig_2, dig_3;								//Здесь происходит разбивка числа на разряды
	dig_3 = n / 1000;
	dig_2 = (n % 1000)/100;
	dig_1 = (n % 100)/10;
	dig_0 = n % 10;
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x00, 0x0F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0C*48]));//Вывод на дисплей буквы S
	}
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x10, 0x1F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0B*48]));//Вывод на дисплей буквы P
	}
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x20, 0x2F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	if (dig_3==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);					//Если разряд пуст, очистка области 
		}															//печати под первую цифру
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_3*48])); //Печать четвертой цифры
		}
	}
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x30, 0x3F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	if (dig_3 == 0 && dig_2==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);					//Если разряд пуст, и предыдущий тоже пуст, 
		}															//очистка области печати под вторую цифру
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_2*48])); //Печать второй цифры
		}
	}
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x40, 0x4F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	if (dig_3==0 && dig_2==0 && dig_1==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);					//Если разряд пуст, и предыдущие тоже пусты, 
		}															//очистка области печати под треть цифру
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_1*48])); //Печать третьей цифры
		}
	}
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x50, 0x5F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_0*48])); //четвертая цифра печатается всегда
	}
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x60, 0x6F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0E*48]));//Печать значка градус
	}
	command();														//Установка режима передачи команд
	setFrame(0x02, 0x04, 0x70, 0x7F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0D*48])); //Печать С
	}
	unselect();														//Отмена выбора дисплея на шине SPI
}
void Displey_ssd1306_128x64::printVPerr(){
	select();														//Выбор дисплея на шине SPI
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x00, 0x0F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0A*48]));//Печать V
	}
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x10, 0x1F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0B*48]));//Печать P
	}
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x20, 0x2F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//Печать тире
	}
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x30, 0x3F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//Печать тире
	}
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x40, 0x4F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//Печать тире
	}
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x50, 0x5F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0F*48]));//Печать тире
	}
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x60, 0x6F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0E*48]));//Печать значка градусов
	}
	command();														//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x70, 0x7F);								//Установка области для вывода данных
	data();															//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0D*48]));//Печать С
	}
	unselect();														//Отмена выбора дисплея на шине SPI
}
void Displey_ssd1306_128x64::printVP(uint16_t n){
	select();															//Выбор дисплея на шине SPI
	uint8_t dig_0, dig_1, dig_2, dig_3;
	dig_3 = n / 1000;													//Разбивка числа на разряды
	dig_2 = (n % 1000)/100;
	dig_1 = (n % 100)/10;
	dig_0 = n % 10;
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x00, 0x0F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0A*48]));	//Печать V
	}
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x10, 0x1F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0B*48]));	//Печать P
	}
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x20, 0x2F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	if (dig_3==0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);						//Процедура вывода первой цифры аналогично  
		}																//тому как и в printSP()
	}
	else{
			for(int i = 0; i < 48; i++){
				spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_3*48]));
			}
	}
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x30, 0x3F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	if (dig_3 == 0 && dig_2 == 0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);						//Процедура вывода второй цифры аналогично 
		}																//тому как и в printSP()
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_2*48]));
		}
	}
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x40, 0x4F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	if (dig_3 == 0 && dig_2 == 0 && dig_1 == 0){
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(0x00);						//Процедура вывода третьей цифры аналогично 
		}																//тому как и в printSP()
	}
	else{
		for(int i = 0; i < 48; i++){
			spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_1*48]));
		}
	}		
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x50, 0x5F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+dig_0*48]));	//Процедура вывода четвертой цифры аналогично 
	}																	//тому как и в printSP()
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x60, 0x6F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0E*48]));	//Печать значка градусов
	}
	command();															//Установка режима передачи команд
	setFrame(0x05, 0x07, 0x70, 0x7F);									//Установка области для вывода данных
	data();																//Установка режима передачи данных
	for(int i = 0; i < 48; i++){
		spi_pointer->transmissionByte(pgm_read_byte(&p[i+0x0D*48]));    //Печать символа C
	}
	unselect();
}
void Displey_ssd1306_128x64::setFrame(uint8_t startRow, uint8_t endRow, uint8_t startColom, uint8_t endColom){
	spi_pointer->transmissionByte(0x22);		//Установить начальную и конечную строки
	spi_pointer->transmissionByte(startRow);	//Установить начальную строку
	spi_pointer->transmissionByte(endRow);		//Установить конечную строку
	spi_pointer->transmissionByte(0x21);		//Установить начальный и конечный столбци
	spi_pointer->transmissionByte(startColom);	//Установить начальный столбец
	spi_pointer->transmissionByte(endColom);	//Установит конечный столбец
}
