#pragma once

#include <map>
#include <string>
#include <functional>

#include "PropertyCollection.h"
#include "TileSetCollection.h"
#include "LayerCollection.h"


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

	void Unload();

	TileSet *GetTileSetFromGID(unsigned int gTilesetId);

	//unsigned int LayerCount();
	//Layer* GetLayerById(unsigned int id);
	//Layer* GetLayerByName(const std::string& name);

	std::string version;
	Orientation orientation;
	RenderOrder renderOrder;

	unsigned int rows;
	unsigned int cols;
	unsigned int tileWidth;
	unsigned int tileHeight;

	TileSetCollection tileSets;
	LayerCollection namedLayers;
	PropertyCollection properties;

	std::vector<ILayer*> layers;
};
