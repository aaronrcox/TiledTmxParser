#pragma once

#include <map>
#include <string>
#include <functional>

#include "PropertyCollection.h"
#include "TileSetCollection.h"
#include "NamedTileLayerCollection.h"

class ITileMapRenderer;

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

	void DrawLayer(ILayer *layer);

	TileSet *GetTileSetFromGID(unsigned int gTilesetId);

public:

	std::string version;
	std::string baseFilePath;
	Orientation orientation;
	RenderOrder renderOrder;

	unsigned int rows;
	unsigned int cols;
	unsigned int tileWidth;
	unsigned int tileHeight;

	TileSetCollection tileSets;
	NamedTileLayerCollection namedLayers;
	PropertyCollection properties;

	std::vector<ILayer*> layers;

private:
	ITileMapRenderer* m_renderer = nullptr;
};
