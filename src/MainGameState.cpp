#include <MainGameState.hpp>
#include <iostream>
#include <cmath>
#include <GameOverState.hpp>
#include <PauseMenuState.hpp>
#include <StateMachine.hpp>
#include <Player.hpp>
#include <ResourceManager.hpp>
#include <SettingsManager.hpp>

extern "C" {
    #include <raylib.h>
}

MainGameState::MainGameState()
{
}

void MainGameState::init()
{
    // Inicialización del personaje de Fuego
    fire_player = Player(100.0f, 300.0f, FIRE);
    
    // Cargar teclas desde SettingsManager
    fire_player.setKeys(
        SettingsManager::getInstance()->getFireLeft(),
        SettingsManager::getInstance()->getFireRight(),
        SettingsManager::getInstance()->getFireJump()
    );

    // Inicialización del personaje de Agua
    water_player = Player(150.0f, 300.0f, WATER);
    
    // Cargar teclas desde SettingsManager
    water_player.setKeys(
        SettingsManager::getInstance()->getWaterLeft(),
        SettingsManager::getInstance()->getWaterRight(),
        SettingsManager::getInstance()->getWaterJump()
    );

    // Inicialización de la cámara
    camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ 640, 360 };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

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
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
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

    character1 = ResourceManager::getInstance()->loadTexture("assets/characters/Wizzart_A.png");
    character2 = ResourceManager::getInstance()->loadTexture("assets/characters/Wizzart_B.png");

    fire_idle_animation = Animation(5, 0.2f, true, character1, 9, 12);
    fire_walk_animation = Animation(8, 0.1f, true, character1, 0, 8);
    
    water_idle_animation = Animation(5, 0.2f, true, character2, 9, 12);
    water_walk_animation = Animation(8, 0.1f, true, character2, 0, 8);
}

void MainGameState::handleInput()
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->add_state(std::make_unique<PauseMenuState>(), false);
        return;
    }

    // Obtener las teclas desde SettingsManager
    int fireLeft = SettingsManager::getInstance()->getFireLeft();
    int fireRight = SettingsManager::getInstance()->getFireRight();
    int fireJump = SettingsManager::getInstance()->getFireJump();
    
    int waterLeft = SettingsManager::getInstance()->getWaterLeft();
    int waterRight = SettingsManager::getInstance()->getWaterRight();
    int waterJump = SettingsManager::getInstance()->getWaterJump();

    // Control del personaje de Fuego
    if (IsKeyDown(fireLeft)) {
        fire_player.walk(-1);
    } else if (IsKeyDown(fireRight)) {
        fire_player.walk(1);
    } else {
        fire_player.walk(0);
    }

    if (IsKeyPressed(fireJump) && fire_on_ground) {
        fire_player.jump();
    }

    // Control del personaje de Agua
    if (IsKeyDown(waterLeft)) {
        water_player.walk(-1);
    } else if (IsKeyDown(waterRight)) {
        water_player.walk(1);
    } else {
        water_player.walk(0);
    }

    if (IsKeyPressed(waterJump) && water_on_ground) {
        water_player.jump();
    }
}

void MainGameState::update(float deltaTime)
{
    fire_on_ground = false;
    water_on_ground = false;

    fire_player.update(deltaTime);
    water_player.update(deltaTime);

    if (fire_player.velocity.x != 0) {
        fire_walk_animation.animationUpdate(deltaTime);
    } else {
        fire_idle_animation.animationUpdate(deltaTime);
    }
    
    if (water_player.velocity.x != 0) {
        water_walk_animation.animationUpdate(deltaTime);
    } else {
        water_idle_animation.animationUpdate(deltaTime);
    }

    for (const auto& platform : level_platforms) {
        int fire_collision = fire_player.checkPlatformCollision(platform);
        int water_collision = water_player.checkPlatformCollision(platform);

        switch (fire_collision) {
            case 0:
            case 2:
            case 3:
                if (fire_player.resolvePlatformCollision(platform)) {
                    fire_on_ground = true;
                }
                break;
            case 1:
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
        }

        switch (water_collision) {
            case 0:
            case 2:
            case 3:
                if (water_player.resolvePlatformCollision(platform)) {
                    water_on_ground = true;
                }
                break;
            case 1:
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
        }

        if (fire_collision == 3 && water_collision == 3) {
            this->state_machine->add_state(std::make_unique<GameOverState>(true), true);
        }
    }

    for (auto& enemy : level_enemies) {
        enemy.update(deltaTime);
        
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
    
    float cameraSpeed = 3.0f * deltaTime;
    camera.target.x += (midPoint.x - camera.target.x) * cameraSpeed;
    camera.target.y += (midPoint.y - camera.target.y) * cameraSpeed;
    
    camera.target.x = roundf(camera.target.x);
    camera.target.y = roundf(camera.target.y);
}

void MainGameState::render()
{
    BeginDrawing();

    BeginMode2D(camera);
    ClearBackground(RAYWHITE);
    mapLoader.draw(camera);

    float spriteScale = 1.0f;
    int frameWidth = 64;
    int frameHeight = 48;
    
    Rectangle fireSourceRect;
    if (fire_player.velocity.x != 0) {
        fireSourceRect = fire_walk_animation.animationFrame(8, frameWidth, frameHeight);
    } else {
        fireSourceRect = fire_idle_animation.animationFrame(8, frameWidth, frameHeight);
    }
    
    bool fireFlipped = fire_player.velocity.x < 0;
    if (fireFlipped) {
        fireSourceRect.width = -frameWidth;
    }
    
    DrawTexturePro(character1, 
                   fireSourceRect,
                   (Rectangle){fire_player.position.x, fire_player.position.y, frameWidth * spriteScale, frameHeight * spriteScale}, 
                   (Vector2){0, 0}, 
                   0.0f, 
                   WHITE);
    
    Rectangle waterSourceRect;
    if (water_player.velocity.x != 0) {
        waterSourceRect = water_walk_animation.animationFrame(8, frameWidth, frameHeight);
    } else {
        waterSourceRect = water_idle_animation.animationFrame(8, frameWidth, frameHeight);
    }
    
    bool waterFlipped = water_player.velocity.x < 0;
    if (waterFlipped) {
        waterSourceRect.width = -frameWidth;
    }
    
    DrawTexturePro(character2, 
                   waterSourceRect,
                   (Rectangle){water_player.position.x, water_player.position.y, frameWidth * spriteScale, frameHeight * spriteScale}, 
                   (Vector2){0, 0}, 
                   0.0f, 
                   WHITE);

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
    
    for (auto& enemy : level_enemies) {
        enemy.render();
    }
    EndMode2D();
    
    renderGlobalUI();
    
    EndDrawing();
}