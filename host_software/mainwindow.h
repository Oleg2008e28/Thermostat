/* mainwindow.h
 * Создан: 21.04.2024 09:15:22
 * Нижегородский Государственный технический университет им. Р.Е.Алексеева
 * группа 22-ВМв Хаченков О.И.
 * Заголовочный файл главного окна программы.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <string>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <algorithm>
#include "hidlibrary.h"

const uint8_t MAX_BRIGHTNESS = 100;                                         //Максимальная яркость дисплея, %
const uint8_t MIN_BRIGHTNESS = 10;                                          //Минимальная яркость дисплея, %
const uint16_t MAX_BAK_LIGHT_TIME = std::numeric_limits<uint16_t>::max();   //Максимальное время до выключения подсветки в секундах, сек
const uint16_t MIN_BAK_LIGHT_TIME = 10;                                     //Минимальное время до выключения подсветки в секунда, сек
const float MAX_KP = 1000;                                                  //Максимальное значение пропорционального коэффициента
const float MIN_KP = 0;                                                     //Минимальное значение пропорционального коэффициента
const float MAX_KI = 1000;                                                  //Максимальное значение интегрально коэффициента
const float MIN_KI = 0;                                                     //Минимальное значение интегрального коэффициента
const float MAX_KD = 1000;                                                  //Максимальное значение дифференциально коэффициента
const float MIN_KD = 0;                                                     //Минимальное значение дифференциально коэффициента
const uint16_t MAX_ID_TIME = std::numeric_limits<uint16_t>::max();          //Максимальное время интегрирования/дифференцирования, мсек.
const uint16_t MIN_ID_TIME = 100;                                           //Минимальное время интегрирования/дифференцирования, мсек.
const uint16_t MAX_MIN_PWM_TIME = std::numeric_limits<uint16_t>::max();     //Минимальная величина ШИМ, максимальное значение, мсек.
const uint16_t MIN_MIN_PWM_TIME = 10;                                       //Минимальная величина ШИМ, минимальное значение, мсек.
const uint16_t MAX_STEPS_PWM = std::numeric_limits<uint16_t>::max();        //Минимальное возможное количество ступеней ШИМ на период
const uint16_t MIN_STEPS_PWM = 10;                                          //Максимально возможное количество ступеней ШИМ на период
const uint16_t MAX_SET_POINT = 1000;                                        //Максимальная величина уставки
const uint16_t MIN_SET_POINT = 0;                                           //Минимальная величина уставки

struct dataexchange_t {					//Структура для передачи данных (лист настроек)
    bool backLightEnable;				//Разрешение отключения подсветки
    uint8_t brightness;					//Яркость подсветки
    uint16_t backLightTime;				//Время до отключения подсветки
    float Kp;							//Пропорциональный коэффициент ПИД  регулятора
    float Ki;							//Интегральный коэффициент ПИД  регулятора
    float Kd;							//Дифференциальный коэффициент ПИД  регулятора
    float pidError;						//Ошибка ПИД регулятора
    float Proposh;						//Пропорциональная составляющая ПИД регулятора
    float Integr;						//Интегральная составляющая ПИД регулятора
    float Differ;						//Дифференциальная составляющая ПИД регулятора
    float out;							//Выходной сигнал ШИМ
    float currentTemperature;			//Текущая температура
    uint16_t minSetPoint;				//Минимальная уставка
    uint16_t maxSetPoint;				//Максимальная уставка
    uint16_t setPointTemperature;		//Уставка температуры
    uint16_t idTime;					//Время интегнрирования/дифференцирования
    uint16_t minPwmTime;				//Минимальное время импульса ШИМ
    uint16_t stepsPWM;					//Количество ступеней шим
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_checkBoxBacklightOnOff_clicked(bool checked);               //Слот обработки нажания на флажок разрешения отключения подсветки
    void on_lineEditBrightness_textEdited(const QString &arg1);         //Слот валидации значения вводимого в поле "Яркость дисплея"
    void on_lineEditBrightness_editingFinished();                       //Слот обработки введенного значения в поле "Яркость дисплея"
    void on_lineEditBacklightTime_textEdited(const QString &arg1);      //Слот валидации значения вводимого в поле "Время отключения подсветки"
    void on_lineEditBacklightTime_editingFinished();                    //Слот обработки введенного значения в поле "Время отключения подсветки"
    void on_lineEditKp_textEdited(const QString &arg1);                 //Слот валидации значения вводимого в поле "Пропорциональный коэффициент"
    void on_lineEditKp_editingFinished();                               //Слот обработки введенного значения в поле "Пропорциональный коэффициент"
    void on_lineEditKi_textEdited(const QString &arg1);                 //Слот валидации значения вводимого в поле "Интегральный коэффициент"
    void on_lineEditKi_editingFinished();                               //Слот обработки введенного значения в поле "Интегральный коэффициент"
    void on_lineEditKd_textEdited(const QString &arg1);                 //Слот валидации значения вводимого в поле "Дифференциальный коэффициент"
    void on_lineEditKd_editingFinished();                               //Слот обработки введенного значения в поле "Дифференциальный коэффициент"
    void on_lineEditIDtime_textEdited(const QString &arg1);             //Слот валидации значения вводимого в поле "Время интегрировани/дифференцирования"
    void on_lineEditIDtime_editingFinished();                           //Слот обработки введенного значения в поле "Время интегрировани/дифференцирования"
    void on_lineEditminPwmTime_textEdited(const QString &arg1);         //Слот валидации значения вводимого в поле "Ступень шим"
    void on_lineEditminPwmTime_editingFinished();                       //Слот обработки введенного значения в поле "Ступень шим"
    void on_lineEditStepsPWM_textEdited(const QString &arg1);           //Слот валидации значения вводимого в поле "Количество ступеней ШИМ на период"
    void on_lineEditStepsPWM_editingFinished();                         //Слот обработки введенного значения в поле "Количество ступеней ШИМ на период"
    void on_lineEditSetPointTemperature_textEdited(const QString &arg1);//Слот валидации значения вводимого в поле "Уставка по температуре"
    void on_lineEditSetPointTemperature_editingFinished();              //Слот обработки введенного значения в поле "Уставка по температуре"
    void on_lineEditMaxSetPoint_textEdited(const QString &arg1);        //Слот валидации значения вводимого в поле "Верхняя граница уставки"
    void on_lineEditMaxSetPoint_editingFinished();                      //Слот обработки введенного значения в поле "Верхняя граница уставки"
    void on_lineEditMinSetPoint_textEdited(const QString &arg1);        //Слот валидации значения вводимого в поле "Нижняя граница уставки"
    void on_lineEditMinSetPoint_editingFinished();                      //Слот обработки введенного значения в поле "Нижняя граница уставки"
    void updateFormItems();                                             //Слот для обновления значений в элементах формы по сигналу таймера
private:
    const std::string vendorName = "NNSTU 22-VMv Khachenkov O.I. 2024";
    const std::string productName = "Temperature control of power TRM-01";
    Ui::MainWindow *ui;
    HIDLibrary <dataexchange_t>* hid = nullptr;                         //Указатель на объект класса HID устройства
    dataexchange_t* pdata = nullptr;                                    //Указатель на структуру данных настроек устройства
    QTimer* pollingTimer = nullptr;                                     //Указатель на объект таймер
    QDoubleValidator *validatorD = nullptr;                             //Указатель на валидатор чисел с плавающей точкой
    QIntValidator *validatorI = nullptr;                                //Указатель на валидатор чисел целого типа
    bool enadledFormItems = false;                                      //Флаг, хранящий состояние элементов формы
    bool connectDevice();                                               //Функция подключения к usb устройству
    bool exchangeData();                                                //Метод обновления настроек устройства и чтения фактических
    void sendDataToForm();                                              //Метод пересылки значений в элементы формы
    void setEnabledFormItem();                                          //Метод установки доступного состояния элементов формы
    void setDisabledFormItem();                                         //Метод установки недоступного состояния элементов формы
    void mainWindowResize();                                            //Метод установка необходимых размеров главного окна
};
#endif // MAINWINDOW_H
