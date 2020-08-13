#include "Property.h"

Property::Property() :
	value(""),
	type(Type::STRING)
{

}

Property::Property(const std::string& value, Type type) :
	value(value), 
	type(type)
{

}

void Property::Set(const std::string& val)
{
	value = val;
	type = Type::STRING;
}
void Property::Set(int val)
{
	value = std::to_string(val);
	type = Type::INT;
}
void Property::Set(float val)
{
	value = std::to_string(val);
	type = Type::FLOAT;
}
void Property::Set(bool val)
{
	value = val ? "true" : "false";
	type = Type::BOOL;
}
void Property::SetColor(unsigned int color)
{
	// TODO:
}
void Property::SetFile(const std::string& filename)
{
	// TODO:
}
void Property::SetObject(unsigned int objectId)
{
	// TODO:
}
