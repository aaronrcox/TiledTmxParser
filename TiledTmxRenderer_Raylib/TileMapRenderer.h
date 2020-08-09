#pragma once

#include "ITileMapRenderer.h"
#include <raylib.h>
#include <map>
#include <string>

class TileMapRenderer : public ITileMapRenderer
{
public:

	TileMapRenderer();
	virtual ~TileMapRenderer();

	virtual void LoadTexture(std::string filename);

	virtual void DrawTile(const std::string & textureFilename,
		int srcX, int srcY, int srcW, int srcH,
		int dstX, int dstY, int dstW, int dstH,
		unsigned int color);

	virtual void DrawRect(int dstX, int dstY, int dstW, int dstH, unsigned int color);
	virtual void DrawRectLines(int dstX, int dstY, int dstW, int dstH, int thickness, unsigned int color);

protected:

	std::map<std::string, Texture2D> m_textureLookup; // filename to texture

private:
};