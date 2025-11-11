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