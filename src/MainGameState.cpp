#include <MainGameState.hpp>
#include <iostream>
#include <GameOverState.hpp> // Incluye la nueva clase
#include <StateMachine.hpp> // Asegúrate de incluirla


extern "C" {
    #include <raylib.h>
}

MainGameState::MainGameState()
{
}

void MainGameState::init()
{
    // Inicialización del personaje de Fuego
    fire_player.position = { 100.0f, 300.0f };
    fire_player.velocity = { 0.0f, 0.0f };
    fire_player.width = 30.0f;
    fire_player.height = 30.0f;
    fire_player.bounds = { fire_player.position.x, fire_player.position.y, fire_player.width, fire_player.height };
    fire_player.type = FIRE;

    // Inicialización del personaje de Agua
    water_player.position = { 150.0f, 300.0f };
    water_player.velocity = { 0.0f, 0.0f };
    water_player.width = 30.0f;
    water_player.height = 30.0f;
    water_player.bounds = { water_player.position.x, water_player.position.y, water_player.width, water_player.height };
    water_player.type = WATER;

    // Creación de plataformas para el nivel de prueba
    Platform ground = { { 0, 400, 800, 50 } };
    level_platforms.push_back(ground);
    
    Platform p1 = { { 300, 300, 200, 20 } };
    level_platforms.push_back(p1);

    // Creación de peligros para el nivel de prueba
    Hazard lava = { { 200, 350, 100, 50 }, FIRE };
    level_hazards.push_back(lava);

    Hazard water = { { 550, 350, 100, 50 }, WATER };
    level_hazards.push_back(water);
    
    // Crear enemigos que se mueven (al final de la pantalla)
    // Enemigo de fuego en la plataforma superior (lado derecho)
    Enemy fire_enemy = { { 450, 270, 25, 25 }, FIRE, 60.0f };
    level_enemies.push_back(fire_enemy);
    
    // Enemigo de agua cerca del suelo (lado derecho)
    Enemy water_enemy = { { 650, 370, 25, 25 }, WATER, 70.0f };
    level_enemies.push_back(water_enemy);
}

void MainGameState::handleInput()
{
    // Control del personaje de Fuego (teclas A y D)
    if (IsKeyDown(KEY_A)) {
        fire_player.velocity.x = -200.0f;
    } else if (IsKeyDown(KEY_D)) {
        fire_player.velocity.x = 200.0f;
    } else {
        fire_player.velocity.x = 0.0f;
    }

    // Control del personaje de Agua (flechas izquierda y derecha)
    if (IsKeyDown(KEY_LEFT)) {
        water_player.velocity.x = -200.0f;
    } else if (IsKeyDown(KEY_RIGHT)) {
        water_player.velocity.x = 200.0f;
    } else {
        water_player.velocity.x = 0.0f;
    }
}

void MainGameState::update(float deltaTime)
{
    // Resetea el estado de "en el suelo" al principio del frame
    fire_on_ground = false;
    water_on_ground = false;

    // Aplica gravedad a los jugadores
    const float gravity = 800.0f;
    fire_player.velocity.y += gravity * deltaTime;
    water_player.velocity.y += gravity * deltaTime;

    // Actualiza las posiciones de los jugadores según su velocidad
    fire_player.position.x += fire_player.velocity.x * deltaTime;
    fire_player.position.y += fire_player.velocity.y * deltaTime;
    water_player.position.x += water_player.velocity.x * deltaTime;
    water_player.position.y += water_player.velocity.y * deltaTime;

    // Actualiza los límites (bounds) de colisión
    fire_player.bounds.x = fire_player.position.x;
    fire_player.bounds.y = fire_player.position.y;
    water_player.bounds.x = water_player.position.x;
    water_player.bounds.y = water_player.position.y;

    // Colisiones con plataformas
    for (const auto& platform : level_platforms) {
        // Revisa colisión con el personaje de Fuego
        if (CheckCollisionRecs(fire_player.bounds, platform.bounds)) {
            // Detiene el movimiento vertical
            fire_player.velocity.y = 0;
            // Corrige la posición del jugador para que no caiga
            fire_player.position.y = platform.bounds.y - fire_player.bounds.height;
            fire_player.bounds.y = fire_player.position.y;
            fire_on_ground = true; // El jugador está en el suelo
        }
        // Revisa colisión con el personaje de Agua
        if (CheckCollisionRecs(water_player.bounds, platform.bounds)) {
            // Detiene el movimiento vertical
            water_player.velocity.y = 0;
            // Corrige la posición del jugador
            water_player.position.y = platform.bounds.y - water_player.bounds.height;
            water_player.bounds.y = water_player.position.y;
            water_on_ground = true; // El jugador está en el suelo
        }
    }

    // Lógica de Salto
    if (IsKeyPressed(KEY_W) && fire_on_ground) {
        fire_player.velocity.y = -400.0f; // Impulso de salto
    }
    if (IsKeyPressed(KEY_UP) && water_on_ground) {
        water_player.velocity.y = -400.0f; // Impulso de salto
    }

    // Actualizar enemigos (movimiento de patrulla)
    for (auto& enemy : level_enemies) {
        if (enemy.movingRight) {
            enemy.bounds.x += enemy.velocity * deltaTime;
            if (enemy.bounds.x >= enemy.originalX + enemy.patrolRange) {
                enemy.movingRight = false;
            }
        } else {
            enemy.bounds.x -= enemy.velocity * deltaTime;
            if (enemy.bounds.x <= enemy.originalX) {
                enemy.movingRight = true;
            }
        }
    }
    
    // Lógica de colisión con peligros estáticos
    for (const auto& hazard : level_hazards) {
        if (CheckCollisionRecs(fire_player.bounds, hazard.bounds)) {
            if (hazard.type == WATER) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
            }
        }
        
        if (CheckCollisionRecs(water_player.bounds, hazard.bounds)) {
            if (hazard.type == FIRE) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
            }
        }
    }
    
    // Lógica de colisión con enemigos
    for (const auto& enemy : level_enemies) {
        if (CheckCollisionRecs(fire_player.bounds, enemy.bounds)) {
            if (enemy.type == WATER) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
            }
        }
        
        if (CheckCollisionRecs(water_player.bounds, enemy.bounds)) {
            if (enemy.type == FIRE) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
            }
        }
    }
}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Dibujar el personaje de Fuego
    DrawRectangleRec(fire_player.bounds, RED);
    
    // Dibujar el personaje de Agua
    DrawRectangleRec(water_player.bounds, BLUE);

    // Dibujar las plataformas del nivel
    for (const auto& platform : level_platforms) {
        DrawRectangleRec(platform.bounds, GRAY);
    }

    // Dibuja los peligros estáticos
    for (const auto& hazard : level_hazards) {
        if (hazard.type == FIRE) {
            DrawRectangleRec(hazard.bounds, MAROON);
        } else if (hazard.type == WATER) {
            DrawRectangleRec(hazard.bounds, DARKBLUE);
        }
    }
    
    // Dibuja los enemigos
    for (const auto& enemy : level_enemies) {
        if (enemy.type == FIRE) {
            DrawRectangleRec(enemy.bounds, RED);
        } else if (enemy.type == WATER) {
            DrawRectangleRec(enemy.bounds, BLUE);
        }
    }
    
    EndDrawing();
}