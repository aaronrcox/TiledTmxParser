#pragma once

#include "PropertyCollection.h"

class TileMap;

/// <summary>
/// Base Class for Layers in the tilemap
/// </summary>
class ILayer
{
public:

	enum class LayerType {
		LAYER,
		IMAGE_LAYER, // currently un-supported
		OBJECT_GROUP // currently un-supported
	};

	// unique layer id
	unsigned int id = 0;
	
	// name assigned to the layer	
	std::string name = "";

	// identifyies which type of layer this instance is.
	LayerType type;

	// pointer back to the map
	TileMap* map = nullptr;

	// the opacity of the layer
	float opacity = 1.0f;

	// indicates if the layer is hidden
	bool visible = true;

	// tint color multiplied with tile/object when rendered
	unsigned int tintColor = 0xFFFFFFFF;

	// horizontal offset for the layer
	int offsetX = 0;

	// vertical offset for the layer
	int offsetY = 0;

	// properties saved on the layer
	PropertyCollection properties = PropertyCollection();


public:

	ILayer(LayerType type) : type(type) { }
	virtual ~ILayer() { }
};

