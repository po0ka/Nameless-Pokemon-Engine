#pragma once

#include "stdafx.h"
//#include "ScriptableObject.h"
#include "graphics.h"

#include <vector>

//class ScriptableObject;

class Tile
{
public:
	int ID;
	int CollisionData;
};

class Encounters
{
public:
	int *encounters;
	int *minLevel;
	int *maxLevel;

	int size;
};

class TileMap
{
public:
	~TileMap();

	void LoadMap( const char *fileName );
	void LoadMapAdjacent( const char *fileName );
	void LoadTileImage( const char *path, const char *fileName2 );
	void RenderMap();
	void RenderBG();

	void RenderPriorityTiles();

	void SetCamera( int x, int y ){ camX = x; camY = y; };

	int GetCollision( int x, int y )
	{
		if( x < MemoryX && y < MemoryY && x >= 0 && y >= 0 )
		{
			return Tiles[x][y].CollisionData;
		}
		else
			return 1;
	}
	void SetTileType( int x, int y, int tileNum ){ Tiles[x][y].ID = tileNum; };
	void SetCollision( int x, int y, int collision ) { Tiles[x][y].CollisionData = collision; };

	void SetLayerTileType( int x, int y, int tileNum ){ LayeredTiles[x][y].ID = tileNum; };

	void SaveMap();

	int MemoryX, MemoryY;
	bool debug;

	const char *TilePath;
	//std::vector<ScriptableObject*> MapObjects;

	Encounters encounters;

private:
	Tile **Tiles;
	Tile **LayeredTiles;

	int spacingX;
	int spacingY;

	SDL_Texture *m_Texture;
	SDL_Texture *m_PriorityTexture;

	int camX, camY;
};
