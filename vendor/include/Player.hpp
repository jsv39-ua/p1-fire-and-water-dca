#pragma once

#include <ElementType.hpp>
#include <vector>
#include <string>

extern "C" {
    #include <raylib.h>
}

class Player {
    public:
    Vector2 position;
    Vector2 velocity;
    Rectangle bounds;
    ElementType type;
    float width;
    float height;

    Player();
    Player(float x, float y, ElementType t);
};