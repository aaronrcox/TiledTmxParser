# Tiled TMX File Parser
This project has been created to parse Tiled maps *.tmx for use in C++ projects.
The *.tmx format has been very clearly documentd:  [MapEditor Documentation](https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#)

The program used to generate the tile maps is called Tiled, which is available at: [https://www.mapeditor.org/](https://www.mapeditor.org/)

## Goals:
 - No dependencies
 - Usable accross multiple graphics libraries,  provide a rendering interface that can be implemented for use in any renderer. OpenGL, SDL, SFML, Raylib etc... An example is provided using Raylib.
 - Currently a learning / support project to assist my students with creating interesting games in C++

## Compile Instructions

1. Clone the repo
2. Open `TiledTmxParser.sln` with Visual Studio 2019
3. Build the project for Build/Release - Win32/x64

`TiledTxmParser.lib` will be saved to the following location:
`$(ProjectDir)\bin\$(Platform)\$(Configuration)\`

The lib and include files need to be included within your own project.
Take a look at the included sample project for settings:
 - `additional include directories`
 - `additional library directories`
 - `additional dependencies`


## Code Examples / Usage
The only include file nescesary is `TileMap.h`
``` (C++)
#include "TileMap.h"
```

**To load a tilemap:**
```
TileMap map;
map.Load("./assets/mytilemap.tmx");
```

We can get `TileLayer`s and `ObjectLayers` by their assigned name.
These methods are usefull for querying map data and aquireing paths, or spawn points
```
TileLayer& groundLayer = map->GetTileLayer("ground"); // Get a TileLayer by its name
TileState& tileState = groundLayer.GetTileData(10, 10); // Tile info x/y on a layer.
```
```
auto levelInfo = map->GetObjectGroup("LevelInfo");
auto playerSpawn = levelInfo->GetObjectByName("PlayerSpawn");
```

**Drawing the tilemap**
This library has no knowlage of any rendering library of framework.  To achieve rendering, we have provided a concrete implementation of the `ITileMapRenderer` class for the renderer of your choice.

The example project showcases a Concrete implementation for the Raylib game framework.
```
TileMap map;
map.Load("./assets/test.tmx");
map.SetRenderer(new TileMapRenderer());
```
After a renderer has been set, we draw the map. Iterate over each layer to draw them in order.
```
// Setting view port enables renderer to only draw tiles that are visible.
map.GetRenderer()->SetView(view.x, view.y, view.width, view.height);
	
// loop through each layer - invoke draw method
for (int i = 0; i < m_map->layers.size(); i++)
{
	m_map->DrawLayer(m_map->layers[i]);
}
```

## Screenshots

![Tiled Map Editor](/docs/screenshots/TiledMapEditor.png "Tiled Map Editor")

![Raylib Rendered TMX](/docs/screenshots/raylib_tmx_demo.png "Raylib Rendered TMX")


## Credits
This project includes the following open source tools / project:

 - **TinyXML2:** An XML File parsing library <br> 	[https://github.com/leethomason/tinyxml2](https://github.com/leethomason/tinyxml2)

 - **Miniz:** A single file zlib compression/decompression library <br> [https://code.google.com/archive/p/miniz/](https://code.google.com/archive/p/miniz/)

- **Basen:** A Base64 encoding / decoding library <br> [https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/](https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/) 

 - **Raylib:** A C++ Game Programming framework, used for this demo. <br> [https://www.raylib.com](https://www.raylib.com)

 - **Kenny Topdown-shooter:** Tileset collection used in the demo project <br> [https://www.kenney.nl/assets/topdown-shooter](https://www.kenney.nl/assets/topdown-shooter)

