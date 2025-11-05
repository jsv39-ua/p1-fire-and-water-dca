#pragma once

#include <Entity.hpp>

class Enemy : public Entity {
public:
    float patrolRange;
    float originalX;
    bool movingRight;

    Enemy();
    Enemy(Rectangle bounds, ElementType type, float range = 50.0f);
    
    void update(float deltaTime) override;
    void render() override;
};