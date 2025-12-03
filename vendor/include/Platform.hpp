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
        std::string text;
        int id;
        bool active;
        bool isButton;
        int targetId;
        
        // Grouping
        std::string tag;
        std::string targetTag;
        
        // Visuals
        bool hasVisuals;
        Texture2D texture;
        Rectangle sourceRect;

        // Pressed State Visuals
        bool isPressed;
        bool hasPressedVisuals;
        Texture2D pressedTexture;
        Rectangle pressedSourceRect;

        Platform();
        Platform(Rectangle bounds, ElementType type = NONE);
        std::string getText() const { return text; }        
    
};