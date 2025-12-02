#include <MainGameState.hpp>
#include <iostream>
#include <cmath>
#include <GameOverState.hpp>
#include <SimpleLevelState.hpp>
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
    fire_player = Player(100.0f, 275.0f, hitboxWidth, hitboxHeight, FIRE);
    
    // Cargar teclas desde SettingsManager
    fire_player.setKeys(
        SettingsManager::getInstance()->getFireLeft(),
        SettingsManager::getInstance()->getFireRight(),
        SettingsManager::getInstance()->getFireJump()
    );

    // Inicialización del personaje de Agua con tamaño basado en el asset
    water_player = Player(150.0f, 275.0f, hitboxWidth, hitboxHeight, WATER);
    
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
        mapLoader.loadMapa("assets/levels/niveles/nivel-tutorial/nivel-tutorial.tmx");
        
        std::vector<Platform> platforms = mapLoader.extractPlatforms();
        
        for (const auto& platform : platforms) {
            level_platforms.push_back(platform);
        }

        // Debug: Log GOAL platforms found
        int goalCount = 0;
        for (const auto& p : level_platforms) {
            if (p.type == GOAL) {
                goalCount++;
                TraceLog(LOG_INFO, "GOAL platform in level_platforms at x=%f y=%f w=%f h=%f", p.bounds.x, p.bounds.y, p.bounds.width, p.bounds.height);
            }
        }
        TraceLog(LOG_INFO, "Total GOAL platforms: %d", goalCount);

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

    // Debug: Spawn enemies during gameplay
    if (IsKeyPressed(KEY_E)) {
        // Spawn a FIRE enemy (affects Water player) near the Fire player, vertical movement
        Rectangle b = { fire_player.position.x + 100, fire_player.position.y - 32, 32, 32 };
        Enemy en(b, FIRE, 90.0f, /*vertical=*/true);
        level_enemies.push_back(en);
        TraceLog(LOG_INFO, "DEBUG: Spawned vertical FIRE enemy at x=%f y=%f", b.x, b.y);
    }

    if (IsKeyPressed(KEY_R)) {
        // Spawn a WATER enemy (affects Fire player) near the Water player, horizontal movement
        Rectangle b = { water_player.position.x + 100, water_player.position.y - 32, 32, 32 };
        Enemy en2(b, WATER, 150.0f, /*vertical=*/false);
        level_enemies.push_back(en2);
        TraceLog(LOG_INFO, "DEBUG: Spawned horizontal WATER enemy at x=%f y=%f", b.x, b.y);
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
            // Ambos jugadores han llegado a la puerta (GOAL): avanzar a siguiente nivel placeholder
            TraceLog(LOG_INFO, "Ambos jugadores han alcanzado la GOAL: cambiando a SimpleLevelState");
            this->state_machine->add_state(std::make_unique<SimpleLevelState>(), true);
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

    // Debug: Check collisions and display logs for enemies (already handled below)

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

    // Comprobar si algún jugador se ha caido por debajo del mapa (p.ej. agua debajo del mapa)
    float mapBottom = mapBounds.y + mapBounds.height;
    bool fire_fell_below = (fire_player.bounds.y + fire_player.bounds.height) > (mapBottom + 1.0f);
    bool water_fell_below = (water_player.bounds.y + water_player.bounds.height) > (mapBottom + 1.0f);
    if (fire_fell_below || water_fell_below) {
        TraceLog(LOG_INFO, "Juego terminado: jugador(s) cayeron por debajo del mapa (fire=%d, water=%d)", fire_fell_below, water_fell_below);
        this->state_machine->add_state(std::make_unique<GameOverState>(), true);
        return;
    }

    // (El chequeo para 2 jugadores ha sido sustituido por el chequeo para 1 jugador arriba.)
}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    // Dibujar capas de parallax ANTES de BeginMode2D
    mapLoader.drawParallaxLayers(camera);

    BeginMode2D(camera);
    
    // Dibujar las capas de tiles (plataformas, decoraciones)
    mapLoader.drawTileLayers();

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
            // Dibujamos la puerta como rectángulo negro relleno, borde y etiqueta para depuración
            DrawRectangleRec(platform.bounds, BLACK);
            DrawRectangleLinesEx(platform.bounds, 3, MAGENTA);
            std::string label = "PUERTA";
            int textW = MeasureText(label.c_str(), 10);
            DrawText(label.c_str(), platform.bounds.x + (platform.bounds.width - textW) / 2, platform.bounds.y - 12, 10, MAGENTA);
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
    // Debug: draw outlines and labels for enemies
    for (size_t ei = 0; ei < level_enemies.size(); ++ei) {
        Enemy &en = level_enemies[ei];
        DrawRectangleLinesEx(en.bounds, 2, YELLOW);
        const char* typeLabel = (en.type == FIRE) ? "FIRE" : (en.type == WATER ? "WATER" : "NONE");
        std::string lab = "ENEMY(" + std::string(typeLabel) + ")";
        int tw = MeasureText(lab.c_str(), 12);
        DrawText(lab.c_str(), en.bounds.x + (en.bounds.width - tw)/2, en.bounds.y - 14, 12, BLACK);
    }
    EndMode2D();
    
    renderGlobalUI();
    
    EndDrawing();
}