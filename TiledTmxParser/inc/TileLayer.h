#pragma once

#include <string>
#include <vector>
#include "PropertyCollection.h"
#include "TileState.h"

typedef std::vector<uint32_t> TileLayerData;
typedef std::vector<TileState> TileStateLookup;

class TileMap;
class TileSet;
class TileState;

class ILayer
{
public:

	enum class LayerType {
		LAYER, 
		IMAGE_LAYER, // currently un-supported
		OBJECT_GROUP // currently un-supported
	};

	// identifyies which type of layer this instance is.
	LayerType type;

	// pointer back to the map
	TileMap* map = nullptr;

public:

	ILayer(LayerType type) : type(type) { }
	virtual ~ILayer() { }
};

class TileLayer : public ILayer
{
public:

	TileLayer() : ILayer(ILayer::LayerType::LAYER) { }
	virtual ~TileLayer() { }

	TileState& GetTileData(unsigned int x, unsigned int y);

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

	// tile data calculated after rawTileData has been generated
	TileStateLookup tileData = TileStateLookup();

	// Raw tile data as stored in tmx file
	TileLayerData rawTileData = TileLayerData();
};