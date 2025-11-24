#include <MainGameState.hpp>
#include <PauseMenuState.hpp>
#include <GameOverState.hpp>
#include <AudioManager.hpp>
#include <iostream>

extern "C" {
    #include <raylib.h>
}

// Inicializar los players estáticos globales con teclas por defecto
Player MainGameState::static_fire_player = Player(0, 0, FIRE);
Player MainGameState::static_water_player = Player(0, 0, WATER);

MainGameState::MainGameState()
{
}

void MainGameState::init()
{
    // Inicialización del personaje de Fuego
    fire_player = Player(100.0f, 300.0f, FIRE);

    // Inicialización del personaje de Agua
    water_player = Player(150.0f, 300.0f, WATER);
    
    // Copiar las teclas desde los players estáticos a los de la partida
    fire_player.setKeys(
        static_fire_player.getKeyLeft(),
        static_fire_player.getKeyRight(),
        static_fire_player.getKeyJump()
    );
    
    water_player.setKeys(
        static_water_player.getKeyLeft(),
        static_water_player.getKeyRight(),
        static_water_player.getKeyJump()
    );

    // Inicialización de la cámara
    camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ 640, 360 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Inicialización del MapLoader
    try {
        mapLoader.loadMapa("assets/levels/level_prueba/nivel1-pantalla2.tmx");
        
        std::vector<Rectangle> collisions = mapLoader.extractCollisions();
        
        for (const auto& collision : collisions) {
            Platform platform = { collision, NONE };
            level_platforms.push_back(platform);
        }
        
        TraceLog(LOG_INFO, "Plataformas cargadas desde Tiled: %d", level_platforms.size());
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error al cargar el mapa: " << e.what() << std::endl;
    }

    Platform goal = Platform({ 400, 390, 100, 50 }, GOAL);
    level_platforms.push_back(goal);

    Platform lava = Platform({ 200, 350, 100, 50 }, FIRE);
    level_platforms.push_back(lava);

    Platform water = Platform({ 550, 350, 100, 50 }, WATER);
    level_platforms.push_back(water);
    
    Enemy fire_enemy = { { 450, 270, 25, 25 }, FIRE, 60.0f };
    level_enemies.push_back(fire_enemy);
    
    Enemy water_enemy = { { 650, 370, 25, 25 }, WATER, 70.0f };
    level_enemies.push_back(water_enemy);
}

void MainGameState::handleInput()
{
    // Abrir menú de pausa con Escape
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->add_state(std::make_unique<PauseMenuState>(fire_player, water_player), true);
        return; // No procesar más input si se abre el menú de pausa
    }

    // Cada jugador maneja su propio input
    fire_player.handleInput(fire_on_ground);
    water_player.handleInput(water_on_ground);
}

void MainGameState::update(float deltaTime)
{
    // Resetea el estado de "en el suelo" al principio del frame
    fire_on_ground = false;
    water_on_ground = false;

    // Actualizar jugadores (la gravedad se aplica dentro de Player::update)
    fire_player.update(deltaTime);
    water_player.update(deltaTime);

    // Colisiones con plataformas
    for (const auto& platform : level_platforms) {
        int fire_collision = fire_player.checkPlatformCollision(platform);
        int water_collision = water_player.checkPlatformCollision(platform);

        // Procesar colisión del personaje de Fuego
        switch (fire_collision) {
            case 0: // Plataforma neutra - comportamiento normal
            case 2: // Plataforma amiga (Fire en plataforma FIRE)
            case 3: // Meta (también es sólida)
                if (fire_player.resolvePlatformCollision(platform)) {
                    fire_on_ground = true;
                }
                break;
            case 1: // Plataforma contraria - Game Over
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
        }

        // Procesar colisión del personaje de Agua
        switch (water_collision) {
            case 0: // Plataforma neutra - comportamiento normal
            case 2: // Plataforma amiga (Water en plataforma WATER)
            case 3: // Meta (también es sólida)
                if (water_player.resolvePlatformCollision(platform)) {
                    water_on_ground = true;
                }
                break;
            case 1: // Plataforma contraria - Game Over
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
        }

        // Detecta si ambos jugadores están en la meta
        if (fire_collision == 3 && water_collision == 3) {
            this->state_machine->add_state(std::make_unique<GameOverState>(true), true);
        }
    }

    // Actualizar enemigos con patrullaje
    for (auto& enemy : level_enemies) {
        enemy.update(deltaTime);
        
        // Colisiones con enemigos
        if (CheckCollisionRecs(fire_player.bounds, enemy.bounds)) {
            if (enemy.type == WATER) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
            }
        }
        
        if (CheckCollisionRecs(water_player.bounds, enemy.bounds)) {
            if (enemy.type == FIRE) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
            }
        }
    }

    Vector2 midPoint = {
        (fire_player.position.x + water_player.position.x) / 2.0f,
        (fire_player.position.y + water_player.position.y) / 2.0f
    };
    
    // Suavizado de movimiento de cámara (interpolación)
    float cameraSpeed = 3.0f * deltaTime;
    camera.target.x += (midPoint.x - camera.target.x) * cameraSpeed;
    camera.target.y += (midPoint.y - camera.target.y) * cameraSpeed;
}

void MainGameState::render()
{
    BeginDrawing();

    BeginMode2D(camera);
    ClearBackground(RAYWHITE);
    mapLoader.draw(camera);

    // Renderizar jugadores
    fire_player.render();
    water_player.render();

    // Dibujar las plataformas del nivel
    for (const auto& platform : level_platforms) {
        if(platform.type == GOAL){
            DrawRectangleRec(platform.bounds, YELLOW);
        }
        else if(platform.type == FIRE){
            DrawRectangleRec(platform.bounds, MAROON);
        }
        else if(platform.type == WATER){
            DrawRectangleRec(platform.bounds, DARKBLUE);
        }
    }
    
    // Renderizar enemigos
    for (auto& enemy : level_enemies) {
        enemy.render();
    }
    EndMode2D();
    
    // Renderizar UI global (en coordenadas de pantalla, no del mundo)
    renderGlobalUI();
    
    EndDrawing();
}
