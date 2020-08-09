
#include "TileMapRenderer.h"


TileMapRenderer::TileMapRenderer()
{

}

TileMapRenderer::~TileMapRenderer()
{
	for (auto& kvp : m_textureLookup)
	{
		::UnloadTexture(kvp.second);
	}
	m_textureLookup.clear();
}

void TileMapRenderer::LoadTexture(std::string filename)
{
	m_textureLookup[filename] = ::LoadTexture(filename.c_str());
}

void TileMapRenderer::DrawTile(const std::string & textureFilename,
	int srcX, int srcY, int srcW, int srcH,
	int dstX, int dstY, int dstW, int dstH,
	unsigned int color)
{
	Rectangle src = { srcX, srcY, srcW, srcH };
	Rectangle dst = { dstX, dstY, dstW, dstH };

	// convert unsigned int to Raylib color struct
	Color tintColor = *((Color*)(&color));

	// get thexture from our lookup table
	auto& tex = m_textureLookup[textureFilename];

	// draw the tile via raylib
	DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, tintColor);
}

void TileMapRenderer::DrawRect(int dstX, int dstY, int dstW, int dstH, unsigned int color)
{
	::DrawRectangle(dstX, dstY, dstW, dstH , *((Color*)(&color)));
}

void TileMapRenderer::DrawRectLines(int dstX, int dstY, int dstW, int dstH, int thickness, unsigned int color)
{
	::DrawRectangleLinesEx({ (float)dstX, (float)dstY, (float)dstW, (float)dstH }, thickness, *((Color*)(&color)));
}