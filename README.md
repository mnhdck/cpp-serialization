# cpp-serialization

#### 介绍
- C++对象序列化、反序列化，
- 使用C11与rapidJson，自动完成序列化与反序列化，
- 包含头文件即可直接使用，无其他依赖。

#### 软件架构
借助C++11新特性 + 宏 + rapidjson，
可扩展对象类型，支持动态数据

#### 安装教程

1.  复制source中的文件到你的工程
2.  include "serialize.hpp"
3.  即可使用

#### 使用说明

1.  简单对象使用如下：

```
    PropertyObject<int> my("Speed",10);
    my.Set(333);
    std::string str = my.Serialize(); //序列化

    PropertyObject<int> my2("Speed", 0);
    bool bSuccess = my2.DeSerialize(str );
    int nSpeed = my2.Get();
```


2.  对象组合用法，详见example文件夹

```
// 定义一个类（需继承自SerializeBase）

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

// Serialize
CarInfo obj;
std::string nameSpace = obj.GetNameSpace();
obj.SetSpeed(28);
obj.AddArrayInt(22);
obj.AddArrayInt(66);
std::string strJson = test.ToSerializeString();

// DeSerialize
CarInfo recieveObj;
bool bSuccess = recieveObj.FromSerializeString(strJson);
std::vector<int> vecInt = recieveObj.GetArrayInt();
int speed =  recieveObj.GetSpeed();
```

3.  宏的使用 
NameSpace：为类提供一个命名空间，在生成json时会作为root对象的名称，不提供则为空。
Property：定义一个属性
PropertyArray：定义一个属性数组


#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


