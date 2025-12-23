#include <entities/Projectile.hpp>
extern "C" {
    #include <raylib.h>
}

Projectile::Projectile(Vector2 pos, Vector2 vel, ElementType t) 
    : position(pos), velocity(vel), type(t), active(true), lifetime(0.0f) {
    bounds = {position.x, position.y, SIZE, SIZE};
}

void Projectile::update(float deltaTime) {
    if (!active) return;
    
    // Actualizar posición
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    
    // Actualizar bounds
    bounds.x = position.x;
    bounds.y = position.y;
    
    // Actualizar tiempo de vida
    lifetime += deltaTime;
    if (lifetime >= MAX_LIFETIME) {
        active = false;
    }
}

void Projectile::render() const {
    if (!active) return;
    
    Color color;
    switch(type) {
        case FIRE:
            color = ORANGE; // Naranja para fuego
            break;
        case WATER:
            color = SKYBLUE; // Azul claro para agua
            break;
        default:
            color = WHITE;
            break;
    }
    
    // Dibujar como círculo pequeño
    DrawCircleV(Vector2{position.x + SIZE/2, position.y + SIZE/2}, SIZE/2, color);
    
    // Borde para mejor visibilidad
    DrawCircleLines(position.x + SIZE/2, position.y + SIZE/2, SIZE/2, DARKGRAY);
}
