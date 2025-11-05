#pragma once

#include <Entity.hpp>

class Player : public Entity {
public:
    Player();
    Player(float x, float y, ElementType t);
    
    void update(float deltaTime) override;
    void render() override;
};