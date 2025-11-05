#include <MainGameState.hpp>
#include <iostream>
#include <GameOverState.hpp> // Incluye la nueva clase
#include <PauseMenuState.hpp> // Incluye el menú de pausa
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
    // Plataforma en fila 5, columna 1: (1*18, 5*18) = (18, 90)
    // Personaje encima: y = 90 - altura_personaje = 90 - 30 = 60
    fire_player.position = { 36.0f, 60.0f };  // Columna 2, encima de la plataforma
    fire_player.velocity = { 0.0f, 0.0f };
    fire_player.width = 30.0f;
    fire_player.height = 30.0f;
    fire_player.bounds = { fire_player.position.x, fire_player.position.y, fire_player.width, fire_player.height };
    fire_player.type = FIRE;

    // Inicialización del personaje de Agua
    water_player.position = { 72.0f, 60.0f };  // Columna 4, encima de la misma plataforma
    water_player.velocity = { 0.0f, 0.0f };
    water_player.width = 30.0f;
    water_player.height = 30.0f;
    water_player.bounds = { water_player.position.x, water_player.position.y, water_player.width, water_player.height };
    water_player.type = WATER;

    // Inicialización de la cámara
    camera = { 0 };
    camera.target = (Vector2){ 0, 0 };
    camera.offset = (Vector2){ 640, 360 }; // Centro de pantalla 1280x720
    camera.rotation = 0.0f;
    camera.zoom = 1.0f; // Escala 2x para hacer los tiles más grandes

    // Inicialización del MapLoader
    try {
        mapLoader.loadMapa("assets/levels/level_prueba/nivel1-pantalla2.tmx");
        
        // Extraer colisiones del mapa de Tiled
        std::vector<Rectangle> collisions = mapLoader.extractCollisions();
        
        // Convertir las colisiones en plataformas
        for (const auto& collision : collisions) {
            Platform platform = { collision, "default" };
            level_platforms.push_back(platform);
        }
        
        TraceLog(LOG_INFO, "Plataformas cargadas desde Tiled: %d", level_platforms.size());
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error al cargar el mapa: " << e.what() << std::endl;
        this->state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }

    // Plataforma de meta (puedes agregarla manualmente o crearla en Tiled)
    // Platform goal = { { 400, 390, 100, 50 }, "goal" };
    // level_platforms.push_back(goal);

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
    // Abrir menú de pausa con Escape
    if (IsKeyPressed(KEY_ESCAPE)) {
        this->state_machine->add_state(std::make_unique<PauseMenuState>(), false);
        return; // No procesar más input si se abre el menú de pausa
    }

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
        bool fire_collides = CheckCollisionRecs(fire_player.bounds, platform.bounds);
        bool water_collides = CheckCollisionRecs(water_player.bounds, platform.bounds);

        // Revisa colisión con el personaje de Fuego
        if (fire_collides) {
            // Detiene el movimiento vertical
            fire_player.velocity.y = 0;
            // Corrige la posición del jugador para que no caiga
            fire_player.position.y = platform.bounds.y - fire_player.bounds.height;
            fire_player.bounds.y = fire_player.position.y;
            fire_on_ground = true; // El jugador está en el suelo
        }
        // Revisa colisión con el personaje de Agua
        if (water_collides) {
            // Detiene el movimiento vertical
            water_player.velocity.y = 0;
            // Corrige la posición del jugador
            water_player.position.y = platform.bounds.y - water_player.bounds.height;
            water_player.bounds.y = water_player.position.y;
            water_on_ground = true; // El jugador está en el suelo
        }

        // Detecta si ambos jugadores están en la meta
        if(fire_collides && water_collides && platform.type == "goal") {
            this->state_machine->add_state(std::make_unique<GameOverState>(true), true);
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
    
    // Actualizar la cámara para seguir el punto medio entre los dos personajes
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
    ClearBackground(RAYWHITE);

    // Inicia el modo de cámara 2D para todos los elementos
    BeginMode2D(camera);
    
    // Dibuja el mapa dentro del modo cámara
    mapLoader.draw(camera);

    // Dibujar el personaje de Fuego
    DrawRectangleRec(fire_player.bounds, RED);
    
    // Dibujar el personaje de Agua
    DrawRectangleRec(water_player.bounds, BLUE);

    // NO dibujar las plataformas en gris porque ya se ven en el tilemap
    // Solo dibujar plataformas especiales como la meta
    for (const auto& platform : level_platforms) {
        if(platform.type == "goal"){
            DrawRectangleRec(platform.bounds, YELLOW);
        }
        // Las plataformas "default" ya están en el tilemap, no las dibujamos
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

    // Termina el modo de cámara 2D
    EndMode2D();
    
    EndDrawing();
}