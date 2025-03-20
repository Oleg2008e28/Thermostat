/* checkchangevalue.h
 * Создан: 02.04.2024
 * Нижегородский Государственный технический университет им. Р.Е. Алексеева
 * группа 22-ВМв Хаченков О.И. 
 * Объект этого класса отслеживает изменение контролируемого значения.
 */ 

#ifndef CHECKCHANGEVALUE_H_
#define CHECKCHANGEVALUE_H_

template <typename T> class CheckChangeValue{
public:
	CheckChangeValue(T & v);											//Конструктор объекта шаблонного класса
	bool checkChange();													//Метод, возвращающий true, если было изменение значения.
	void updatValue();													//Метод, обновляющий контролируемое значение.
private:
	T * value;															//Переменная член класса для хранения адреса контролируемой переменной.
	T backValue;														//Перемена для хранения значения контролируемой величины.
};
template <typename T> CheckChangeValue<T>::CheckChangeValue(T & v){		//При создании объекта класса,
	backValue = v;														//значение контролируемой переменной сохраняется в переменную класса,
	value = & v;														//член класса указатель, получает адрес контролируемой переменной.
}
template <typename T> bool CheckChangeValue<T>::checkChange(){			//Метод класса выполняет сравнение сохраненной 
	if (backValue == *value)											//переменной и ее текущего значения. 
		return false;													//Если значения одинаковые, возвращается false,
	else return true;													//если разные true.
}
template <typename T> void CheckChangeValue<T>::updatValue(){			//Метод обновляет значение переменной сохраненной в объекте класса.
	backValue = *value;													//В переменную класс сохраняется текущее значение контролируемой переменной.
}
#endif /* CHECKCHANGEVALUE_H_ */