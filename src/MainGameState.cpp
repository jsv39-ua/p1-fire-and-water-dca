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
    // Cargar texturas primero para obtener sus dimensiones
    character1 = ResourceManager::getInstance()->loadTexture("assets/characters/Wizzart_A.png");
    character2 = ResourceManager::getInstance()->loadTexture("assets/characters/Wizzart_C.png");
    
    // Calcular dimensiones de cada frame del spritesheet
    int columns = 8; // 8 columnas en el spritesheet
    int rows = 4;    // 4 filas en el spritesheet
    float frameWidth = character1.width / (float)columns;
    float frameHeight = character1.height / (float)rows;
    
    // Ajustar el tamaño de la hitbox (aproximadamente la mitad del ancho del frame)
    float hitboxWidth = frameWidth * 0.5f;
    float hitboxHeight = frameHeight;
   
    // Inicialización del personaje de Fuego con tamaño basado en el asset
    fire_player = Player(100.0f, 300.0f, hitboxWidth, hitboxHeight, FIRE);
    // Inicialización del personaje de Fuego
    fire_player = Player(100.0f, 300.0f, hitboxWidth, hitboxHeight, FIRE);
    
    // Cargar teclas desde SettingsManager
    fire_player.setKeys(
        SettingsManager::getInstance()->getFireLeft(),
        SettingsManager::getInstance()->getFireRight(),
        SettingsManager::getInstance()->getFireJump()
    );

    // Inicialización del personaje de Agua con tamaño basado en el asset
    water_player = Player(150.0f, 300.0f, hitboxWidth, hitboxHeight, WATER);
    
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

        level_enemies = mapLoader.extractEnemies();
        
        TraceLog(LOG_INFO, "Plataformas cargadas desde Tiled: %d", level_platforms.size());
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error al cargar el mapa: " << e.what() << std::endl;
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }

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

    // TECLA DE PRUEBA: pulsar '1' fuerza Game Over (útil para testing)
    if (IsKeyPressed(KEY_ONE)) {
        this->state_machine->add_state(std::make_unique<GameOverState>(), true);
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
    
    int fireJump = SettingsManager::getInstance()->getFireJump();
    int waterJump = SettingsManager::getInstance()->getWaterJump();
    
    fire_player.updateJump(deltaTime, IsKeyDown(fireJump));
    water_player.updateJump(deltaTime, IsKeyDown(waterJump));

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
    
    
    Rectangle mapBounds = mapLoader.getMapBounds();
    
    // Calcular los límites de la cámara basados en el tamaño de la pantalla y el zoom
    float screenWidth = 1280.0f;  
    float screenHeight = 720.0f;
    
    // Calcular el área visible de la cámara en el mundo (dividido por el zoom)
    float visibleWidth = screenWidth / camera.zoom;
    float visibleHeight = screenHeight / camera.zoom;
    
    // Calcular los límites mínimos y máximos para el target de la cámara
    float minX = mapBounds.x + visibleWidth / 2.0f;
    float maxX = mapBounds.x + mapBounds.width - visibleWidth / 2.0f;
    float minY = mapBounds.y + visibleHeight / 2.0f;
    float maxY = mapBounds.y + mapBounds.height - visibleHeight / 2.0f;
    
    // Aplicar los límites a la posición de la cámara
    if (mapBounds.width > visibleWidth) {
        camera.target.x = fmaxf(minX, fminf(camera.target.x, maxX));
    } else {
        // Si el mapa es más pequeño que la pantalla, centrar en el mapa
        camera.target.x = mapBounds.x + mapBounds.width / 2.0f;
    }
    
    if (mapBounds.height > visibleHeight) {
        camera.target.y = fmaxf(minY, fminf(camera.target.y, maxY));
    } else {
        // Si el mapa es más pequeño que la pantalla, centrar en el mapa
        camera.target.y = mapBounds.y + mapBounds.height / 2.0f;
    }
    
    camera.target.x = roundf(camera.target.x);
    camera.target.y = roundf(camera.target.y);
}

void MainGameState::render()
{
    BeginDrawing();

    BeginMode2D(camera);
    ClearBackground(RAYWHITE);
    mapLoader.draw(camera);

    // Renderizar jugadores con animaciones
    float spriteScale = 1.5f;  
    int frameWidth = 64;  // Ancho de cada frame en el spritesheet (512/8)
    int frameHeight = 48; // Alto de cada frame en el spritesheet (192/4)
    
    
    float renderWidth = frameWidth * spriteScale;
    float renderHeight = frameHeight * spriteScale;
    
    
    Rectangle fireSourceRect;
    if (fire_player.velocity.x != 0) {
        
        fireSourceRect = fire_walk_animation.animationFrame(8, frameWidth, frameHeight);
        fire_player.facingRight = fire_player.velocity.x > 0;
    } else {
        fireSourceRect = fire_idle_animation.animationFrame(8, frameWidth, frameHeight);
    }
    
    if (!fire_player.facingRight) {
        fireSourceRect.width = -frameWidth;
    }
    
    float fireRenderX = fire_player.position.x + (fire_player.width / 2.0f) - (renderWidth / 2.0f);
    float fireRenderY = fire_player.position.y + fire_player.height - renderHeight;
    
    DrawTexturePro(character1, 
                   fireSourceRect,  
                   (Rectangle){fireRenderX, fireRenderY, renderWidth, renderHeight}, 
                   (Vector2){0, 0}, 
                   0.0f, 
                   WHITE);
    
    
    Rectangle waterSourceRect;
    if (water_player.velocity.x != 0) {
        waterSourceRect = water_walk_animation.animationFrame(8, frameWidth, frameHeight);
      
        water_player.facingRight = water_player.velocity.x > 0;
    } else {
        waterSourceRect = water_idle_animation.animationFrame(8, frameWidth, frameHeight);
    }
    
    
    if (!water_player.facingRight) {
        waterSourceRect.width = -frameWidth;
    }
    
    float waterRenderX = water_player.position.x + (water_player.width / 2.0f) - (renderWidth / 2.0f);
    float waterRenderY = water_player.position.y + water_player.height - renderHeight;
    
    DrawTexturePro(character2, 
                   waterSourceRect,
                   (Rectangle){waterRenderX, waterRenderY, renderWidth, renderHeight}, 
                   (Vector2){0, 0}, 
                   0.0f, 
                   WHITE);

    // Debug
    DrawRectangleLinesEx(fire_player.bounds, 2, GREEN);
    DrawRectangleLinesEx(water_player.bounds, 2, BLUE);

    

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
    
    for (auto& enemy : level_enemies) {
        enemy.render();
    }
    EndMode2D();
    
    renderGlobalUI();
    
    EndDrawing();
}