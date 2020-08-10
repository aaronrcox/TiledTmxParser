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

#include "ILayer.h"

class TileLayer : public ILayer
{
public:

	TileLayer() : ILayer(ILayer::LayerType::LAYER) { }
	virtual ~TileLayer() { }

	TileState& GetTileData(unsigned int x, unsigned int y);

public:

	unsigned int rows = 0;
	unsigned int cols = 0;

	// tile data calculated after rawTileData has been generated
	TileStateLookup tileData = TileStateLookup();

	// Raw tile data as stored in tmx file
	TileLayerData rawTileData = TileLayerData();
};