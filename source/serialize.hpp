/***********************************************************************************/
// Copyright (C) 2021 Meng Naihong.
//
// Licensed under the MIT License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
// Contact mengnaihong@foxmail.com for support.
/***********************************************************************************/
#pragma once
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/StringBuffer.h"

#define NameSpace(namespace) \
	public: \
	virtual std::string GetNameSpace() { return #namespace; }

#define Property(name, type, defaultVal) \
	private: \
		PropertyObject<type>* m_##name = new PropertyObject<type>(#name, defaultVal, m_objs); \
	public: \
		type Get##name(){ return m_##name->Get();} \
		void Set##name(type value){m_##name->Set(value);}

#define PropertyArray(name, type) \
	private: \
		PropertyArrayBase<type>* m_array##name = new PropertyArrayBase<type>(#name, m_objs); \
	public: \
		void Add##name(type val){m_array##name->Add(val);} \
		std::vector<type> Get##name(){return m_array##name->GetArray();}

class SerializeBase;
class PropertyBase
{
public:
	PropertyBase(std::string name) :m_name(name) {}
	virtual ~PropertyBase() {}

	std::string GetName() { return m_name; }

	virtual std::string Serialize() = 0;

	virtual bool DeSerialize(std::string& data) = 0;

protected:
	std::string m_name;
private:
	friend SerializeBase;
};

template<typename T>
class PropertyObject : public PropertyBase
{
public:
	PropertyObject(std::string name, T defaultVal):m_data(defaultVal), PropertyBase(name) {}

	PropertyObject(std::string name, T defaultVal, std::vector<PropertyBase*>& list)
		:m_data(defaultVal), PropertyBase(name)
	{
		list.push_back(this);
	}

	void Set(T value, bool bUserSet = true) { m_data = value; }

	T Get() { return m_data; }

	std::string Serialize() override{
		rapidjson::StringBuffer strBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
		writer.StartObject();
		writer.Key(m_name.c_str());

		SerializeValue<T>(writer);

		writer.EndObject();
		return strBuffer.GetString();
	}

	bool DeSerialize(std::string& data) override {
		rapidjson::Document doc;
		doc.Parse(data.c_str());
		if (!doc.HasParseError())
			if (doc.HasMember(m_name.c_str()))
				return DeSerializeValue<T>(doc[m_name.c_str()]);
		return false;
	}

protected:
	template<typename T>
	void SerializeValue(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	template<>
	void SerializeValue<bool>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { writer.Bool(m_data); }

	template<>
	void SerializeValue<int>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { writer.Int(m_data); }

	template<>
	void SerializeValue<long>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { writer.Int64(m_data); }

	template<>
	void SerializeValue<float>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { writer.Double(m_data); }

	template<>
	void SerializeValue<double>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { writer.Double(m_data); }


	template<>
	void SerializeValue<std::string>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { writer.String(m_data.c_str()); }


	template<typename T>
	bool DeSerializeValue(rapidjson::Value& val);

	template<>
	bool DeSerializeValue<bool>(rapidjson::Value& val) { if (val.IsBool()) Set(val.GetBool(), false); return val.IsBool(); }

	template<>
	bool DeSerializeValue<int>(rapidjson::Value& val) { if (val.IsInt()) Set(val.GetInt(), false); return val.IsInt();}

	template<>
	bool DeSerializeValue<long>(rapidjson::Value& val) { if (val.IsInt64()) Set(val.GetInt64(), false); return val.IsInt64(); }

	template<>
	bool DeSerializeValue<float>(rapidjson::Value& val) { if (val.IsFloat()) Set(val.GetFloat(), false); return val.IsFloat(); }

	template<>
	bool DeSerializeValue<double>(rapidjson::Value& val) { if (val.IsDouble()) Set(val.GetDouble(), false); return val.IsDouble(); }

	template<>
	bool DeSerializeValue<std::string>(rapidjson::Value& val) { if (val.IsString()) Set(val.GetString(), false); return val.IsString(); }

private:
	T m_data;
};

template<typename T>
class PropertyArrayBase : public PropertyBase
{
public:
	PropertyArrayBase(std::string name) :PropertyBase(name) {}
	PropertyArrayBase(std::string name, std::vector<PropertyBase*>& list)
		:PropertyBase(name)
	{
		list.push_back(this);
	}

	void Add(T val) { m_list.push_back(val); }

	void Clear() { m_list.clear(); }

	std::vector<T> GetArray() { return m_list; }

	virtual std::string Serialize() override {
		if (m_list.size() > 0)
		{
			if (m_list.size() > 0)
			{
				rapidjson::StringBuffer strBuffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
				writer.StartObject();
				writer.Key(m_name.c_str());
				writer.StartArray();

				SerializeValue<T>(writer);

				writer.EndArray();
				writer.EndObject();
				return strBuffer.GetString();
			}
		}
		return "";
	}

	virtual bool DeSerialize(std::string& data) override {
		rapidjson::Document doc;
		doc.Parse(data.c_str());
		const char* name = m_name.c_str();
		if (!doc.HasParseError())
			if (doc.HasMember(name) && doc[name].IsArray())
			{
				m_list.clear();
				rapidjson::Value& arrays = doc[name];

				bool bRet = true;
				for (rapidjson::Value& obj : arrays.GetArray()) {
					if (!DeSerializeValue<T>(obj))
						bRet = false;
				}
				return bRet;
			}
		return false;
	}

protected:
	template<typename T>
	void SerializeValue(rapidjson::Writer<rapidjson::StringBuffer>& writer);

	template<>
	void SerializeValue<bool>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { for (auto member : m_list) writer.Bool(member); }

	template<>
	void SerializeValue<int>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { for (auto member : m_list) writer.Int(member); }

	template<>
	void SerializeValue<long>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { for (auto member : m_list) writer.Int(member); }

	template<>
	void SerializeValue<float>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { for (auto member : m_list) writer.Double(member); }

	template<>
	void SerializeValue<double>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { for (auto member : m_list) writer.Double(member); }

	template<>
	void SerializeValue<std::string>(rapidjson::Writer<rapidjson::StringBuffer>& writer) { for (auto member : m_list) writer.String(member.c_str()); }

	template<typename T>
	bool DeSerializeValue(rapidjson::Value& val);

	template<>
	bool DeSerializeValue<bool>(rapidjson::Value& val) { if (val.IsBool()) m_list.push_back(val.GetBool()); return val.IsBool(); }

	template<>
	bool DeSerializeValue<int>(rapidjson::Value& val){ if (val.IsInt()) m_list.push_back(val.GetInt()); return val.IsInt(); }

	template<>
	bool DeSerializeValue<long>(rapidjson::Value& val) { if (val.IsInt64()) m_list.push_back(val.GetInt64()); return val.IsInt64(); }

	template<>
	bool DeSerializeValue<float>(rapidjson::Value& val) { if (val.IsFloat()) m_list.push_back(val.GetFloat()); return val.IsFloat(); }

	template<>
	bool DeSerializeValue<double>(rapidjson::Value& val) { if (val.IsDouble()) m_list.push_back(val.GetDouble()); return val.IsDouble(); }

	template<>
	bool DeSerializeValue<std::string>(rapidjson::Value& val) { if (val.IsString()) m_list.push_back(val.GetString()); return val.IsString(); }


private:
	std::vector<T> m_list;
};

class SerializeBase
{
public:
	SerializeBase(){}
	virtual ~SerializeBase()
	{
		for (auto obj : m_objs) { delete obj; }
		m_objs.clear();
	}
	virtual std::string GetNameSpace() { return ""; }

	std::string ToSerializeString()
	{
		std::string data;
		std::string tail;
		std::string sNameSpace = GetNameSpace();
		if (!sNameSpace.empty())
		{
			data.append("{\"");
			data.append(sNameSpace);
			data.append("\":");
			tail = "}";
		}
		data.append("{");
		bool bHasValidItem = false;
		for (auto obj : m_objs) {
			std::string sJson = obj->Serialize();

			//HasValidItem
			if (sJson.size() > 2)
			{
				bHasValidItem = true;
				data.append(sJson.substr(1, sJson.size() - 2));
				data.append(",");
			}
		}
		if (m_objs.size() > 0 && bHasValidItem)
			data.pop_back();

		data.append("}");
		data.append(tail);
		return data;
	}

	bool FromSerializeString(std::string& json)
	{
		std::string data = json;
		rapidjson::Document doc;
		doc.Parse(json.c_str());

		bool bRet = true;
		if (doc.HasParseError())
		{
			printf("SerializeBase::ParseError Code=%d, ErrorOffset=%d\n", doc.GetParseError(), doc.GetErrorOffset());
			return false;
		}
		
		std::string sNameSpace = GetNameSpace();
		if (sNameSpace.empty() == false)
		{
			if (doc.HasMember(sNameSpace.c_str()) && doc[sNameSpace.c_str()].IsObject())
			{
				rapidjson::StringBuffer buffer;
				rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
				rapidjson::Value& objs = doc[sNameSpace.c_str()];
				objs.Accept(writer);
				data = buffer.GetString();
			}
		}

		for (auto obj : m_objs)
		{
			if (!obj->DeSerialize(data))
			{
				bRet = false;
				printf("SerializeBase::FromSerializeString() %s restore failed!", obj->GetName().c_str());
			}
		}

		return bRet;
	}
protected:
	std::vector<PropertyBase*> m_objs;
};