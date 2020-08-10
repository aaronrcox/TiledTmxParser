#pragma once

#include <map>
#include <string>
#include <functional>

#include "PropertyCollection.h"
#include "TileSetCollection.h"
#include "NamedTileLayerCollection.h"
#include "NamedObjectGroupLayerCollection.h"

class ITileMapRenderer;
class ObjectGrpupLayer;

class TileMap
{
public:

	enum class Orientation {
		ORTHOGONAL,
		UNSUPPORTED,
		UNKNOWN
	};

	enum class RenderOrder {
		RIGHT_DOWN,
		RIGHT_UP,
		LEFT_DOWN,
		LEFT_UP,
		UNKNOWN
	};


public:

	TileMap();
	~TileMap();

	void Load(const char* filename);
	void LoadTextures(std::function<void(std::string)> loadTextureCallback);

	void SetRenderer(ITileMapRenderer* renderer);
	ITileMapRenderer* GetRenderer();

	TileLayer* GetTileLayer(const std::string& name);
	ObjectGroupLayer* GetObjectGroup(const std::string& name);
	

	void DrawLayer(ILayer *layer);

public:

	std::string version;
	std::string baseFilePath;
	Orientation orientation;
	RenderOrder renderOrder;

	unsigned int rows;
	unsigned int cols;
	unsigned int tileWidth;
	unsigned int tileHeight;

	PropertyCollection properties;

	TileSetCollection tileSets;

	NamedTileLayerCollection namedTileLayers;
	NamedObjectGroupLayerCollection namedObjectLayers;
	
	std::vector<ILayer*> layers;

private:
	ITileMapRenderer* m_renderer = nullptr;
};
