#include <MainGameState.hpp>
#include <iostream>


extern "C" {
    #include <raylib.h>
}

#include <Player.hpp>

Player::Player() : width(30.0f), height(30.0f) {
    position = {0.0f, 0.0f};
    velocity = {0.0f, 0.0f};
    type = NONE;
    bounds = {position.x, position.y, width, height};
}

Player::Player(float x, float y, ElementType t) : width(30.0f), height(30.0f)  {
    position = {x, y};
    velocity = {0.0f, 0.0f};
    type = t;
    bounds = {position.x, position.y, width, height};
}