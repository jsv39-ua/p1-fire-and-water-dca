#pragma once

#include <ElementType.hpp>

extern "C" {
    #include <raylib.h>
}

class Entity {
public:
    Vector2 position;
    Vector2 velocity;
    Rectangle bounds;
    ElementType type;
    float width;
    float height;
    bool facingRight; // true = derecha, false = izquierda

    Entity();
    Entity(Vector2 pos, Vector2 vel, float w, float h, ElementType t);
    
    virtual ~Entity() = default;
    
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
};