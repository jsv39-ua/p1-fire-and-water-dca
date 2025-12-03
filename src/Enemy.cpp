#include <Enemy.hpp>

Enemy::Enemy() : Entity(), patrolRange(50.0f), originalX(0.0f), movingRight(true), verticalMovement(false), originalY(0.0f), movingDown(true), hasTexture(false) {
    texture = { 0 };
    sourceRect = { 0 };
    life_count = 3;
    invincibility_timer = 0.0f;
}

Enemy::Enemy(Rectangle b, ElementType t, float range, bool vertical) 
        : Entity({b.x, b.y}, {vertical ? 0.0f : 30.0f, vertical ? 30.0f : 0.0f}, b.width, b.height, t),
            patrolRange(range), originalX(b.x), movingRight(true), verticalMovement(vertical), originalY(b.y), movingDown(true), hasTexture(false) {
    texture = { 0 };
    sourceRect = { 0 };
    life_count = 3;
    invincibility_timer = 0.0f;
}

Enemy::Enemy(Rectangle b, ElementType t, float range, Texture2D tex, Rectangle src, bool vertical)
        : Entity({b.x, b.y}, {vertical ? 0.0f : 30.0f, vertical ? 30.0f : 0.0f}, b.width, b.height, t),
            patrolRange(range), originalX(b.x), movingRight(true), verticalMovement(vertical), originalY(b.y), movingDown(true), texture(tex), sourceRect(src), hasTexture(true) {
    life_count = 3;
    invincibility_timer = 0.0f;
}

void Enemy::update(float deltaTime) {
    // Actualizar timer de invencibilidad
    if (invincibility_timer > 0.0f) {
        invincibility_timer -= deltaTime;
        if (invincibility_timer < 0.0f) {
            invincibility_timer = 0.0f;
        }
    }
    
    // Lógica de patrullaje (horizontal o vertical)
    if (!verticalMovement) {
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
    } else {
        if (movingDown) {
            position.y += velocity.y * deltaTime;
            if (position.y >= originalY + patrolRange) {
                movingDown = false;
            }
        } else {
            position.y -= velocity.y * deltaTime;
            if (position.y <= originalY) {
                movingDown = true;
            }
        }
    }
    
    // Actualizar bounds
    bounds.x = position.x;
    bounds.y = position.y;
}

void Enemy::render() {
    // Solo renderizar si debe ser visible (parpadeo durante invencibilidad)
    if (!shouldRenderVisible()) {
        return;
    }
    
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

void Enemy::updateLifeCount(int change) {
    life_count += change;
    
    if (life_count < 0) {
        life_count = 0;
    }
    
    // Activar invencibilidad cuando pierde vida
    if (change < 0) {
        activateInvincibility();
    }
}

bool Enemy::shouldRenderVisible() const {
    if (!isInvincible()) {
        return true; // Siempre visible si no está invencible
    }
    
    // Usar el tiempo del juego para un parpadeo consistente
    float currentTime = GetTime();
    int blinkCycle = (int)(currentTime / BLINK_INTERVAL);
    return (blinkCycle % 2) == 0; // Alterna entre visible/invisible
}