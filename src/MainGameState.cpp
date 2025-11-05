#include <MainGameState.hpp>
#include <iostream>
#include <GameOverState.hpp> // Incluye la nueva clase
#include <PauseMenuState.hpp> // Incluye el menú de pausa
#include <StateMachine.hpp> // Asegúrate de incluirla
#include <Player.hpp>


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

    // Inicialización del personaje de Agua
    water_player = Player(150.0f, 300.0f, WATER);

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
            Platform platform = { collision, NONE };
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
    // Creación de plataformas para el nivel de prueba
    Platform ground = Platform({ 0, 400, 800, 50 });
    level_platforms.push_back(ground);

    Platform goal = Platform({ 400, 390, 100, 50 }, GOAL);
    level_platforms.push_back(goal);

    Platform p1 = Platform({ 300, 300, 200, 20 });
    level_platforms.push_back(p1);

    // Creación de peligros (ahora son plataformas con tipo FIRE/WATER)
    Platform lava = Platform({ 200, 350, 100, 50 }, FIRE);
    level_platforms.push_back(lava);

    Platform water = Platform({ 550, 350, 100, 50 }, WATER);
    level_platforms.push_back(water);
    
    // Enemigos permanecen igual
    Enemy fire_enemy = { { 450, 270, 25, 25 }, FIRE, 60.0f };
    level_enemies.push_back(fire_enemy);
    
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

    // Salto del personaje de Fuego (tecla W)
    if (IsKeyPressed(KEY_W) && fire_on_ground) {
        fire_player.velocity.y = -300.0f;
    }

    // Control del personaje de Agua (flechas izquierda y derecha)
    if (IsKeyDown(KEY_LEFT)) {
        water_player.velocity.x = -200.0f;
    } else if (IsKeyDown(KEY_RIGHT)) {
        water_player.velocity.x = 200.0f;
    } else {
        water_player.velocity.x = 0.0f;
    }

    // Salto del personaje de Agua (tecla FLECHA ARRIBA)
    if (IsKeyPressed(KEY_UP) && water_on_ground) {
        water_player.velocity.y = -300.0f;
    }
}

void MainGameState::update(float deltaTime)
{
    // Resetea el estado de "en el suelo" al principio del frame
    fire_on_ground = false;
    water_on_ground = false;

    // Aplicar gravedad
    fire_player.velocity.y += 500.0f * deltaTime;
    water_player.velocity.y += 500.0f * deltaTime;

    // Actualizar posiciones
    fire_player.update(deltaTime);
    water_player.update(deltaTime);

    // Colisiones con plataformas
    for (const auto& platform : level_platforms) {
        bool fire_collides = CheckCollisionRecs(fire_player.bounds, platform.bounds);
        bool water_collides = CheckCollisionRecs(water_player.bounds, platform.bounds);

        // Revisa colisión con el personaje de Fuego
        if (fire_collides) {
            // Si es peligro de agua, game over
            if (platform.type == WATER) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
            }
            
            // Si no es peligro, comportamiento normal de plataforma
            if (platform.type != FIRE) {
                fire_player.velocity.y = 0;
                fire_player.position.y = platform.bounds.y - fire_player.bounds.height;
                fire_player.bounds.y = fire_player.position.y;
                fire_on_ground = true;
            }
        }
        
        // Revisa colisión con el personaje de Agua
        if (water_collides) {
            // Si es peligro de fuego, game over
            if (platform.type == FIRE) {
                this->state_machine->add_state(std::make_unique<GameOverState>(), true);
                return;
            }
            
            // Si no es peligro, comportamiento normal de plataforma
            if (platform.type != WATER) {
                water_player.velocity.y = 0;
                water_player.position.y = platform.bounds.y - water_player.bounds.height;
                water_player.bounds.y = water_player.position.y;
                water_on_ground = true;
            }
        }

        // Detecta si ambos jugadores están en la meta
        if(fire_collides && water_collides && platform.type == GOAL) {
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
}

void MainGameState::render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    mapLoader.draw(camera);

    // Renderizar jugadores
    fire_player.render();
    water_player.render();

    // Dibujar las plataformas del nivel
    for (const auto& platform : level_platforms) {
        if(platform.type == NONE){
            DrawRectangleRec(platform.bounds, GRAY);
        }
        else if(platform.type == GOAL){
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
    
    EndDrawing();
}