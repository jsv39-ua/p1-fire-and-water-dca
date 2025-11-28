#pragma once

#include <Entity.hpp>

class Enemy : public Entity {
public:
    float patrolRange;
    float originalX;
    bool movingRight;
    
    // Textura y frame del enemigo
    Texture2D texture;
    Rectangle sourceRect;
    bool hasTexture;

    Enemy();
    Enemy(Rectangle bounds, ElementType type, float range = 50.0f);
    // Constructor con textura
    Enemy(Rectangle bounds, ElementType type, float range, Texture2D tex, Rectangle src);
    
    void update(float deltaTime) override;
    void render() override;
};