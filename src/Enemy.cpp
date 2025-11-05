#include <Enemy.hpp>

Enemy::Enemy() : Entity(), patrolRange(50.0f), originalX(0.0f), movingRight(true) {
}

Enemy::Enemy(Rectangle b, ElementType t, float range) 
    : Entity({b.x, b.y}, {30.0f, 0.0f}, b.width, b.height, t),
      patrolRange(range), originalX(b.x), movingRight(true) {
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
    Color color = (type == FIRE) ? RED : BLUE;
    DrawRectangleRec(bounds, color);
}