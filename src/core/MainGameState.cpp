#include <MainGameState.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
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

// Helper function to format sign text with current controls
std::string formatSignText(const std::string& text) {
    // Check if this is the tutorial sign
    if (text.find("Hola, bienvenido a Elements") != std::string::npos) {
        SettingsManager* sm = SettingsManager::getInstance();
        
        std::string newText = "Hola, bienvenido a Elements, vamos a aprender lo básico de forma práctica... ¡Diviértete!\n\n";
        newText += "Controles:\n\n";
        
        newText += "Jugador Agua: Movimiento básico: " + sm->getKeyName(sm->getWaterLeft()) + ", " + sm->getKeyName(sm->getWaterJump()) + ", " + sm->getKeyName(sm->getWaterRight()) + "\n";
        newText += "Ataque: " + sm->getKeyName(sm->getWaterAttack()) + "\n\n";
        
        newText += "Jugador Fuego: Movimiento básico: " + sm->getKeyName(sm->getFireLeft()) + ", " + sm->getKeyName(sm->getFireJump()) + ", " + sm->getKeyName(sm->getFireRight()) + "\n";
        newText += "Ataque: " + sm->getKeyName(sm->getFireAttack());
        
        return newText;
    }
    return text;
}

const float DEFAULT_SCREEN_WIDTH = 1280.0f;
const float DEFAULT_SCREEN_HEIGHT = 720.0f;

MainGameState::MainGameState() : playerDying(false), deathTimer(0.0f), speedrunTimer(0.0f)
{
}

void MainGameState::init()
{
    // Cargar texturas de personajes
    character_textures[0] = ResourceManager::getInstance()->loadTexture("assets/characters/Wizzart_A.png");
    character_textures[1] = ResourceManager::getInstance()->loadTexture("assets/characters/Wizzart_C.png");
    
    // Calcular dimensiones de cada frame del spritesheet
    int columns = 8; // 8 columnas en el spritesheet
    int rows = 4;    // 4 filas en el spritesheet
    float frameWidth = character_textures[0].width / (float)columns;
    float frameHeight = character_textures[0].height / (float)rows;
    
    // Ajustar el tamaño de la hitbox
    float hitboxWidth = frameWidth * 0.5f;
    float hitboxHeight = frameHeight;
   
    // Inicializar jugadores: [0] = FIRE, [1] = WATER
    players[0] = Player(100.0f, 275.0f, hitboxWidth, hitboxHeight, FIRE);
    players[1] = Player(150.0f, 275.0f, hitboxWidth, hitboxHeight, WATER);
    
    // Cargar teclas desde SettingsManager (Updated)
    players[0].setKeys(
        SettingsManager::getInstance()->getFireLeft(),
        SettingsManager::getInstance()->getFireRight(),
        SettingsManager::getInstance()->getFireJump(),
        SettingsManager::getInstance()->getFireAttack()
    );
    
    players[1].setKeys(
        SettingsManager::getInstance()->getWaterLeft(),
        SettingsManager::getInstance()->getWaterRight(),
        SettingsManager::getInstance()->getWaterJump(),
        SettingsManager::getInstance()->getWaterAttack()
    );

    // Inicialización de la cámara (se configurará después de cargar el mapa)

    // Inicialización del MapLoader
    loadLevel("assets/levels/niveles/nivel-tutorial/zona-1.tmx");

    // Inicializar cámara con bounds del mapa
    Rectangle mapBounds = mapLoader.getMapBounds();
    camera.init({DEFAULT_SCREEN_WIDTH / 2.0f, DEFAULT_SCREEN_HEIGHT / 2.0f}, 2.0f, mapBounds);
    camera.setSmoothSpeed(5.0f);
    camera.setZoomSmoothSpeed(3.0f);

    // Calcular zoom mínimo para evitar ver fuera del mapa
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float minZoomX = screenWidth / mapBounds.width;
    float minZoomY = screenHeight / mapBounds.height;
    
    // El zoom mínimo debe ser suficiente para cubrir la pantalla (evitar bordes negros)
    // Y también respetamos un mínimo de 1.0f para no alejar demasiado la cámara en mapas grandes
    float minZoom = fmaxf(1.0f, fmaxf(minZoomX, minZoomY));

    camera.setZoomLimits(minZoom, 3.0f);
    camera.setDynamicZoom(true);

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
    int playerKeys[2][4] = {
        {SettingsManager::getInstance()->getFireLeft(), SettingsManager::getInstance()->getFireRight(), SettingsManager::getInstance()->getFireJump(), SettingsManager::getInstance()->getFireAttack()},
        {SettingsManager::getInstance()->getWaterLeft(), SettingsManager::getInstance()->getWaterRight(), SettingsManager::getInstance()->getWaterJump(), SettingsManager::getInstance()->getWaterAttack()}
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
        
        // Control de ataque
        if (IsKeyPressed(playerKeys[i][3])) {
            if (players[i].canAttack()) {
                players[i].attack();
                
                // Crear proyectil
                // Posición: centro del jugador
                Vector2 pos = { players[i].position.x + players[i].width/2, players[i].position.y + players[i].height/2 };
                
                // Velocidad: Depende de la dirección
                float speed = 400.0f;
                Vector2 vel = { players[i].facingRight ? speed : -speed, 0 };
                
                // Tipo: El del jugador
                projectiles.emplace_back(pos, vel, players[i].type);
            }
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
    // Actualizar timer de speedrun
    speedrunTimer += deltaTime;

    // Si el jugador está muriendo, actualizar timer de muerte
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
    // bool bothOnGoal = true;
    
    // Reset active sign text
    activeSignText = "";
    
    // Optimization: Merged reset logic into the main collision loop to avoid double iteration
    for (auto& platform : level_platforms) {
        if (!platform.active) continue;

        bool fireOnGoal = false;
        bool waterOnGoal = false;
        bool touchedByAny = false;
        
        for (int i = 0; i < 2; i++) {
            // Lógica de Carteles (SIGN) - Solicitada por usuario
            if (platform.type == SIGN) {
                if (CheckCollisionRecs(players[i].bounds, platform.bounds)) {
                    activeSignText = formatSignText(platform.text);
                }
            }

            int collision = players[i].checkPlatformCollision(platform);

            // Check if button is being touched (collision 5 is BUTTON)
            if (platform.isButton && collision == 5) {
                touchedByAny = true;
            }

            switch (collision) {
                case 0:
                    if (players[i].resolvePlatformCollision(platform)) {
                        on_ground[i] = true;
                    }
                    break;
                case 2:
                    // Friendly liquid: do nothing (pass through)
                    break;
                case 3:  // GOAL
                    if (players[i].resolvePlatformCollision(platform)) {
                        on_ground[i] = true;
                    }
                    if (i == 0) fireOnGoal = true;
                    else waterOnGoal = true;
                    break;
                case 1:  // Daño por superficie letal
                    if (!players[i].isInvincible()) {
                        // TODO: poner mas daño si queremos que se muera directo
                        players[i].updateLifeCount(-3);
                        
                        if (players[i].getLifeCount() > 0) {
                            players[i].damageSound.play();
                        } else {
                            playerDying = true;
                            deathTimer = 0.0f;
                            players[i].deathSound.play();
                            TraceLog(LOG_INFO, "[MainGameState] Player %d died from deadly platform", i);
                            return;
                        }
                    }
                    break;
                case 5: // BUTTON
                    if (platform.isButton) {
                        // Logic for One-Shot Trigger
                        if (!platform.isPressed) {
                            platform.isPressed = true;
                            
                            // Activate targets
                            for (auto& target : level_platforms) {
                                // Check ID match
                                if (platform.targetId != -1 && target.id == platform.targetId && target.id != platform.id) {
                                    target.active = true;
                                    TraceLog(LOG_INFO, "Button %d activated platform %d by ID", platform.id, target.id);
                                }
                                // Check Tag match
                                if (!platform.targetTag.empty() && !target.tag.empty() && target.tag == platform.targetTag && target.id != platform.id) {
                                    target.active = true;
                                    TraceLog(LOG_INFO, "Button %d activated platform %d by TAG", platform.id, target.id);
                                }
                            }
                        }
                    }
                    break;
            }
        }
        
        // Reset button state if it was pressed but no one is touching it anymore
        if (platform.isButton && platform.isPressed && !touchedByAny) {
            platform.isPressed = false;
        }

        // Verificar si ambos están en la puerta
        if (fireOnGoal && waterOnGoal) {
            TraceLog(LOG_INFO, "Ambos jugadores han alcanzado la GOAL: cambiando a SimpleLevelState");
            
            // Guardar mejor tiempo si corresponde
            float currentBest = SettingsManager::getInstance()->getBestTime();
            if (currentBest == 0.0f || speedrunTimer < currentBest) {
                SettingsManager::getInstance()->setBestTime(speedrunTimer);
                SettingsManager::getInstance()->saveSettings();
                TraceLog(LOG_INFO, "New Best Time: %f", speedrunTimer);
            }
            
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

    // Actualizar proyectiles y colisiones con enemigos
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        it->update(deltaTime);
        
        bool hit = false;
        if (it->isActive()) {
            for (auto& enemy : level_enemies) {
                if (CheckCollisionRecs(it->getBounds(), enemy.bounds)) {
                    // Fuego daña Agua, Agua daña Fuego
                    bool isOpposite = (it->getType() == FIRE && enemy.type == WATER) || 
                                      (it->getType() == WATER && enemy.type == FIRE);
                                      
                    if (isOpposite) {
                        if (!enemy.isInvincible()) {
                            enemy.updateLifeCount(-1);
                        }
                        hit = true;
                    } else {
                        // Mismo elemento: el proyectil se destruye sin daño
                        hit = true;
                    }
                    break; // Un proyectil solo golpea un enemigo
                }
            }
        }
        
        if (!it->isActive() || hit) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
    
    // Check for dead enemies and trigger events (e.g. deactivate platforms)
    for (const auto& enemy : level_enemies) {
        if (enemy.getLifeCount() <= 0) {
            if (!enemy.targetTag.empty()) {
                int count = 0;
                // Deactivate platforms with this tag
                for (auto& platform : level_platforms) {
                    if (platform.active && !platform.tag.empty() && platform.tag == enemy.targetTag) {
                        platform.active = false;
                        count++;
                    }
                }
                if (count > 0) {
                    TraceLog(LOG_INFO, "Enemy death deactivated %d platforms with tag: %s", count, enemy.targetTag.c_str());
                }
            }
        }
    }

    // Limpiar enemigos muertos
    level_enemies.erase(std::remove_if(level_enemies.begin(), level_enemies.end(),
        [](const Enemy& e) { return e.getLifeCount() <= 0; }), level_enemies.end());

    // Verificar colisiones con enemigos
    for (auto& enemy : level_enemies) {
        enemy.update(deltaTime);
        
        for (int i = 0; i < 2; i++) {
            if (CheckCollisionRecs(players[i].bounds, enemy.bounds)) {
                // FIRE player (i=0) muere con enemigos WATER, WATER player (i=1) muere con enemigos FIRE
                ElementType dangerousType = (i == 0) ? WATER : FIRE;
                
                if (enemy.type == dangerousType && !players[i].isInvincible()) {
                    players[i].updateLifeCount(-1);

                    // Para no reproducir sonido de daño y muerte a la vez
                    if(players[i].getLifeCount() > 0)
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

    handleTransitions();
    handleMapBoundaries();
    updateCamera(deltaTime);
    checkFallDeath();
}

void MainGameState::handleTransitions() {
    bool fire_in_transition = false;
    bool water_in_transition = false;
    MapTransition* active_transition = nullptr;

    for (auto& transition : level_transitions) {
        if (CheckCollisionRecs(players[0].bounds, transition.bounds)) {
            fire_in_transition = true;
        }
        if (CheckCollisionRecs(players[1].bounds, transition.bounds)) {
            water_in_transition = true;
        }
        
        if (fire_in_transition && water_in_transition) {
            active_transition = &transition;
            break;
        } else {
            // Reset if not both in the same transition
            fire_in_transition = false;
            water_in_transition = false;
        }
    }

    if (active_transition) {
        TraceLog(LOG_INFO, "Transition triggered to %s", active_transition->targetMap.c_str());
        
        // Check for Game Over / Win condition
        if (active_transition->targetMap == "GOAL" || active_transition->targetMap == "WIN") {
             TraceLog(LOG_INFO, "Transition is GOAL: changing to SimpleLevelState");
             
             // Guardar mejor tiempo si corresponde
             float currentBest = SettingsManager::getInstance()->getBestTime();
             if (currentBest == 0.0f || speedrunTimer < currentBest) {
                 SettingsManager::getInstance()->setBestTime(speedrunTimer);
                 SettingsManager::getInstance()->saveSettings();
                 TraceLog(LOG_INFO, "New Best Time: %f", speedrunTimer);
             }
             
             this->state_machine->add_state(std::make_unique<SimpleLevelState>(), true);
             return;
        }

        // Store velocities to preserve momentum
        Vector2 fire_vel = players[0].velocity;
        Vector2 water_vel = players[1].velocity;
        
        // Load new level
        std::string fullPath = active_transition->targetMap;
        if (fullPath.find("assets/") == std::string::npos) {
            fullPath = LEVEL_BASE_PATH + fullPath;
        }
        loadLevel(fullPath);
        
        // Set positions
        players[0].position = active_transition->targetSpawn;
        players[1].position = { active_transition->targetSpawn.x + PLAYER_SPAWN_OFFSET, active_transition->targetSpawn.y }; 

        // Restore velocity
        players[0].velocity = fire_vel;
        players[1].velocity = water_vel;
        
        // Update bounds immediately
        players[0].update(0); 
        players[1].update(0);
    }
}

void MainGameState::handleMapBoundaries() {
    Rectangle mapBounds = mapLoader.getMapBounds();

    for (int i = 0; i < 2; i++) {
        bool inTransition = false;
        Rectangle tBounds = {0,0,0,0};

        for (const auto& t : level_transitions) {
            if (CheckCollisionRecs(players[i].bounds, t.bounds)) {
                inTransition = true;
                tBounds = t.bounds;
                break;
            }
        }

        if (inTransition) {
            // Simular suelo en transiciones laterales: si sale del mapa horizontalmente,
            // anulamos la gravedad para que no caiga al vacío.
            if (players[i].position.x < mapBounds.x || 
                players[i].position.x + players[i].width > mapBounds.x + mapBounds.width) {
                if (players[i].velocity.y > 0) players[i].velocity.y = 0;
            }

            // Límite Izquierdo (si la transición está a la izquierda)
            if (tBounds.x < mapBounds.x) {
                if (players[i].position.x < tBounds.x) {
                    players[i].position.x = tBounds.x;
                    players[i].velocity.x = 0;
                }
            }
            
            // Límite Derecho (si la transición está a la derecha)
            if (tBounds.x + tBounds.width > mapBounds.x + mapBounds.width) {
                if (players[i].position.x + players[i].width > tBounds.x + tBounds.width) {
                    players[i].position.x = tBounds.x + tBounds.width - players[i].width;
                    players[i].velocity.x = 0;
                }
            }

            // Límite Superior (si la transición está arriba)
            if (tBounds.y < mapBounds.y) {
                if (players[i].position.y < tBounds.y) {
                    players[i].position.y = tBounds.y;
                    players[i].velocity.y = 0;
                }
            }

            // Límite Inferior (si la transición está abajo)
            if (tBounds.y + tBounds.height > mapBounds.y + mapBounds.height) {
                // Permitir caer hasta el fondo de la transición
                if (players[i].position.y > tBounds.y + tBounds.height - players[i].height) {
                    players[i].position.y = tBounds.y + tBounds.height - players[i].height;
                    players[i].velocity.y = 0; // Detener caída (quedarse flotando)
                }
            }
        }

        // Actualizar bounds tras modificar posición
        players[i].update(0); 
    }
}

void MainGameState::updateCamera(float deltaTime) {
    // Actualizar cámara para seguir ambos jugadores
    std::vector<Vector2> playerPositions = {players[0].position, players[1].position};
    camera.setMultipleTargets(playerPositions);
    camera.update(deltaTime);
}

void MainGameState::checkFallDeath() {
    Rectangle mapBounds = mapLoader.getMapBounds();
    float mapBottom = mapBounds.y + mapBounds.height;
    float deathThreshold = FALL_DEATH_THRESHOLD;

    bool fire_fell_below = (players[0].bounds.y) > (mapBottom + deathThreshold);
    bool water_fell_below = (players[1].bounds.y) > (mapBottom + deathThreshold);
    
    if (fire_fell_below || water_fell_below) {
        TraceLog(LOG_INFO, "Juego terminado: jugador(s) cayeron muy por debajo del mapa (fire=%d, water=%d)", fire_fell_below, water_fell_below);
        this->state_machine->add_state(std::make_unique<GameOverState>(), true);
        return;
    }
}

void MainGameState::loadLevel(const std::string& mapPath) {
    level_platforms.clear();
    level_enemies.clear();
    level_transitions.clear();

    try {
        mapLoader.loadMapa(mapPath.c_str());
        
        std::vector<Platform> platforms = mapLoader.extractPlatforms();
        for (const auto& platform : platforms) {
            level_platforms.push_back(platform);
        }

        level_enemies = mapLoader.extractEnemies();
        level_transitions = mapLoader.extractTransitions();
        
        TraceLog(LOG_INFO, "Nivel cargado: %s", mapPath.c_str());
        TraceLog(LOG_INFO, "Plataformas: %d, Enemigos: %d, Transiciones: %d", 
            (int)level_platforms.size(), (int)level_enemies.size(), (int)level_transitions.size());
        
        // Actualizar límites de la cámara con el nuevo mapa
        Rectangle mapBounds = mapLoader.getMapBounds();
        camera.setMapBounds(mapBounds);

        // Recalcular zoom mínimo
        float screenWidth = (float)GetScreenWidth();
        float screenHeight = (float)GetScreenHeight();
        
        // Si la ventana no está inicializada o tiene tamaño 0 (al inicio), usar valores por defecto
        if (screenWidth <= 0) screenWidth = DEFAULT_SCREEN_WIDTH;
        if (screenHeight <= 0) screenHeight = DEFAULT_SCREEN_HEIGHT;

        float minZoomX = screenWidth / mapBounds.width;
        float minZoomY = screenHeight / mapBounds.height;
        
        float minZoom = fmaxf(1.0f, fmaxf(minZoomX, minZoomY));
        camera.setZoomLimits(minZoom, 3.0f);
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error al cargar el mapa: " << e.what() << std::endl;
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }
}


void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    // Dibujar capas de parallax ANTES de BeginMode2D
    mapLoader.drawParallaxLayers(camera.getRaylibCamera());

    BeginMode2D(camera.getRaylibCamera());
    
    // Dibujar las capas de tiles (plataformas, decoraciones)
    mapLoader.drawTileLayers();

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
                           Rectangle{renderX, renderY, renderWidth, renderHeight}, 
                           Vector2{0, 0}, 
                           0.0f, 
                           WHITE);
        }
    }

    // Debug: bounds de jugadores (ELIMINADO)
    // DrawRectangleLinesEx(players[0].bounds, 2, GREEN);
    // DrawRectangleLinesEx(players[1].bounds, 2, BLUE);

    

    // Dibujar las plataformas del nivel
    for (const auto& platform : level_platforms) {
        if (!platform.active) continue;

        if (platform.isButton && platform.isPressed && platform.hasPressedVisuals) {
             DrawTexturePro(platform.pressedTexture, platform.pressedSourceRect, platform.bounds, {0,0}, 0.0f, WHITE);
        }
        else if (platform.hasVisuals) {
             DrawTexturePro(platform.texture, platform.sourceRect, platform.bounds, {0,0}, 0.0f, WHITE);
        }

        else if(platform.type == GOAL){
            // Dibujamos la puerta como rectángulo negro relleno, borde y etiqueta para depuración
            DrawRectangleRec(platform.bounds, BLACK);
            DrawRectangleLinesEx(platform.bounds, 3, MAGENTA);
            std::string label = "PUERTA";
            int textW = MeasureText(label.c_str(), 10);
            DrawText(label.c_str(), platform.bounds.x + (platform.bounds.width - textW) / 2, platform.bounds.y - 12, 10, MAGENTA);
        }
        // FIRE and WATER are invisible (visuals are in the map tiles)
        else if(platform.type == BUTTON){
            if (!platform.hasVisuals) {
                DrawRectangleRec(platform.bounds, ORANGE);
                DrawRectangleLinesEx(platform.bounds, 2, BLACK);
            }
        }
    }
    
    for (auto& enemy : level_enemies) {
        enemy.render();
    }
    
    // Renderizar proyectiles
    for (const auto& projectile : projectiles) {
        projectile.render();
    }

    // Debug: draw outlines and labels for enemies (ELIMINADO)
    /*
    for (size_t ei = 0; ei < level_enemies.size(); ++ei) {
        Enemy &en = level_enemies[ei];
        DrawRectangleLinesEx(en.bounds, 2, YELLOW);
        const char* typeLabel = (en.type == FIRE) ? "FIRE" : (en.type == WATER ? "WATER" : "NONE");
        std::string lab = "ENEMY(" + std::string(typeLabel) + ")";
        int tw = MeasureText(lab.c_str(), 12);
        DrawText(lab.c_str(), en.bounds.x + (en.bounds.width - tw)/2, en.bounds.y - 14, 12, BLACK);
    }
    */
    EndMode2D();
    
    // Dibujar vidas de los jugadores (UI fija en pantalla)
    int maxLives = 3;
    int heartSize = 30;
    int heartSpacing = 5;
    int marginFromEdge = 20;
    int topMargin = 20;
    
    // Colores para cada jugador
    // Color playerColors[2] = {RED, BLUE};
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

    // --- TIMER SPEEDRUN ---
    int minutes = (int)speedrunTimer / 60;
    int seconds = (int)speedrunTimer % 60;
    int milliseconds = (int)((speedrunTimer - (int)speedrunTimer) * 100);
    
    const char* timerText = TextFormat("%02d:%02d.%02d", minutes, seconds, milliseconds);
    int timerFontSize = 30;
    int timerWidth = MeasureText(timerText, timerFontSize);
    int timerX = (screenWidth / 2) - (timerWidth / 2);
    int timerY = topMargin;
    
    DrawText(timerText, timerX + 2, timerY + 2, timerFontSize, BLACK); // Sombra
    DrawText(timerText, timerX, timerY, timerFontSize, WHITE);
    
    // --- BEST TIME ---
    float bestTime = SettingsManager::getInstance()->getBestTime();
    if (bestTime > 0.0f) {
        int bMinutes = (int)bestTime / 60;
        int bSeconds = (int)bestTime % 60;
        int bMilliseconds = (int)((bestTime - (int)bestTime) * 100);
        
        const char* bestText = TextFormat("Best: %02d:%02d.%02d", bMinutes, bSeconds, bMilliseconds);
        int bestFontSize = 20;
        int bestWidth = MeasureText(bestText, bestFontSize);
        int bestX = (screenWidth / 2) - (bestWidth / 2);
        int bestY = timerY + timerFontSize + 2;
        
        DrawText(bestText, bestX + 1, bestY + 1, bestFontSize, BLACK);
        DrawText(bestText, bestX, bestY, bestFontSize, GOLD);
    }
    
    // --- UI FINAL (DIBUJADO FUERA DE LOS BUCLES) ---
    if (!activeSignText.empty()) {
        // 1. Configuración de estilo
        int fontSize = 20;
        int padding = 30;        // Más padding para que se vea más grande
        Color boxColor = { 0, 0, 0, 220 }; 
        Color borderColor = RAYWHITE;
        
        // 2. Medir dimensiones del texto multilínea manualmente
        int maxLineWidth = 0;
        int lineCount = 0;
        
        const char* textPtr = activeSignText.c_str();
        const char* lineStart = textPtr;
        
        // Recorrer el texto para contar líneas y medir la más ancha
        for (int i = 0; textPtr[i] != '\0'; i++) {
            if (textPtr[i] == '\n') {
                std::string line(lineStart, textPtr + i - lineStart);
                int w = MeasureText(line.c_str(), fontSize);
                if (w > maxLineWidth) maxLineWidth = w;
                
                lineCount++;
                lineStart = textPtr + i + 1;
            }
        }
        // Medir última línea
        int w = MeasureText(lineStart, fontSize);
        if (w > maxLineWidth) maxLineWidth = w;
        lineCount++; 

        int textWidth = maxLineWidth;
        int textHeight = lineCount * fontSize + (lineCount * 2); // Altura basada en líneas

        // 3. Calcular dimensiones de la caja
        int boxWidth = textWidth + (padding * 2);
        int boxHeight = textHeight + (padding * 2);

        // 4. Calcular posición para centrarlo arriba en la pantalla
        int screenWidth = GetScreenWidth();
        int boxX = (screenWidth / 2) - (boxWidth / 2); 
        int boxY = 50; 

        // 5. DIBUJAR
        DrawRectangle(boxX + 4, boxY + 4, boxWidth, boxHeight, Fade(BLACK, 0.5f));
        DrawRectangle(boxX, boxY, boxWidth, boxHeight, boxColor);
        DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, borderColor);
        DrawText(activeSignText.c_str(), boxX + padding, boxY + padding, fontSize, WHITE);
    }

    renderGlobalUI();
    
    EndDrawing();
}
