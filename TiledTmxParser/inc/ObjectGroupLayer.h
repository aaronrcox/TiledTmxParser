#pragma once

#include <string>
#include <vector>
#include "ILayer.h"

class IObject
{
public:

	struct Point
	{
		int x; 
		int y;
	};

	enum class Type
	{
		POINT, POLYGON, POLYLINE, ELLIPSE,
		TEXT, // TEXT NOT YET SUPPORTED
	};

	// unique id of the object
	unsigned int id;

	Type shapeType = Type::POINT;

	// name of the object (not unique)
	std::string name = "";

	// type of the object (not unique) arbitary string
	std::string type = "";

	// x/y/width/height position/size (pixels) of object 
	int x = 0;
	int y = 0;
	int width = 1;
	int height = 1;
	float rotation = 0.0f;

	// indicates if the object is visible / hidden
	bool visible;

	// properties stored on the object
	PropertyCollection properties;


	std::vector<Point> shapePoints;

};

typedef std::vector<IObject> ObjectCollection;
typedef std::vector<IObject*> ObjectPtrCollection;

class ObjectGroupLayer : public ILayer
{
public:

	ObjectGroupLayer() : ILayer(ILayer::LayerType::OBJECT_GROUP) { }
	virtual ~ObjectGroupLayer() { }

	// TODO: Optimise
	// Slow: linear search - returns first object found that matches name
	IObject* GetObjectByName(const std::string& name);

	// TODO: Optimise
	// Slow: linear search - returns collection of objects that matches name
	ObjectPtrCollection GetObjectsByName(const std::string& name);
	
	// TODO: Optimise
	// Slow: linear search - returns first object found that matches type
	IObject* GetObjectByType(const std::string& type);

	// TODO: Optimise
	// Slow: linear search - returns first object found that matches type
	ObjectPtrCollection GetObjectsByType(const std::string& type);

	ObjectCollection objects;

};

