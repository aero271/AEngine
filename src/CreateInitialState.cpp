#include "CreateInitialState.hpp"
#include "Item.hpp"
#include "ALight.hpp"
#include "Water.hpp"

#include "Items/Player.hpp"
#include "Items/Boat.hpp"
#include "Items/Player.hpp"

#include "raylib.h"
#include "raymath.h"

#include <string>
#include <iostream>

#include <boost/format.hpp>
using boost::format;

#include "AItems.hpp"


void createInitialState(AItems* items, Player** player)
{
    *player = new Player(items, {0.0, 0.0, 0.0});
}