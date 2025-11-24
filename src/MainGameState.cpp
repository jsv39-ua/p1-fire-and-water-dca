#include <MainGameState.hpp>
#include <PauseMenuState.hpp>
#include <GameOverState.hpp>
#include <AudioManager.hpp>
#include <iostream>
#include <cmath>
#include <GameOverState.hpp> // Incluye la nueva clase
#include <PauseMenuState.hpp> // Incluye el menú de pausa
#include <StateMachine.hpp> // Asegúrate de incluirla
#include <Player.hpp>
#include <ResourceManager.hpp>


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

    // Inicialización del personaje de Agua
    water_player = Player(150.0f, 300.0f, hitboxWidth, hitboxHeight, WATER);
    
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

    // Inicializar animaciones (tu spritesheet: 512x192 = 8 columnas x 4 filas, 64x48 por frame)
    // Animation(frameCount, frameTime, isLooping, sprite, primerFrame, ultimoFrame)
    
    // Idle: primeros 4 frames de la primera fila (frames 0-3)
    fire_idle_animation = Animation(5, 0.2f, true, character1, 9, 12);
    
    // Walk: frames de la tercera fila (fila 2, frames 16-22) - 7 frames de caminar
    fire_walk_animation = Animation(8, 0.1f, true, character1, 0, 8);
    
    // Lo mismo para el personaje de agua
    water_idle_animation = Animation(5, 0.2f, true, character2, 9, 12);
    water_walk_animation = Animation(8, 0.1f, true, character2, 0, 8);
}

void MainGameState::handleInput()
{
    // Abrir menú de pausa con Escape
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->add_state(std::make_unique<PauseMenuState>(fire_player, water_player), true);
        return; // No procesar más input si se abre el menú de pausa
    }

    // Control del personaje de Fuego (teclas A y D)
    if (IsKeyDown(KEY_A)) {
        fire_player.walk(-1);
    } else if (IsKeyDown(KEY_D)) {
        fire_player.walk(1);
    } else {
        fire_player.walk(0);
    }

    // Salto del personaje de Fuego (tecla W)
    if (IsKeyPressed(KEY_W) && fire_on_ground) {
        fire_player.jump();
    }

    // Control del personaje de Agua (flechas izquierda y derecha)
    if (IsKeyDown(KEY_LEFT)) {
        water_player.walk(-1);
    } else if (IsKeyDown(KEY_RIGHT)) {
        water_player.walk(1);
    } else {
        water_player.walk(0);
    }

    // Salto del personaje de Agua (tecla FLECHA ARRIBA)
    if (IsKeyPressed(KEY_UP) && water_on_ground) {
        water_player.jump();
    }

    
}

void MainGameState::update(float deltaTime)
{
    // Resetea el estado de "en el suelo" al principio del frame
    fire_on_ground = false;
    water_on_ground = false;

    // Actualizar jugadores (la gravedad se aplica dentro de Player::update)
    fire_player.update(deltaTime);
    water_player.update(deltaTime);

    // Actualizar animaciones según el estado del jugador
    // Si el jugador se está moviendo, usar animación de caminar
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
    
    // Redondear la posición de la cámara para evitar parpadeo
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
    
    // Renderizar enemigos
    for (auto& enemy : level_enemies) {
        enemy.render();
    }
    EndMode2D();
    
    // Renderizar UI global (en coordenadas de pantalla, no del mundo)
    renderGlobalUI();
    
    EndDrawing();
}
