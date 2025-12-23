#pragma once

#include <components/ElementType.hpp>

extern "C" {
    #include <raylib.h>
}

class Projectile {
public:
    Projectile(Vector2 pos, Vector2 vel, ElementType t);
    
    void update(float deltaTime);
    void render() const;
    
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    
    Rectangle getBounds() const { return bounds; }
    ElementType getType() const { return type; }
    
private:
    Vector2 position;
    Vector2 velocity;
    Rectangle bounds;
    ElementType type;
    bool active;
    float lifetime;
    
    static constexpr float MAX_LIFETIME = 0.4f; // 0.4 segundos (ataque corto)
    static constexpr float SIZE = 8.0f; // Tamaño del proyectil
};
