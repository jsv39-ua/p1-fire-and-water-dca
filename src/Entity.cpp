#include <Entity.hpp>

Entity::Entity() : width(30.0f), height(30.0f) {
    position = {0.0f, 0.0f};
    velocity = {0.0f, 0.0f};
    type = NONE;
    bounds = {position.x, position.y, width, height};
}

Entity::Entity(Vector2 pos, Vector2 vel, float w, float h, ElementType t) 
    : position(pos), velocity(vel), width(w), height(h), type(t) {
    bounds = {position.x, position.y, width, height};
}