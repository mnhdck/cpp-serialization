#include <iostream>
#include "serialize.hpp"

class CarInfo: public SerializeBase
{
public:
	NameSpace(CarInfo);

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
	std::string nameSpace = test.GetNameSpace();
    std::cout << NameSpace <<std::endl;

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
    return 0;
}