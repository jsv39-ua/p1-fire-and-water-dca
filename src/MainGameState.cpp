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

MainGameState::MainGameState() : playerDying(false), deathTimer(0.0f)
{
}

void MainGameState::init()
{
    // Cargar texturas de personajes
    character_textures[0] = ResourceManager::getInstance()->loadTexture("assets/characters/tilesets/Wizzart_A.png");
    character_textures[1] = ResourceManager::getInstance()->loadTexture("assets/characters/tilesets/Wizzart_C.png");
    
    // Calcular dimensiones de cada frame del spritesheet
    int columns = 8; // 8 columnas en el spritesheet
    int rows = 4;    // 4 filas en el spritesheet
    float frameWidth = character_textures[0].width / (float)columns;
    float frameHeight = character_textures[0].height / (float)rows;
    
    // Ajustar el tamaño de la hitbox
    float hitboxWidth = frameWidth * 0.5f;
    float hitboxHeight = frameHeight;
   
    // Inicializar jugadores: [0] = FIRE, [1] = WATER
    players[0] = Player(100.0f, 300.0f, hitboxWidth, hitboxHeight, FIRE);
    players[1] = Player(150.0f, 300.0f, hitboxWidth, hitboxHeight, WATER);
    
    // Cargar teclas desde SettingsManager
    players[0].setKeys(
        SettingsManager::getInstance()->getFireLeft(),
        SettingsManager::getInstance()->getFireRight(),
        SettingsManager::getInstance()->getFireJump()
    );
    
    players[1].setKeys(
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

    // Inicializar animaciones para ambos jugadores
    idle_animations[0] = Animation(5, 0.2f, true, character_textures[0], 9, 12);
    walk_animations[0] = Animation(8, 0.1f, true, character_textures[0], 0, 8);
    
    idle_animations[1] = Animation(5, 0.2f, true, character_textures[1], 9, 12);
    walk_animations[1] = Animation(8, 0.1f, true, character_textures[1], 0, 8);
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
    int playerKeys[2][3] = {
        {SettingsManager::getInstance()->getFireLeft(), SettingsManager::getInstance()->getFireRight(), SettingsManager::getInstance()->getFireJump()},
        {SettingsManager::getInstance()->getWaterLeft(), SettingsManager::getInstance()->getWaterRight(), SettingsManager::getInstance()->getWaterJump()}
    };

    // Control de ambos jugadores con un bucle
    for (int i = 0; i < 2; i++) {
        // Control de movimiento
        if (IsKeyDown(playerKeys[i][0])) {
            players[i].walk(-1);
        } else if (IsKeyDown(playerKeys[i][1])) {
            players[i].walk(1);
        } else {
            players[i].walk(0);
        }

        // Control de salto
        if (IsKeyPressed(playerKeys[i][2]) && on_ground[i]) {
            players[i].jump();
            players[i].jumpSound.play();
        }
    }

    // Debug: Spawn enemies during gameplay
    if (IsKeyPressed(KEY_E)) {
        // Spawn a FIRE enemy (affects Water player) near the Fire player, vertical movement
        Rectangle b = { players[0].position.x + 100, players[0].position.y - 32, 32, 32 };
        Enemy en(b, FIRE, 90.0f, /*vertical=*/true);
        level_enemies.push_back(en);
        TraceLog(LOG_INFO, "DEBUG: Spawned vertical FIRE enemy at x=%f y=%f", b.x, b.y);
    }

    if (IsKeyPressed(KEY_R)) {
        // Spawn a WATER enemy (affects Fire player) near the Water player, horizontal movement
        Rectangle b = { players[1].position.x + 100, players[1].position.y - 32, 32, 32 };
        Enemy en2(b, WATER, 150.0f, /*vertical=*/false);
        level_enemies.push_back(en2);
        TraceLog(LOG_INFO, "DEBUG: Spawned horizontal WATER enemy at x=%f y=%f", b.x, b.y);
    }
}

void MainGameState::update(float deltaTime)
{
    // Si un jugador está muriendo, contar el timer
    if (playerDying) {
        deathTimer += deltaTime;
        if (deathTimer >= DEATH_DELAY) {
            this->state_machine->add_state(std::make_unique<GameOverState>(), true);
            return;
        }
        // No actualizar el resto del juego durante la muerte
        return;
    }
    
    // Resetear flags de suelo
    on_ground[0] = false;
    on_ground[1] = false;

    // Actualizar ambos jugadores
    for (int i = 0; i < 2; i++) {
        players[i].update(deltaTime);
        
        int jumpKey = (i == 0) ? SettingsManager::getInstance()->getFireJump() 
                               : SettingsManager::getInstance()->getWaterJump();
        players[i].updateJump(deltaTime, IsKeyDown(jumpKey));

        // Actualizar animaciones
        if (players[i].velocity.x != 0) {
            walk_animations[i].animationUpdate(deltaTime);
        } else {
            idle_animations[i].animationUpdate(deltaTime);
        }
    }

    // Verificar colisiones con plataformas
    bool bothOnGoal = true;
    for (const auto& platform : level_platforms) {
        bool fireOnGoal = false;
        bool waterOnGoal = false;
        
        for (int i = 0; i < 2; i++) {
            int collision = players[i].checkPlatformCollision(platform);

            switch (collision) {
                case 0:
                case 2:
                    if (players[i].resolvePlatformCollision(platform)) {
                        on_ground[i] = true;
                    }
                    break;
                case 3:  // GOAL
                    if (players[i].resolvePlatformCollision(platform)) {
                        on_ground[i] = true;
                    }
                    if (i == 0) fireOnGoal = true;
                    else waterOnGoal = true;
                    break;
                case 1:  // Muerte
                    // Activar timer de muerte en vez de cambiar inmediatamente
                    playerDying = true;
                    deathTimer = 0.0f;
                    TraceLog(LOG_INFO, "[MainGameState] Player hit deadly platform, starting death timer");
                    return;
            }
        }
        
        // Verificar si ambos están en la puerta
        if (fireOnGoal && waterOnGoal) {
            TraceLog(LOG_INFO, "Ambos jugadores han alcanzado la GOAL: cambiando a SimpleLevelState");
            this->state_machine->add_state(std::make_unique<SimpleLevelState>(), true);
            return;
        }
    }
    
    // Controlar música de pasos para ambos jugadores
    for (int i = 0; i < 2; i++) {
        if (players[i].velocity.x != 0 && on_ground[i]) {
            if (!players[i].isWalkMusicPlaying) {
                if (IsMusicValid(players[i].walkMusic)) {
                    PlayMusicStream(players[i].walkMusic);
                    players[i].isWalkMusicPlaying = true;
                    TraceLog(LOG_INFO, "[MainGameState] Player %d walk music started", i);
                }
            }
        } else {
            if (players[i].isWalkMusicPlaying) {
                StopMusicStream(players[i].walkMusic);
                players[i].isWalkMusicPlaying = false;
                TraceLog(LOG_INFO, "[MainGameState] Player %d walk music stopped", i);
            }
        }
    }

    // Verificar colisiones con enemigos
    for (auto& enemy : level_enemies) {
        enemy.update(deltaTime);
        
        for (int i = 0; i < 2; i++) {
            if (CheckCollisionRecs(players[i].bounds, enemy.bounds)) {
                // FIRE player (i=0) muere con enemigos WATER, WATER player (i=1) muere con enemigos FIRE
                ElementType dangerousType = (i == 0) ? WATER : FIRE;
                
                if (enemy.type == dangerousType && !players[i].isInvincible()) {
                    players[i].updateLifeCount(-1);
                    players[i].damageSound.play();
                    if (players[i].getLifeCount() <= 0) {
                        // Activar timer de muerte en vez de cambiar inmediatamente
                        playerDying = true;
                        deathTimer = 0.0f;
                        players[i].deathSound.play();
                        TraceLog(LOG_INFO, "[MainGameState] Player %d died, starting death timer", i);
                        return;
                    }
                }
            }
        }
    }

    // Actualizar cámara (punto medio entre ambos jugadores)
    Vector2 midPoint = {
        (players[0].position.x + players[1].position.x) / 2.0f,
        (players[0].position.y + players[1].position.y) / 2.0f
    };
    
    float cameraSpeed = 3.0f * deltaTime;
    camera.target.x += (midPoint.x - camera.target.x) * cameraSpeed;
    camera.target.y += (midPoint.y - camera.target.y) * cameraSpeed;
    
    // Aplicar límites de la cámara
    Rectangle mapBounds = mapLoader.getMapBounds();
    
    float screenWidth = 1280.0f;  
    float screenHeight = 720.0f;
    
    float visibleWidth = screenWidth / camera.zoom;
    float visibleHeight = screenHeight / camera.zoom;
    
    float minX = mapBounds.x + visibleWidth / 2.0f;
    float maxX = mapBounds.x + mapBounds.width - visibleWidth / 2.0f;
    float minY = mapBounds.y + visibleHeight / 2.0f;
    float maxY = mapBounds.y + mapBounds.height - visibleHeight / 2.0f;
    
    if (mapBounds.width > visibleWidth) {
        camera.target.x = fmaxf(minX, fminf(camera.target.x, maxX));
    } else {
        camera.target.x = mapBounds.x + mapBounds.width / 2.0f;
    }
    
    if (mapBounds.height > visibleHeight) {
        camera.target.y = fmaxf(minY, fminf(camera.target.y, maxY));
    } else {
        camera.target.y = mapBounds.y + mapBounds.height / 2.0f;
    }
    
    camera.target.x = roundf(camera.target.x);
    camera.target.y = roundf(camera.target.y);

    // Comprobar si algún jugador se ha caido por debajo del mapa
    float mapBottom = mapBounds.y + mapBounds.height;
    bool fire_fell_below = (players[0].bounds.y + players[0].bounds.height) > (mapBottom + 1.0f);
    bool water_fell_below = (players[1].bounds.y + players[1].bounds.height) > (mapBottom + 1.0f);
    if (fire_fell_below || water_fell_below) {
        TraceLog(LOG_INFO, "Juego terminado: jugador(s) cayeron por debajo del mapa (fire=%d, water=%d)", fire_fell_below, water_fell_below);
        // Activar timer de muerte en vez de cambiar inmediatamente
        playerDying = true;
        deathTimer = 0.0f;
        return;
    }
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
    
    // Renderizar ambos jugadores en un bucle
    for (int i = 0; i < 2; i++) {
        Rectangle sourceRect;
        
        // Seleccionar animación según movimiento
        if (players[i].velocity.x != 0) {
            sourceRect = walk_animations[i].animationFrame(8, frameWidth, frameHeight);
            players[i].facingRight = players[i].velocity.x > 0;
        } else {
            sourceRect = idle_animations[i].animationFrame(8, frameWidth, frameHeight);
        }
        
        // Voltear sprite si mira a la izquierda
        if (!players[i].facingRight) {
            sourceRect.width = -frameWidth;
        }
        
        // Calcular posición de renderizado
        float renderX = players[i].position.x + (players[i].width / 2.0f) - (renderWidth / 2.0f);
        float renderY = players[i].position.y + players[i].height - renderHeight;
        
        // Dibujar solo si debe ser visible (parpadeo durante invencibilidad)
        if (players[i].shouldRenderVisible()) {
            DrawTexturePro(character_textures[i], 
                           sourceRect,  
                           (Rectangle){renderX, renderY, renderWidth, renderHeight}, 
                           (Vector2){0, 0}, 
                           0.0f, 
                           WHITE);
        }
    }

    // Debug: bounds de jugadores
    DrawRectangleLinesEx(players[0].bounds, 2, GREEN);
    DrawRectangleLinesEx(players[1].bounds, 2, BLUE);

    

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
    
    // Dibujar vidas de los jugadores (UI fija en pantalla)
    int maxLives = 3;
    int heartSize = 30;
    int heartSpacing = 5;
    int marginFromEdge = 20;
    int topMargin = 20;
    
    // Colores para cada jugador
    Color playerColors[2] = {RED, BLUE};
    int screenWidth = GetScreenWidth();
    
    // Jugador FIRE (izquierda)
    int fireLives = players[0].getLifeCount();
    for (int i = 0; i < maxLives; i++) {
        int x = marginFromEdge + i * (heartSize + heartSpacing);
        int y = topMargin;
        
        Color color = (i < fireLives) ? RED : GRAY;
        DrawRectangle(x, y, heartSize, heartSize, color);
        DrawRectangleLinesEx({(float)x, (float)y, (float)heartSize, (float)heartSize}, 2, DARKGRAY);
    }
    
    // Jugador WATER (derecha)
    int waterLives = players[1].getLifeCount();
    for (int i = 0; i < maxLives; i++) {
        int x = screenWidth - marginFromEdge - (i + 1) * (heartSize + heartSpacing) + heartSpacing;
        int y = topMargin;
        
        Color color = (i < waterLives) ? BLUE : GRAY;
        DrawRectangle(x, y, heartSize, heartSize, color);
        DrawRectangleLinesEx({(float)x, (float)y, (float)heartSize, (float)heartSize}, 2, DARKGRAY);
    }
    
    renderGlobalUI();
    
    EndDrawing();
}