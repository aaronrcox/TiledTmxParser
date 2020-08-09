#pragma once

#include <string>

class ILayer;

class TileMapRenderSettings
{
public:
	int viewX = 0;
	int viewY = 0;
	int viewW = 1;
	int viewH = 1;

	bool drawDebugViewLines = false;
};

class ITileMapRenderer
{
public:

	ITileMapRenderer() = default;
	virtual ~ITileMapRenderer() = default;

	void SetView(int viewX, int viewY, int viewW, int viewH) {
		settings.viewX = viewX; settings.viewY = viewY;
		settings.viewW = viewW; settings.viewH = viewH;
	}

public:

	virtual void LoadTexture(std::string filename) = 0;

	virtual void DrawTile(const std::string& textureFilename,
		int srcX, int srcY, int srcW, int srcH,
		int dstX, int dstY, int dstW, int dstH,
		unsigned int color) = 0;

	virtual void DrawRect(int dstX, int dstY, int dstW, int dstH, unsigned int color) = 0;
	virtual void DrawRectLines(int dstX, int dstY, int dstW, int dstH, int thickness, unsigned int color) = 0;

public:

	TileMapRenderSettings settings;

};