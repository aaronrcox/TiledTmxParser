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

	typedef std::function<void(
		const std::string textureFilename,
		int srcX, int srcY, int srcW, int srcH,
		int dstX, int dstY, int dstW, int dstH,
		unsigned int color
		)> DrawTileCb;

public:

	TileMap();
	~TileMap();

	void Load(const char* filename);
	void LoadTextures(std::function<void(std::string)> loadTextureCallback);

	void DrawLayer(ILayer *layer, DrawTileCb draw);

	TileSet *GetTileSetFromGID(unsigned int gTilesetId);

public:

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
