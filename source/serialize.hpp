#pragma once
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#define Property(name, type, defaultValue) \
	private: \
		PropertyObject<type>* name = new PropertyObject<type>(#name, defaultValue, m_objs); \
	public: \
		type Get##name(){ return name->Get();} \
		void Set##name(type value){name->Set(value);}

class SerializeBase;
class OBase
{
public:
	OBase(std::string name) :m_name(name) {}
	virtual ~OBase() {}
	std::string GetName() { return m_name; }

protected:
	virtual std::string Serialize() = 0;
	virtual bool DeSerialize(std::string data) = 0;

protected:
	std::string m_name;
private:
	friend SerializeBase;
};

template<class T>
class PropertyObject : public OBase
{
public:
	PropertyObject(std::string name, T defaultValue):m_data(defaultValue), OBase(name) {}

	PropertyObject(std::string name, T defaultValue, std::vector<OBase*>& list)
		:m_data(defaultValue), OBase(name)
	{
		list.push_back(this);
	}

	void Set(T value) { m_data = value; }
	T Get() { return m_data; }

	std::string ToSerializeString()
	{
		std::string data = "{";
		data.append(Serialize());
		data.append("}");
		return data;
	}

	bool FromSerializeString(std::string json)
	{
		return DeSerialize(json);
	}

protected:
	std::string Serialize() {
		std::string data = "\"";
		data.append(m_name);
		data.append("\":");
		data.append(SerializeValue<T>());
		return data;
	}
	template<typename T>
	std::string SerializeValue();

	template<>
	std::string SerializeValue<int>() { return std::to_string(m_data); }

	template<>
	std::string SerializeValue<long>() { return std::to_string(m_data); }

	template<>
	std::string SerializeValue<float>() { return std::to_string(m_data); }

	template<>
	std::string SerializeValue<double>() { return std::to_string(m_data); }

	template<>
	std::string SerializeValue<bool>() { return m_data ? "true" : "false"; }

	template<>
	std::string SerializeValue<std::string>() { std::string data = "\""; data.append(m_data); data.append("\""); return data; }


	bool DeSerialize(std::string data)
	{
		rapidjson::Document doc;
		doc.Parse(data.c_str());
		if(!doc.HasParseError())
			if (doc.HasMember(m_name.c_str()))
				return DeSerializeValue<T>(doc);
		return false;
	}

	template<typename T>
	bool DeSerializeValue(rapidjson::Document& doc);

	template<>
	bool DeSerializeValue<int>(rapidjson::Document& doc)
	{
		bool bRet = doc[m_name.c_str()].IsInt();
		if (bRet) m_data = doc[m_name.c_str()].GetInt();
		return bRet;
	}

	template<>
	bool DeSerializeValue<long>(rapidjson::Document& doc)
	{
		bool bRet = doc[m_name.c_str()].IsInt64();
		if (bRet) m_data = doc[m_name.c_str()].GetInt64();
		return bRet;
	}

	template<>
	bool DeSerializeValue<float>(rapidjson::Document& doc)
	{
		bool bRet = doc[m_name.c_str()].IsFloat();
		if (bRet) m_data = doc[m_name.c_str()].GetFloat();
		return bRet;
	}

	template<>
	bool DeSerializeValue<double>(rapidjson::Document& doc)
	{
		bool bRet = doc[m_name.c_str()].IsDouble();
		if (bRet) m_data = doc[m_name.c_str()].GetDouble();
		return bRet;
	}

	template<>
	bool DeSerializeValue<bool>(rapidjson::Document& doc)
	{
		bool bRet = doc[m_name.c_str()].IsBool();
		if (bRet) m_data = doc[m_name.c_str()].GetBool();
		return bRet;
	}

	template<>
	bool DeSerializeValue<std::string>(rapidjson::Document& doc)
	{
		bool bRet = doc[m_name.c_str()].IsString();
		if (bRet) m_data = doc[m_name.c_str()].GetString();
		return bRet;
	}


private:
	T m_data;
};

class SerializeBase
{
public:
	SerializeBase(std::string sNameSpace):m_sNameSpace(sNameSpace){}
	virtual ~SerializeBase()
	{
		for (auto obj : m_objs) { delete obj; }
		m_objs.clear();
	}
	std::string ToSerializeString()
	{
		std::string data;
		std::string tail;
		if (!m_sNameSpace.empty())
		{
			data.append("{\"");
			data.append(m_sNameSpace);
			data.append("\":");
			tail = "}";
		}
		data.append("{");
		for (auto obj : m_objs) {
			data.append(obj->Serialize());
			data.append(",");
		}
		if (m_objs.size() > 0)
			data.pop_back();
		data.append("}");
		data.append(tail);
		return data;
	}

	bool FromSerializeString(std::string json)
	{
		std::string data = json;

		rapidjson::Document doc;
		doc.Parse(json.c_str());

		bool bRet = true;
		if (doc.HasParseError())
			return false;
		
		if (m_sNameSpace.empty() == false)
		{
			if (doc.HasMember(m_sNameSpace.c_str()) && doc[m_sNameSpace.c_str()].IsObject())
			{
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				rapidjson::Value& objs = doc[m_sNameSpace.c_str()];
				objs.Accept(writer);
				data = buffer.GetString();
			}
		}

		
		for (auto obj : m_objs)
		{
			if (!obj->DeSerialize(data))
			{
				bRet = false;
				printf("SerializeBase::FromSerializeString() %s resore failed!", obj->GetName().c_str());
			}
		}

		return bRet;
	}
protected:
	std::vector<OBase*> m_objs;
	std::string m_sNameSpace;
};

