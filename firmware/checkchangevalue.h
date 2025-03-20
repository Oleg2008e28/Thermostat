/* checkchangevalue.h
 * ������: 02.04.2024
 * ������������� ��������������� ����������� ����������� ��. �.�. ���������
 * ������ 22-��� �������� �.�. 
 * ������ ����� ������ ����������� ��������� ��������������� ��������.
 */ 

#ifndef CHECKCHANGEVALUE_H_
#define CHECKCHANGEVALUE_H_

template <typename T> class CheckChangeValue{
public:
	CheckChangeValue(T & v);											//����������� ������� ���������� ������
	bool checkChange();													//�����, ������������ true, ���� ���� ��������� ��������.
	void updatValue();													//�����, ����������� �������������� ��������.
private:
	T * value;															//���������� ���� ������ ��� �������� ������ �������������� ����������.
	T backValue;														//�������� ��� �������� �������� �������������� ��������.
};
template <typename T> CheckChangeValue<T>::CheckChangeValue(T & v){		//��� �������� ������� ������,
	backValue = v;														//�������� �������������� ���������� ����������� � ���������� ������,
	value = & v;														//���� ������ ���������, �������� ����� �������������� ����������.
}
template <typename T> bool CheckChangeValue<T>::checkChange(){			//����� ������ ��������� ��������� ����������� 
	if (backValue == *value)											//���������� � �� �������� ��������. 
		return false;													//���� �������� ����������, ������������ false,
	else return true;													//���� ������ true.
}
template <typename T> void CheckChangeValue<T>::updatValue(){			//����� ��������� �������� ���������� ����������� � ������� ������.
	backValue = *value;													//� ���������� ����� ����������� ������� �������� �������������� ����������.
}
#endif /* CHECKCHANGEVALUE_H_ */