#include <states/MainGameState.hpp>
#include <iostream>


extern "C" {
    #include <raylib.h>
}

#include <entities/Platform.hpp>
Platform::Platform() {
    bounds = {0, 0, 0, 0};
    type = NONE;
    id = -1;
    active = true;
    isButton = false;
    targetId = -1;
    tag = "";
    targetTag = "";
    hasVisuals = false;
    texture = {0};
    sourceRect = {0,0,0,0};
    text="";
    
    isPressed = false;
    hasPressedVisuals = false;
    pressedTexture = {0};
    pressedSourceRect = {0,0,0,0};
}

Platform::Platform(Rectangle b, ElementType t) : bounds(b), type(t) {
    id = -1;
    active = true;
    isButton = false;
    targetId = -1;
    tag = "";
    targetTag = "";
    hasVisuals = false;
    texture = {0};
    sourceRect = {0,0,0,0};
    text="";

    isPressed = false;
    hasPressedVisuals = false;
    pressedTexture = {0};
    pressedSourceRect = {0,0,0,0};
}


