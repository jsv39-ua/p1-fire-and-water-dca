#include <Enemy.hpp>

Enemy::Enemy() : Entity(), patrolRange(50.0f), originalX(0.0f), movingRight(true), hasTexture(false) {
    texture = { 0 };
    sourceRect = { 0 };
}

Enemy::Enemy(Rectangle b, ElementType t, float range) 
    : Entity({b.x, b.y}, {30.0f, 0.0f}, b.width, b.height, t),
      patrolRange(range), originalX(b.x), movingRight(true), hasTexture(false) {
    texture = { 0 };
    sourceRect = { 0 };
}

Enemy::Enemy(Rectangle b, ElementType t, float range, Texture2D tex, Rectangle src)
    : Entity({b.x, b.y}, {30.0f, 0.0f}, b.width, b.height, t),
      patrolRange(range), originalX(b.x), movingRight(true), hasTexture(true), texture(tex), sourceRect(src) {
}

void Enemy::update(float deltaTime) {
    // Lógica de patrullaje
    if (movingRight) {
        position.x += velocity.x * deltaTime;
        if (position.x >= originalX + patrolRange) {
            movingRight = false;
        }
    } else {
        position.x -= velocity.x * deltaTime;
        if (position.x <= originalX) {
            movingRight = true;
        }
    }
    
    // Actualizar bounds
    bounds.x = position.x;
    bounds.y = position.y;
}

void Enemy::render() {
    if (hasTexture) {
        // Si tiene textura, dibujarla
        // Voltear sprite si va a la izquierda
        Rectangle src = sourceRect;
        if (!movingRight) src.width = -src.width;
        
        DrawTexturePro(texture, src, bounds, {0,0}, 0.0f, WHITE);
    } else {
        // Fallback a rectángulo de color
        Color color = (type == FIRE) ? RED : BLUE;
        DrawRectangleRec(bounds, color);
    }
}