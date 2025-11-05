#include <MainGameState.hpp>
#include <iostream>


extern "C" {
    #include <raylib.h>
}

#include <Platform.hpp>

Platform::Platform() {
    bounds = {0, 0, 0, 0};
    type = NONE;
}

Platform::Platform(Rectangle b, ElementType t) : bounds(b), type(t) {
}


