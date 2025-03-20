/* mainwindow.ccp
 * Создан: 21.04.2024 09:15:22
 * Нижегородский Государственный технический университет им. Р.Е.Алексеева
 * группа 22-ВМв Хаченков О.И.
 * Исполнительный файл главного окна программы.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
    hid = new HIDLibrary<dataexchange_t>(0x16C0, 0x05DF);                                           // Создаем экземпляр класса с типом нашей структуры
    if (!hid->initLibUsb()){                                                                        //Инициализация библиотеки libusb-1.0
        QMessageBox msgBox;                                                                         //В случае неудачи, создается окно сообщения
        msgBox.setText("Библиотека libusb-1.0 не найдена. "                                         //с указанной надписью.
                       "Работа приложения невозможна.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        exit(-1);                                                                                   //Приложение закрывается
    }
    validatorI = new QIntValidator;                                                                 //Валидатор для проверки целых чисел
    validatorD = new QDoubleValidator;                                                              //Валидатор для проверки дробных чисел
    pdata = new dataexchange_t;                                                                     //Структура для хранения настроек устройства
    pollingTimer = new QTimer(this);                                                                //Таймер периодического опроса устройства
    connect(pollingTimer, SIGNAL(timeout()), this, SLOT(updateFormItems()));                        //Связь сигнала таймера со слотом обработки
    pollingTimer->start(1000);                                                                      //Запуск таймера, на срабатывание каждую секунду
    mainWindowResize();                                                                             //Установка необходимых размеров главного окна
}

MainWindow::~MainWindow(){   
    delete pollingTimer;                                                                            //Удалить таймер
    delete hid;                                                                                     //Удалить объект устройства
    delete pdata;                                                                                   //Удалить структуру данных для приемаи передачи
    delete validatorI;                                                                              //Удалить валидатор целых чисел
    delete validatorD;                                                                              //Удалить валидатор дробных чисел
    delete ui;                                                                                      //Удалить окно
}
void MainWindow::updateFormItems(){
    if (connectDevice()){
        if (hid->receiveData(pdata)){
//***!!!!*** Обязательно сначала sendDataToForm() затем setEnabledFormItem() ***!!!***
            sendDataToForm();                                                                       // Заполнить значениями элементы формы.
            setEnabledFormItem();                                                                   // Сделать элементы формы доступными для редактирования
        }
   }
    else{
       setDisabledFormItem();                                                                       // Сделать элементы формы не доступными для редактирования
    }
}
void MainWindow::sendDataToForm(){
    ui->lineEditCurrentTemperature->setText(QString::number(pdata->currentTemperature));            //Заполнение поля "Измеренная температура"
    if ((!ui->lineEditSetPointTemperature->hasFocus()) || !enadledFormItems){                       //Выполнить заполнение поля "Уставка по температуре",
        ui->lineEditSetPointTemperature->setText(QString::number(pdata->setPointTemperature));}     //если оно еще пока недоступно или на нем нет фокуса ввода.
    if ((!ui->lineEditMaxSetPoint->hasFocus()) || !enadledFormItems){                              //Выполнить заполнение поля "Верхняя граница уставки",
        ui->lineEditMaxSetPoint->setText(QString::number(pdata->maxSetPoint));}                     //если оно еще пока недоступно или на нем нет фокуса ввода.
    if ((!ui->lineEditMinSetPoint->hasFocus()) || !enadledFormItems){                                //Выполнить заполнение поля "Нижняя граница уставки",
        ui->lineEditMinSetPoint->setText(QString::number(pdata->minSetPoint));}                     //если оно еще пока недоступно или на нем нет фокуса ввода.
    ui->lineEditPidError->setText(QString::number(pdata->pidError));                                //Заполнение поля "Ошибка ПИД регулятора"
    ui->lineEditProposh->setText(QString::number(pdata->Proposh));                                  //Заполнение поля "Пропорциональная составляющая"
    ui->lineEditIntegr->setText(QString::number(pdata->Integr));                                    //Заполнение поля "Интегральная составляющая"
    ui->lineEditDiffer->setText(QString::number(pdata->Differ));                                    //Заполнение поля "Дифференциальная составляющая"
    ui->lineEditOut->setText(QString::number(pdata->out));                                          //Заполнение поля "Выходная величина регулятора"
    if ((!ui->lineEditBrightness->hasFocus()) || !enadledFormItems){                                //Выполнить заполнение поля "Яркость дисплея",
        ui->lineEditBrightness->setText(QString::number(pdata->brightness));}                       //если оно еще пока недоступно или на нем нет фокуса ввода.
    ui->checkBoxBacklightOnOff->setChecked(pdata->backLightEnable);                                 //Заполнение поля ""
    if(pdata->backLightEnable){                                                                     //Если отключение подсветки разрешено
        ui->lineEditBacklightTime->setEnabled(pdata->backLightEnable);                              //сделать поле ввода значения доступным,
        if ((!ui->lineEditBacklightTime->hasFocus()) || !enadledFormItems){                         //и установить значение, если поле еще не доступно,
            ui->lineEditBacklightTime->setText(QString::number(pdata->backLightTime));}             //или на нем нет фокуса ввода.
    }
    if ((!ui->lineEditKp->hasFocus()) || !enadledFormItems){                                        //Выполнить заполнение поля "Пропорциональный коэффициент",
        ui->lineEditKp->setText(QString::number(pdata->Kp));}                                       //если оно еще пока недоступно или на нем нет фокуса ввода.
    if ((!ui->lineEditKi->hasFocus()) || !enadledFormItems){                                        //Выполнить заполнение поля "Интегральный коэффициент",
        ui->lineEditKi->setText(QString::number(pdata->Ki));}                                       //если оно еще пока недоступно или на нем нет фокуса ввода.
    if ((!ui->lineEditKd->hasFocus()) || !enadledFormItems){                                        //Выполнить заполнение поля "Дифференциальный коэффициент",
        ui->lineEditKd->setText(QString::number(pdata->Kd));}                                       //если оно еще пока недоступно или на нем нет фокуса ввода.
    if ((!ui->lineEditIDtime->hasFocus()) || !enadledFormItems){                                    //Выполнить заполнение поля "Время интегрирования/дифференцирования",
        ui->lineEditIDtime->setText(QString::number(pdata->idTime));}                               //если оно еще пока недоступно или на нем нет фокуса ввода.
    if ((!ui->lineEditminPwmTime->hasFocus()) || !enadledFormItems){                                //Выполнить заполнение поля "Ступень ШИМ",
        ui->lineEditminPwmTime->setText(QString::number(pdata->minPwmTime));}                       //если оно еще пока недоступно или на нем нет фокуса ввода.
    if ((!ui->lineEditStepsPWM->hasFocus()) || !enadledFormItems){                                  //Выполнить заполнение поля "Количество ступеней ШИМ на период",
        ui->lineEditStepsPWM->setText(QString::number(pdata->stepsPWM));}                           //если оно еще пока недоступно или на нем нет фокуса ввода.
}
void MainWindow::setEnabledFormItem(){
    ui->lineEditCurrentTemperature->setEnabled(true);                                               //Сделать доступным поле "Измеренная температура"
    ui->lineEditSetPointTemperature->setEnabled(true);                                              //Сделать доступным поле "Уставка по температуре"
    ui->lineEditMaxSetPoint->setEnabled(true);                                                      //Сделать доступным поле "Верхняя граница уставки"
    ui->lineEditMinSetPoint->setEnabled(true);                                                      //Сделать доступным поле "Нижняя граница уставки"
    ui->lineEditPidError->setEnabled(true);                                                         //Сделать доступным поле "Ошибка ПИД регулятора"
    ui->lineEditProposh->setEnabled(true);                                                          //Сделать доступным поле "Пропорциональная составляющая"
    ui->lineEditIntegr->setEnabled(true);                                                           //Сделать доступным поле "Интегральная составляющая"
    ui->lineEditDiffer->setEnabled(true);                                                           //Сделать доступным поле "Дифференциальная составляющая"
    ui->lineEditOut->setEnabled(true);                                                              //Сделать доступным поле "Выходная величина регулятора"
    ui->lineEditBrightness->setEnabled(true);                                                       //Сделать доступным поле "Яркость дисплея"
    ui->checkBoxBacklightOnOff->setEnabled(true);                                                   //Сделать доступным флажок "Разрешения отключения подсветки"
    ui->checkBoxBacklightOnOff->setChecked(pdata->backLightEnable);                                 //установить его состояние в соответствии с текущими настройками.
    ui->lineEditKp->setEnabled(true);                                                               //Сделать доступным поле "Пропорциональный коэффициент"
    ui->lineEditKi->setEnabled(true);                                                               //Сделать доступным поле "Интегральный коэффициент"
    ui->lineEditKd->setEnabled(true);                                                               //Сделать доступным поле "Дифференциальный коэффициент"
    ui->lineEditIDtime->setEnabled(true);                                                           //Сделать доступным поле "Время интегрировани/дифференцирования"
    ui->lineEditminPwmTime->setEnabled(true);                                                       //Сделать доступным поле "Степень ШИМ"
    ui->lineEditStepsPWM->setEnabled(true);                                                         //Сделать доступным поле "Количество ступеней ШИМ на период"
    enadledFormItems = true;
}
void MainWindow::setDisabledFormItem(){
    ui->lineEditCurrentTemperature->clear();
    ui->lineEditCurrentTemperature->setDisabled(true);
    ui->lineEditSetPointTemperature->clear();
    ui->lineEditSetPointTemperature->setDisabled(true);
    ui->lineEditMaxSetPoint->clear();
    ui->lineEditMaxSetPoint->setDisabled(true);
    ui->lineEditMinSetPoint->clear();
    ui->lineEditMinSetPoint->setDisabled(true);
    ui->lineEditPidError->clear();
    ui->lineEditPidError->setDisabled(true);
    ui->lineEditProposh->clear();
    ui->lineEditProposh->setDisabled(true);
    ui->lineEditIntegr->clear();
    ui->lineEditIntegr->setDisabled(true);
    ui->lineEditDiffer->clear();
    ui->lineEditDiffer->setDisabled(true);
    ui->lineEditOut->clear();
    ui->lineEditOut->setDisabled(true);
    ui->lineEditBrightness->clear();
    ui->lineEditBrightness->setDisabled(true);
    ui->checkBoxBacklightOnOff->setChecked(false);
    ui->checkBoxBacklightOnOff->setDisabled(true);
    ui->lineEditBacklightTime->clear();
    ui->lineEditBacklightTime->setDisabled(true);
    ui->lineEditKp->clear();
    ui->lineEditKp->setDisabled(true);
    ui->lineEditKi->clear();
    ui->lineEditKi->setDisabled(true);
    ui->lineEditKd->clear();
    ui->lineEditKd->setDisabled(true);
    ui->lineEditIDtime->clear();
    ui->lineEditIDtime->setDisabled(true);
    ui->lineEditminPwmTime->clear();
    ui->lineEditminPwmTime->setDisabled(true);
    ui->lineEditStepsPWM->clear();
    ui->lineEditStepsPWM->setDisabled(true);
    enadledFormItems = false;
}

bool MainWindow::exchangeData(){
    if (connectDevice()){
        if (hid->sendData(pdata)){
            if (hid->receiveData(pdata)){
                sendDataToForm();
                return true;
            }
        }
    }
    return false;
}

bool MainWindow::connectDevice(){
    int n = hid->enumerateHIDDevices();
    for (int i=0; i<n; i++){
       if (hid->connect(i)){
           if (    hid->getConnectedDeviceVendorName() == vendorName
                && hid->getConnectedDeviceProductName() == productName ){
              return true;
           }
       }
    }
    return false;
}

void MainWindow::mainWindowResize(){
    int widthLeftBlock = std::max(std::max(ui->gBoxMeasureValue->sizeHint().width(),//Нужно определить наибольший рекомендуемый размер левых groupBox
                                             ui->gBoxSetPoint->sizeHint().width()),
                                    std::max(ui->gBoxBacklight->sizeHint().width(),
                                             ui->gBoxPWM->sizeHint().width()));
    ui->gBoxMeasureValue->setMinimumWidth(widthLeftBlock);                          //Всем левым groupBox установить максимкальный и
    ui->gBoxMeasureValue->setMaximumWidth(widthLeftBlock);                          //минимальный размеры по ширине, как у самого широкого,
    ui->gBoxSetPoint->setMinimumWidth(widthLeftBlock);                              //у которого наибольший рекомендуемый размер.
    ui->gBoxSetPoint->setMaximumWidth(widthLeftBlock);
    ui->gBoxBacklight->setMinimumWidth(widthLeftBlock);
    ui->gBoxBacklight->setMaximumWidth(widthLeftBlock);
    ui->gBoxPWM->setMinimumWidth(widthLeftBlock);
    ui->gBoxPWM->setMaximumWidth(widthLeftBlock);
    int widthRightBlock = std::max(ui->gBoxCarentPidValues->sizeHint().width(),     //Аналогично нужно определить наибольший рекомендуемый
                                   ui->gBoxPidSettings->sizeHint().width());        //размер правых groupBox
    ui->gBoxCarentPidValues->setMinimumWidth(widthRightBlock);                      //Всем правым groupBox установить максимкальный и
    ui->gBoxCarentPidValues->setMaximumWidth(widthRightBlock);                      //минимальный размеры по ширине, как у самого широкого,
    ui->gBoxPidSettings->setMinimumWidth(widthRightBlock);                          //у которого наибольший рекомендуемый размер.
    ui->gBoxPidSettings->setMaximumWidth(widthRightBlock);
    this->setMinimumWidth(this->sizeHint().width());                                //В качестве минимального и максимального размера по ширине,
    this->setMaximumWidth(this->sizeHint().width());                                //для главного окна применяется рекомендованная величина
    this->setMinimumHeight(this->sizeHint().height());                              //В качестве минимального и максимального размера по высоте,
    this->setMaximumHeight(this->sizeHint().height());                              //для главного окна применяется рекомендованная величина
}

//*************** Разрешение / запрет выключения подсветки ***************//
void MainWindow::on_checkBoxBacklightOnOff_clicked(bool checked){
    if (checked){
        ui->lineEditBacklightTime->setEnabled(true);
        pdata->backLightEnable = true;
    }
    else{
        ui->lineEditBacklightTime->setDisabled(true);
        ui->lineEditBacklightTime->clear();
        pdata->backLightEnable = false;
    }
    exchangeData();
}
//************ Конец разрешение / запрет выключения подсветки ************//

//********** Обработка вводимых данных в поле яркость подсветки **********//
void MainWindow::on_lineEditBrightness_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditBrightness->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditBrightness->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditBrightness_editingFinished(){
    if (ui->lineEditBrightness->hasFocus()){
        ui->lineEditBrightness->clearFocus();
        return;
    }
    bool flagOk = false;
    uint8_t m;
    QString s = ui->lineEditBrightness->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_BRIGHTNESS) n = MAX_BRIGHTNESS;
        if (n < MIN_BRIGHTNESS) n = MIN_BRIGHTNESS;
        m = uint8_t(n);
        pdata->brightness = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditBrightness->setText(QString::number(pdata->brightness));
    }
    ui->lineEditBrightness->setStyleSheet("color: rgb(0, 0, 0)");
}
//******* Конец обработка вводимых данных в поле яркость подсветки *******//


//******** Обработка вводимых данных в поле длительности подсветки ********//
void MainWindow::on_lineEditBacklightTime_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(MIN_BAK_LIGHT_TIME, MAX_BAK_LIGHT_TIME);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditBacklightTime->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditBacklightTime->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditBacklightTime_editingFinished(){
    if(ui->lineEditBacklightTime->hasFocus()){
        ui->lineEditBacklightTime->clearFocus();
        return;
    }
    bool flagOk = false;
    uint16_t m;
    QString s = ui->lineEditBacklightTime->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_BAK_LIGHT_TIME)  n = MAX_BAK_LIGHT_TIME;
        if (n < MIN_BAK_LIGHT_TIME ) n = MIN_BAK_LIGHT_TIME;
        m = uint16_t(n);
        pdata->backLightTime = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditBacklightTime->setText(QString::number(pdata->backLightTime));
    }
    ui->lineEditBacklightTime->setStyleSheet("color: rgb(0, 0, 0)");
}
//***** Конец обработка вводимых данных в поле длительности подсветки *****//


//*** Обработка данных в поле поле пропорциональный коэффициент Kp ***//
void MainWindow::on_lineEditKp_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    s = s.replace('.',',');
    validatorD->setRange(MIN_KP, MAX_KP,-1);
    if (QValidator::Acceptable == validatorD->validate(s, npos)){
        ui->lineEditKp->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditKp->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditKp_editingFinished(){
    if (ui->lineEditKp->hasFocus()){
        ui->lineEditKp->clearFocus();
        return;
    }
    bool flagOk = false;
    float m;
    QString s = ui->lineEditKp->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_KP) n = MAX_KP;
        if (n < MIN_KP) n = MIN_KP;
        m = float(n);
        pdata->Kp = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditKp->setText(QString::number(pdata->Kp));
    }
    ui->lineEditKp->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработки данных в поле поле пропорциональный коэффициент Kp ***//


//*** Обработка данных в поле  интегральный коэффициент Ki ***//
void MainWindow::on_lineEditKi_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    s = s.replace('.',',');
    validatorD->setRange(MIN_KI, MAX_KI,-1);
    if (QValidator::Acceptable == validatorD->validate(s, npos)){
        ui->lineEditKi->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditKi->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditKi_editingFinished(){
    if (ui->lineEditKi->hasFocus()){
        ui->lineEditKi->clearFocus();
        return;
    }
    bool flagOk = false;
    float m;
    QString s = ui->lineEditKi->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_KI) n = MAX_KI;
        if (n < MIN_KI) n = MIN_KI;
        m = float(n);
        pdata->Ki = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditKi->setText(QString::number(pdata->Ki));
    }
    ui->lineEditKi->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработки данных в поле интегральный коэффициент Ki ***//


//*** Обработка данных в поле диференциальный коэффициент Kd ***//
void MainWindow::on_lineEditKd_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    s = s.replace('.',',');
    validatorD->setRange(MIN_KD, MAX_KD,-1);
    if (QValidator::Acceptable == validatorD->validate(s, npos)){
        ui->lineEditKd->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditKd->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditKd_editingFinished(){
    if (ui->lineEditKd->hasFocus()){
        ui->lineEditKd->clearFocus();
        return;
    }
    bool flagOk = false;
    float m;
    QString s = ui->lineEditKd->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_KD )n = MAX_KD;
        if (n < MIN_KD )n = MIN_KD;
        m = float(n);
        pdata->Kd = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditKd->setText(QString::number(pdata->Kd));
    }
    ui->lineEditKd->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработки данных в поле диференциальный коэффициент Kd ***//


//*** Обработка данных в поле время индегрирования/дифференцирования ***//
void MainWindow::on_lineEditIDtime_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(MIN_ID_TIME, MAX_ID_TIME);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditIDtime->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditIDtime->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditIDtime_editingFinished(){
    if (ui->lineEditIDtime->hasFocus()){
        ui->lineEditIDtime->clearFocus();
        return;
    }
    bool flagOk = false;
    uint16_t m;
    QString s = ui->lineEditIDtime->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_ID_TIME )n = MAX_ID_TIME;
        if (n < MIN_ID_TIME )n = MIN_ID_TIME;
        m = uint16_t(n);
        pdata->idTime = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditIDtime->setText(QString::number(pdata->idTime));
    }
    ui->lineEditIDtime->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработка данных в поле время индегрирования/дифференцирования ***//


//*** Обработка данных в поле длительность наименьшего импульса ШИМ ***//
void MainWindow::on_lineEditminPwmTime_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(MIN_MIN_PWM_TIME, MAX_MIN_PWM_TIME);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditminPwmTime->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditminPwmTime->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditminPwmTime_editingFinished(){
    if (ui->lineEditminPwmTime->hasFocus()){
        ui->lineEditminPwmTime->clearFocus();
        return;
    }
    bool flagOk = false;
    uint16_t m;
    QString s = ui->lineEditminPwmTime->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_MIN_PWM_TIME) n = MAX_MIN_PWM_TIME;
        if (n < MIN_MIN_PWM_TIME) n = MIN_MIN_PWM_TIME;
        m = uint16_t(n);
        pdata->minPwmTime = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditminPwmTime->setText(QString::number(pdata->minPwmTime));
    }
    ui->lineEditminPwmTime->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработки данных в поле длительность наименьшего импульса ШИМ ***//


//*** Обработка данных в поле количество импульсов ШИМ на период ***//
void MainWindow::on_lineEditStepsPWM_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(MIN_STEPS_PWM, MAX_STEPS_PWM);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditStepsPWM->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditStepsPWM->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditStepsPWM_editingFinished(){
    if (ui->lineEditStepsPWM->hasFocus()){
        ui->lineEditStepsPWM->clearFocus();
        return;
    }
    bool flagOk = false;
    uint16_t m;
    QString s = ui->lineEditStepsPWM->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_STEPS_PWM) n = MAX_STEPS_PWM;
        if (n < MIN_STEPS_PWM) n = MIN_STEPS_PWM;
        m = uint16_t(n);
        pdata->stepsPWM = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditStepsPWM->setText(QString::number(pdata->stepsPWM));
    }
    ui->lineEditStepsPWM->setStyleSheet("color: rgb(0, 0, 0)");
 }
//*** Конец обработки данных в поле количество импульсов ШИМ на период ***//

//*** Обработка данных в поле уставка по температуре ***//
void MainWindow::on_lineEditSetPointTemperature_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(pdata->minSetPoint, pdata->maxSetPoint);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditSetPointTemperature->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditSetPointTemperature->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditSetPointTemperature_editingFinished(){
    if (ui->lineEditSetPointTemperature->hasFocus()){
        ui->lineEditSetPointTemperature->clearFocus();
        return;
    }
    bool flagOk = false;
    uint16_t m;
    QString s = ui->lineEditSetPointTemperature->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > pdata->maxSetPoint) n = pdata->maxSetPoint;
        if (n < pdata->minSetPoint) n = pdata->minSetPoint;
        m = uint16_t(n);
        pdata->setPointTemperature = m; //Запись значения в таблицу
        exchangeData();
    }
    else{
        ui->lineEditSetPointTemperature->setText(QString::number(pdata->setPointTemperature));
    }
    ui->lineEditSetPointTemperature->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработки данных в поле уставка по температуре ***//

//*** Обработка данных в поле верхняя граница уставки ***//
void MainWindow::on_lineEditMaxSetPoint_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(pdata->minSetPoint, MAX_SET_POINT);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditMaxSetPoint->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditMaxSetPoint->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditMaxSetPoint_editingFinished(){
    if (ui->lineEditMaxSetPoint->hasFocus()){
        ui->lineEditMaxSetPoint->clearFocus();
        return;
    }
    bool flagOk = false;
    uint16_t m;
    QString s = ui->lineEditMaxSetPoint->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > MAX_SET_POINT) n = MAX_SET_POINT;
        if (n < pdata->minSetPoint) n = pdata->minSetPoint;
        m = uint16_t(n);
        pdata->maxSetPoint = m;                         //Запись значения в таблицу
        if (pdata->setPointTemperature > m){            //Если текущая уставка выходит за пределы
            pdata->setPointTemperature = m;             //максимальной уставки, ее нужно скоректировать.
        }
        exchangeData();
    }
    else{
        ui->lineEditMaxSetPoint->setText(QString::number(pdata->maxSetPoint));
    }
    ui->lineEditMaxSetPoint->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработки данных в поле верхняя граница уставки ***//

//*** Обработка данных в поле нижняя граница уставки ***//
void MainWindow::on_lineEditMinSetPoint_textEdited(const QString &arg1){
    int npos;
    QString s = arg1;
    validatorI->setRange(MIN_SET_POINT, pdata->maxSetPoint);
    if (QValidator::Acceptable == validatorI->validate(s, npos)){
        ui->lineEditMinSetPoint->setStyleSheet("color: rgb(35, 135, 45)");
    }
    else{
        ui->lineEditMinSetPoint->setStyleSheet("color: rgb(255, 0, 0)");
    }
}
void MainWindow::on_lineEditMinSetPoint_editingFinished(){
    if (ui->lineEditMinSetPoint->hasFocus()){
        ui->lineEditMinSetPoint->clearFocus();
        return;
    }
    bool flagOk = false;
    uint16_t m;
    QString s = ui->lineEditMinSetPoint->text().replace(',', '.');
    double n = s.toDouble(&flagOk);
    if (flagOk){
        if (n > pdata->maxSetPoint) n = pdata->maxSetPoint;
        if (n < MIN_SET_POINT) n = MIN_SET_POINT;
        m = uint16_t(n);
        pdata->minSetPoint = m;                         //Запись значения в таблицу
        if (pdata->setPointTemperature < m){            //Если текущая уставка выходит за пределы
            pdata->setPointTemperature = m;             //максимальной уставки, ее нужно скоректировать.
        }
        exchangeData();
    }
    else{
        ui->lineEditMinSetPoint->setText(QString::number(pdata->maxSetPoint));
    }
    ui->lineEditMinSetPoint->setStyleSheet("color: rgb(0, 0, 0)");
}
//*** Конец обработки данных в поле нижняя граница уставки ***//






