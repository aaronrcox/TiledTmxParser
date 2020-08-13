#pragma once
#include <string>


class Property
{
public:

	enum class Type {
		STRING, INT, FLOAT, BOOL, COLOR, FILE, OBJECT
	};

public:

	Property();
	Property(const std::string& value, Type type);


	void Set(const std::string& value);
	void Set(int value);
	void Set(float value);
	void Set(bool value);
	void SetColor(unsigned int color);
	void SetFile(const std::string& filename);
	void SetObject(unsigned int objectId);
	
	std::string value;
	Type type;
};
