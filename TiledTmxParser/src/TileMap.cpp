#include "TileMap.h"
#include "TiledMapParser.h"
#include "Layer.h"

TileMap::TileMap()
{

}

TileMap::~TileMap()
{

}

void TileMap::Load(const char* filename)
{
	TiledMapParser parser;
	parser.LoadFromFile(filename, this);
}

void TileMap::LoadTextures(std::function<void(std::string)> loadTextureCallback)
{
	for (auto iter : tileSets)
	{
		loadTextureCallback(iter.second.imageFileName.c_str());
	}
}

void TileMap::Unload()
{

}

TileSet *TileMap::GetTileSetFromGID(unsigned int gTilesetId)
{
	for (auto& iter : tileSets)
	{
		auto& tileset = iter.second;
		int maxId = tileset.firstGlobalTileId + tileset.tileCount;

		if (gTilesetId >= tileset.firstGlobalTileId && gTilesetId < maxId)
			return &(iter.second);
	}

	return nullptr;
}

//unsigned int TileMap::LayerCount()
//{
//	return namedLayers.size();
//}
//Layer* TileMap::GetLayerById(unsigned int id)
//{
//	for (auto& iter : namedLayers)
//	{
//		if (iter.second.id == id)
//			return &(iter.second);
//	}
//
//	return nullptr;
//}
//Layer* TileMap::GetLayerByName(const std::string& name)
//{
//	auto iter = namedLayers.find(name);
//	if (iter == namedLayers.end())
//		return nullptr;
//
//	return &(iter->second);
//}