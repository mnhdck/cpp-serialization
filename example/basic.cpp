#include <iostream>
#include "serialize.hpp"

class CarInfo: public SerializeBase
{
public:
	GroupName("CarInfo");

	Property(Speed, int, 0);
	Property(volume, int, 0);
	Property(Light, bool, false);
	Property(Temp, float, 0.f);
	Property(Name, std::string, "Mengziqing");

	PropertyArray(ArrayInt, int);
}

int main()
{
    CarInfo test;
	std::string name = test.GetGroupName();
    std::cout << name <<std::endl;

	test.SetSpeed(28);
    int testValue = test.GetSpeed();
	test.AddArrayInt(22);
	test.AddArrayInt(66);
	test.AddArrayInt(300);

	// Serialize
	std::string strJson = test.ToSerializeString();
    std::cout << strJson.c_str() <<std::endl;

    // DeSerialize
	CarInfo test2;
	bool bSuccess = test2.FromSerializeString(strJson);
    if(bSuccess)
	{
        std::vector<int> vecInt = test2.GetArrayInt();
	    int speed =  test2.GetSpeed();
        std::cout << "Speed=" << speed <<std::endl;
    }

    // Single use
    PropertyObject<int> my("hehe",10);
	my.Set(333);
	std::string sss = my.Serialize();

	PropertyObject<int> my2("hehe", 10);
	my2.DeSerialize(sss);

    // Single static use
    std::string jsstr = PropertyObject<int>::CreateJson("hi", 250);
	int number = PropertyObject<int>::CreateObject(jsstr).Get();

    // Get serialize string's name for compare
    std::string sName = PropertyBase::GetSerializeStringName(strJson);
    
    return 0;
}