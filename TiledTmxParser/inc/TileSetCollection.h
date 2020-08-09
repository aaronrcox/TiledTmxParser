#pragma once

#include <string>
#include <map>

#include "TileSet.h"

typedef std::map<std::string, TileSet> TileSetCollection;

// TODO: create custom container
//  - should allow for lookup by string (name)
//  - should allow for lookup by index (order in collection)

