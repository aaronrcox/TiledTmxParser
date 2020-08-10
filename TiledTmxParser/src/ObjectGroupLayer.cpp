
#include "ObjectGroupLayer.h"

IObject* ObjectGroupLayer::GetObjectByName(const std::string& name)
{
	ObjectPtrCollection found;
	for (auto& obj : objects)
	{
		if (obj.name == name)
			return &obj;
	}
	
	return nullptr;
}

ObjectPtrCollection ObjectGroupLayer::GetObjectsByName(const std::string& name)
{
	ObjectPtrCollection found;
	for (auto& obj : objects)
	{
		if (obj.name == name)
			found.push_back(&obj);
	}
	return found;
}

IObject* ObjectGroupLayer::GetObjectByType(const std::string& type)
{
	ObjectPtrCollection found;
	for (auto& obj : objects)
	{
		if (obj.type == type)
			return &obj;
	}
	return nullptr;
}

ObjectPtrCollection ObjectGroupLayer::GetObjectsByType(const std::string& type)
{
	ObjectPtrCollection found;
	for (auto& obj : objects)
	{
		if (obj.type == type)
			found.push_back(&obj);
	}
	return found;
}

