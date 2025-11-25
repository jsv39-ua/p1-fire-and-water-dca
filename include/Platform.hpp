#pragma once

#include <ElementType.hpp>
#include <vector>
#include <string>

extern "C" {
    #include <raylib.h>
}

class Platform {
    public:
    Rectangle bounds;
    ElementType type;

    Platform();
    Platform(Rectangle bounds, ElementType type = NONE);
};