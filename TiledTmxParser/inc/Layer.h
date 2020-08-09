#pragma once

#include <string>
#include <vector>
#include "PropertyCollection.h"

typedef std::vector<uint32_t> LayerData;



class ILayer
{
public:

	enum class LayerType {
		LAYER, IMAGE_LAYER, OBJECT_GROUP
	};

	LayerType type;

public:

	ILayer(LayerType type) : type(type) { }
	virtual ~ILayer() { }
};

class Layer : public ILayer
{
public:

	struct TileState
	{
		unsigned int globalTileId;
		bool flipped_vertical;
		bool flipped_horizontal;
		bool flipped_diagonally;
	};

public:

	Layer() : ILayer(ILayer::LayerType::LAYER) { }
	virtual ~Layer() { }

	TileState GetTileData(unsigned int x, unsigned int y);

public:

	unsigned int id = 0;
	std::string name = "";

	unsigned int rows = 0;
	unsigned int cols = 0;

	float opacity = 1.0f;
	bool visible = true;

	unsigned int tintColor = 0xFFFFFFFF;

	int offsetX = 0;
	int offsetY = 0;

	PropertyCollection properties = PropertyCollection();
	LayerData data = LayerData();
};