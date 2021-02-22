#include "serialization.hpp"

struct StructCustom {
	int a = 0;
	bool b = false;
};

// extend PropertyBase
class PropertyObjectCustom : public PropertyBase
{
public:
	PropertyObjectCustom(std::string name, StructCustom defaultValue) :m_data(defaultValue), PropertyBase(name) {}
	PropertyObjectCustom(std::string name, StructCustom defaultValue, std::vector<PropertyBase*>& list)
		:m_data(defaultValue), PropertyBase(name)
	{
		list.push_back(this);
	}

	void Set(StructCustom value) { m_data = value; }
	StructCustom Get() { return m_data; }

    // 按保存的结构实现序列化接口
	virtual std::string Serialize() override {
		rapidjson::StringBuffer strBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);

		writer.StartObject();
		writer.Key(m_name.c_str());
		writer.StartObject();

		writer.Key("a");
		writer.Int(m_data.a);
		writer.Key("b");
		writer.Bool(m_data.b);

		writer.EndObject();
		writer.EndObject();

		return strBuffer.GetString();
	}

    // 按保存的结构实现反序列化接口
	virtual bool DeSerialize(std::string& data) override { 
		rapidjson::Document doc;
		doc.Parse(data.c_str());
		const char* name = m_name.c_str();
		if (!doc.HasParseError())
			if (doc.HasMember(name) && doc[name].IsObject())
			{
				if (doc[name].HasMember("a") && doc[name]["a"].IsInt())
					m_data.a = doc[name]["a"].GetInt();
				else
					return false;
				if (doc[name].HasMember("b") && doc[name]["b"].IsBool())
					m_data.a = doc[name]["b"].GetBool();
				else
					return false;
				return true;
			}
				
		return false;
	}

private:
	StructCustom m_data;
};

// 可以自己写个宏定义简化定义，也可以单独声明，如下例子所示
#define PropertyCustom(name, type, defaultValue, propertyDeliver) \
	private: \
		propertyDeliver* m_##name = new propertyDeliver(#name, defaultValue, m_objs); \
	public: \
		type Get##name(){return m_##name->Get();} \
		void Set##name(type val){m_##name->Set(val);}

class CarInfo: public SerializeBase
{
public:
	NameSpace(CarInfo);

	Property(Speed, int, 0);
	PropertyArray(ArrayInt, int);

    // custom struct
    PropertyCustom(MyStruct, StructCustom, StructCustom(), PropertyObjectCustom);

	// or you can also define like this
    PropertyObjectCustom* myObjectStructCustom = new PropertyObjectCustom("MyStruct2", { 20,true }, m_objs);
		
};

int main()
{
	CarInfo carInfo;
	std::string nameSpace = carInfo.GetNameSpace();
	carInfo.SetSpeed(28);
	
	carInfo.SetMyStruct({ 222,true });

	std::string strJson = carInfo.ToSerializeString();


	CarInfo structCustom2;
	bool bSuccess = structCustom2.FromSerializeString(strJson);
	int speed =  structCustom2.GetSpeed();

    // 这样就获取到了自定义的结构数据，同理数组也可以自定义，参考PropertyArrayBase
    StructCustom mystruct = structCustom2.GetMyStruct();

    return 0；
}