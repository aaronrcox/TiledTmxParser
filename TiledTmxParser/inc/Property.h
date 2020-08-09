#pragma once
#include <string>


class Property
{
public:

	enum class Type {
		STRING, INT, FLOAT, BOOL, COLOR, FILE, OBJECT
	};

public:

	Property() : 
		name(""), value(""), type(Type::STRING) { }

	Property(const std::string& name, const std::string& value, Type type) : 
		name(name), value(value), type(type) { }

	std::string name;
	std::string value;
	Type type;
};
