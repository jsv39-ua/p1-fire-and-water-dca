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
    // Actualizar posición basada en velocidad
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