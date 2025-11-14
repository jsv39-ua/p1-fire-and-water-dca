#include <MainGameState.hpp>
#include <iostream>


extern "C" {
    #include <raylib.h>
}

#include <Player.hpp>

Player::Player() : Entity() {
}

Player::Player(float x, float y, ElementType t) 
    : Entity({x, y}, {0.0f, 0.0f}, 30.0f, 30.0f, t) {
}

void Player::update(float deltaTime) {
    // Aplicar gravedad y actualizar posición basada en velocidad
    static const float gravity = 800.0f;
    velocity.y += gravity * deltaTime;

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    // Actualizar bounds
    bounds.x = position.x;
    bounds.y = position.y;
}

void Player::render() {
    Color color = (type == FIRE) ? RED : BLUE;
    DrawRectangleRec(bounds, color);
}

void Player::walk(int direction) {
    // direction: -1 = izquierda, 0 = parar, 1 = derecha
    if (direction == -1) {
        velocity.x = -WALK_SPEED;
    } else if (direction == 1) {
        velocity.x = WALK_SPEED;
    } else {
        velocity.x = 0.0f;
    }
}

void Player::jump() {
    velocity.y = -JUMP_FORCE;
}

/*
    Resuelve la física de colisión con una plataforma sólida.
    Retorna true si el jugador está sobre la plataforma (en el suelo).
*/
bool Player::resolvePlatformCollision(const Platform & platform) {
    // Solo corregir si viene desde arriba (cayendo)
    if (this->velocity.y > 0) {
        float playerBottom = this->position.y + this->height;
        // Verificar que el jugador está encima de la plataforma (no dentro por los lados)
        if (playerBottom > platform.bounds.y && playerBottom < platform.bounds.y + platform.bounds.height / 2) {
            this->velocity.y = 0;
            this->position.y = platform.bounds.y - this->bounds.height;
            this->bounds.y = this->position.y;
            return true; // Está en el suelo
        }
    }
    return false; // No está en el suelo
}


/*
    Los valores que se interpretan de la colisión son:
     - 0: Colisión con plataforma neutra
     - 1: Colisión con plataforma de contraria
     - 2: Colisión con plataforma amiga
     - 3: Colsión con plataforma meta
     - 4: No hay colisión
     - -1: Error
*/
int Player::checkPlatformCollision(const Platform & platform) const {

    bool collisionX = this->bounds.x + this->bounds.width >= platform.bounds.x &&
                      platform.bounds.x + platform.bounds.width >= this->bounds.x;
    bool collisionY = this->bounds.y + this->bounds.height >= platform.bounds.y &&
                      platform.bounds.y + platform.bounds.height >= this->bounds.y;

    if(collisionX && collisionY){
        switch (platform.type) {
            case NONE:
                return 0;
            case GOAL:
                return 3;
            case FIRE:
                return (this->type == FIRE) ? 2 : 1;
            case WATER:
                return (this->type == WATER) ? 2 : 1;
            default:
                return -1; // Será una plataforma con tipo inválido
        }
    }


    return 4;
}