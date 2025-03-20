/* hidlibrary.h
 * Создан: 13.03.2024 23:51:44
 * Нижегородский Государственный технический университет им. Р.Е.Алексеева
 * группа 22-ВМв Хаченков О.И.
 * Шаблонный класс HIDLibrary предназначен для организации передачи данных
 * от usb хоста к HID устройству. Данный класс построен на использовании
 * библиотеки libusb-1.0, которая входит в станндартый пакет установки 
 * Linux Astra. Для выполнения сборки проекта требуется дополнительно установить 
 * пакет libusb-1.0-0-dev.
 */
#ifndef HIDLIBRARY_H
#define HIDLIBRARY_H

#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include <vector>
#include <string>
#include <QMessageBox>
#include <QDebug>

template <typename T> class HIDLibrary{

public:
    HIDLibrary(unsigned short IdVendor, unsigned short IdProduct);
    ~HIDLibrary();
    bool initLibUsb();                              //Метод инициализации библиотеки libusb-1.0 и получение контекста
    int enumerateHIDDevices();                      //Метод перебора всех устройств и формирования списка нужных по ven и prod
    void printHIDDevices();                         //Отладочный метод, распечатывает список устройств
    bool connect(unsigned int device=0);            //Метод поиска и присвоения номера устройства к которому будет осуществляться подключение
    std::string getConnectedDeviceVendorName();     //Метод передачи строкового значения названия производителя
    std::string getConnectedDeviceProductName();    //Метод передачи строкового значения имени устройства
    std::string getConnectedDeviceSerialNum();      //Метод передачи серийного номера устроиства
    int sendData(T* data);                          //Метод передачи структуры данных от usb хоста к устройству
    int receiveData(T* data);                       //Метод чтения структуры данных usb хостом из устройства
private:
    const int interfaceTRM = 0;
    T data;
    struct devAttributes {                          //Структура для хранения атрибутов найденных устройств
        std::string devVendor;                      //Vendor - производитель устройства
        std::string devProduct;                     //Product - имя устпройства
        std::string devSerialNum;                   //Серийный номен устройства
        libusb_device* dev;                         //Указатель на дескриптор устройства
    };
    unsigned short idVendor;                        //Номер производителя
    unsigned short idProduct;                       //Номер апродукта
    int m_ConnectedDevice;                          //Номер по списку устройства, к которому происходит подключение
    bool flagAlarmMessage = false;                  //Флаг для одноразового оповещения об отсутствии прав доступа
    libusb_device **devs = nullptr;                 //Умазатель на массив устройств USB
    libusb_context *ctx = nullptr;                  //Контекст
    std::vector<struct devAttributes>deviceList;    //Вектор для хранения доступных устройств
};

template< typename T > HIDLibrary< T >::HIDLibrary(unsigned short IdVendor, unsigned short IdProduct){
    idVendor = IdVendor;                                        //Передаем объекту номер пргоизводителя
    idProduct = IdProduct;                                      //Передаем объекту номер продукта
}
template< typename T > HIDLibrary< T >::~HIDLibrary(){
    if(ctx){
        libusb_exit(ctx); //Если библиотека libusb-1.0 была инициализированна
    }                     //освобождаем контекст.
}
template< typename T > bool HIDLibrary< T >::initLibUsb(){
    if (libusb_init(&ctx)==LIBUSB_SUCCESS){                     //Если инициализация библиотеки libusb успешна
        return	true;                                           //и контекст получен, то возвращаеется истина.
    }
    else{                                                       //Иначе, если инициализация не выполнена, 
        return false;                                           //возвращается ложь.
    }
}

template<typename T> int HIDLibrary<T>::enumerateHIDDevices(){
    deviceList.clear();
    int cnt = 0;                                                //Переменная для сохранения количества устройств на шине 
    libusb_device_descriptor desc;                              //Дескриптор usb устройства
    cnt = libusb_get_device_list(ctx, &devs);                   //Получение количества устройств на шине
    if (cnt){                                                   //Если устройства на шине существуют
        for (int i=0; i<cnt; i++){                              //Перебор всех устройств
                if (libusb_get_device_descriptor(devs[i], &desc)==LIBUSB_SUCCESS) { //Получаем дескриптор устройства
                    if (desc.idVendor == idVendor && desc.idProduct == idProduct){       //Если при переборе устройств встретилось нужное
                            libusb_device_handle *handle = nullptr;                 //Указатель на хендел устройства
                        if (libusb_open(devs[i], &handle) == LIBUSB_SUCCESS){       //Если устройство доступно получаем его нимер в системе
                            const std::size_t size_buf = 255;
                            uint8_t  vendorName_cstr[size_buf];
                            uint8_t  productName_cstr[size_buf];
                            uint8_t serialNum_cstr[size_buf];
                            if (libusb_get_string_descriptor_ascii(handle,desc.iManufacturer,vendorName_cstr,size_buf) //Чтение названия производителя
                                && libusb_get_string_descriptor_ascii(handle,desc.iProduct,productName_cstr,size_buf)  //и названия продукта
                                && libusb_get_string_descriptor_ascii(handle,desc.iSerialNumber,serialNum_cstr,size_buf)){
                                struct devAttributes tempStruct = { std::string( (char*)vendorName_cstr),
                                                                    std::string((char*)productName_cstr),
                                                                    std::string((char*)serialNum_cstr),
                                                                    devs[i]
                                                                };
                                deviceList.push_back(tempStruct);
                                if(handle != nullptr){
                                    libusb_close(handle);
                                }
                            }
                        }
                        else{
                                if (!flagAlarmMessage){
                                    QMessageBox msgBox;                                                     //В случае неудачи, создается окно сообщения
                                    msgBox.setText("Устройство не доступно. "                               //Которое показывается один раз
                                                   "Перезапустите программу с правами root.");              //а приложение продолжает работать дальше.
                                    msgBox.setIcon(QMessageBox::Warning);
                                    msgBox.exec();
                                    flagAlarmMessage = true;
                                }
                             }
                    }
                }
        }
    }
    return deviceList.size();
}

template<typename T> bool HIDLibrary<T>::connect(unsigned int device){
    m_ConnectedDevice = device;
	return true;
}

template<typename T> int HIDLibrary<T>::sendData(T* data){
    libusb_device_handle *handle = NULL;                    //Указатель на хендел устройства
    if (libusb_open(deviceList[m_ConnectedDevice].dev, &handle) == LIBUSB_SUCCESS){
        #ifdef __linux__
        if (libusb_kernel_driver_active(handle,interfaceTRM)){
            libusb_detach_kernel_driver(handle,interfaceTRM);   //Отбираем контроль у драйвера
        }
        #endif
        //Конфигурируем устройство
        libusb_set_configuration(handle, 1);
        if (libusb_claim_interface(handle, interfaceTRM) != LIBUSB_SUCCESS){
        };
        int r;
        r = libusb_control_transfer(handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, 0x09, 0, 0, (unsigned char*)(data), sizeof(*data), 1000);
        libusb_release_interface(handle,0);
        #ifdef __linux__
        libusb_attach_kernel_driver(handle,interfaceTRM); //Возвращаем управление драйверу
        #endif
        libusb_close(handle);
        if(r == sizeof(*data)) return true;
        else return false;
    }       
	return false;
}

template<typename T> int HIDLibrary<T>::receiveData(T* data){
    libusb_device_handle *handle = NULL;                    //Указатель на хендел устройства
    if (libusb_open(deviceList[m_ConnectedDevice].dev, &handle) == LIBUSB_SUCCESS){
        #ifdef __linux__
        //Отбираем контроль у драйвера
        if (libusb_kernel_driver_active(handle,interfaceTRM)){
            libusb_detach_kernel_driver(handle,interfaceTRM);
        }
        #endif
        //Конфигурируем устройство
        libusb_set_configuration(handle, 1);
        libusb_claim_interface(handle, interfaceTRM);
        int r;
        r = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, 0x09, 0, 0, (unsigned char*)(data), sizeof(*data), 1000);
        libusb_release_interface(handle,0);
        #ifdef __linux__
        libusb_attach_kernel_driver(handle,interfaceTRM); //Возвращаем управление драйверу
        #endif
        libusb_close(handle);
        if(r == sizeof(*data)) return true;
        else return false;
    }       
	return false;
}

template<typename T> std::string HIDLibrary<T>::getConnectedDeviceVendorName(){
        return deviceList[m_ConnectedDevice].devVendor;
}

template<typename T> std::string HIDLibrary<T>::getConnectedDeviceProductName(){
        return deviceList[m_ConnectedDevice].devProduct;
}

template<typename T> std::string HIDLibrary<T>::getConnectedDeviceSerialNum(){
        return deviceList[m_ConnectedDevice].devSerialNum;
}

#endif // HIDLIBRARY_H
