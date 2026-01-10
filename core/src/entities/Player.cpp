#include <states/MainGameState.hpp>
#include <iostream>
#include <systems/ResourceManager.hpp>
#include <systems/AudioManager.hpp>
#include <filesystem>
#include <vector>

extern "C" {
    #include <raylib.h>
}

#include <entities/Player.hpp>
Player::Player() : Entity() {
    key_left = 0;
    key_right = 0;
    key_jump = 0;
    key_attack = 0;
    jump_hold_timer = 0.0f;
    isWalkMusicPlaying = false;
    life_count = 3;
    invincibility_timer = 0.0f;
    attack_cooldown = 0.0f;
    log_timer = 0.0f;
    walkMusic = {0};
}

Player::~Player() {
    // Desregistrar walkMusic del AudioManager
    if (walkMusic.ctxData != nullptr) {
        AudioManager::getInstance()->unregisterWalkMusic(&walkMusic);
    }
    
    // Solo intentar detener la música si el audio está disponible, la ventana está abierta 
    // y walkMusic fue realmente cargada
    if (IsWindowReady() && IsAudioDeviceReady() && walkMusic.ctxData != nullptr) {
        StopMusicStream(walkMusic);
    }
}

Player::Player(float x, float y, float height, float width, ElementType t) 
    : Entity({x, y}, {0.0f, 0.0f}, height, width, t) {
    // Inicializar teclas a 0 por defecto
    key_left = 0;
    key_right = 0;
    key_jump = 0;
    key_attack = 0;
    jump_hold_timer = 0.0f;
    isWalkMusicPlaying = false;
    log_timer = 0.0f;

    // Función helper para cargar todos los archivos de una carpeta
    auto loadSoundsFromDirectory = [](const std::string& path, std::vector<std::string>& soundList) {
        try {
            if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                    if (entry.is_regular_file()) {
                        std::string extension = entry.path().extension().string();
                        // Verificar que sea un archivo de audio válido
                        if (extension == ".mp3" || extension == ".ogg" || extension == ".wav") {
                            soundList.push_back(entry.path().string());
                        }
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            TraceLog(LOG_WARNING, "[Player] Error reading directory %s: %s", path.c_str(), e.what());
        }
    };

    std::vector<std::string> sonidosDamage;
    std::vector<std::string> sonidosJump;
    std::vector<std::string> sonidosDeath;

    switch (t) {
        case FIRE:
            loadSoundsFromDirectory("assets/characters/sounds/fire_sounds/damage", sonidosDamage);
            loadSoundsFromDirectory("assets/characters/sounds/fire_sounds/jump", sonidosJump);
            loadSoundsFromDirectory("assets/characters/sounds/fire_sounds/death", sonidosDeath);
            break;
        case WATER:
            loadSoundsFromDirectory("assets/characters/sounds/water_sounds/damage", sonidosDamage);
            loadSoundsFromDirectory("assets/characters/sounds/water_sounds/jump", sonidosJump);
            loadSoundsFromDirectory("assets/characters/sounds/water_sounds/death", sonidosDeath);
        
            break;
        default:
            // Fallback para tipo NONE
            sonidosJump.push_back("assets/characters/sounds/Hero Jump.mp3");
            break;
    }

    // Inicializar sonidos solo si hay archivos cargados
    if (!sonidosDamage.empty()) {
        damageSound.init(sonidosDamage);
    }
    if (!sonidosJump.empty()) {
        jumpSound.init(sonidosJump);
    }
    if (!sonidosDeath.empty()) {
        deathSound.init(sonidosDeath);
    }
    
    walkMusic = ResourceManager::getInstance()->loadMusic("assets/characters/sounds/Hero Walk Footsteps Stone.mp3");
    
    if (IsMusicValid(walkMusic)) {
        walkMusic.looping = true;
        AudioManager::getInstance()->registerWalkMusic(&walkMusic);
        TraceLog(LOG_INFO, "[Player] Walk music loaded and registered with AudioManager");
    } else {
        TraceLog(LOG_ERROR, "[Player] Failed to load walk music");
    }
    life_count = 3;
    invincibility_timer = 0.0f;
    attack_cooldown = 0.0f;
}

void Player::setKeys(int left, int right, int jump, int attack) {
    key_left = left;
    key_right = right;
    key_jump = jump;
    key_attack = attack;
}

void Player::update(float deltaTime) {
    // Logging periódico en modo debug
    #ifdef DEBUG
    log_timer += deltaTime;
    if (log_timer >= LOG_INTERVAL) {
        log_timer = 0.0f;
        TraceLog(LOG_INFO, "[Player %s] Pos: (%.2f, %.2f), Vel: (%.2f, %.2f), Lives: %d, Invincible: %s", 
            (type == FIRE ? "FIRE" : (type == WATER ? "WATER" : "NONE")),
            position.x, position.y,
            velocity.x, velocity.y,
            life_count,
            (isInvincible() ? "YES" : "NO"));
    }
    #endif

    // Actualizar timer de invencibilidad
    if (invincibility_timer > 0.0f) {
        invincibility_timer -= deltaTime;
        if (invincibility_timer < 0.0f) {
            invincibility_timer = 0.0f;
        }
    }
    
    // Actualizar cooldown de ataque
    if (attack_cooldown > 0.0f) {
        attack_cooldown -= deltaTime;
        if (attack_cooldown < 0.0f) {
            attack_cooldown = 0.0f;
        }
    }
    
    // Aplicar gravedad y actualizar posición basada en velocidad
    static const float gravity = 800.0f;
    static const float MAX_FALL_SPEED = 900.0f;
    
    velocity.y += gravity * deltaTime;
    
    // Limitar velocidad de caída para evitar tunneling
    if (velocity.y > MAX_FALL_SPEED) {
        velocity.y = MAX_FALL_SPEED;
    }

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    // Actualizar bounds
    bounds.x = position.x;
    bounds.y = position.y;
    
    // Actualizar música de caminar
    if (IsMusicValid(walkMusic)) {
        UpdateMusicStream(walkMusic);
    }
}

void Player::render() {
    Color color = (type == FIRE) ? RED : BLUE;
    DrawRectangleRec(bounds, color);
}

void Player::walk(int direction) {
    // direction: -1 = izquierda, 0 = parar, 1 = derecha
    if (direction == -1) {
        velocity.x = -WALK_SPEED;
    } else if (direction == 1) {
        velocity.x = WALK_SPEED;
    } else {
        velocity.x = 0.0f;
    }
}

void Player::jump() {
    velocity.y = -JUMP_FORCE;
    jump_hold_timer = 0.0f; // Reiniciar timer al saltar
}

void Player::attack() {
    if (canAttack()) {
        attack_cooldown = ATTACK_COOLDOWN;
        // El proyectil se creará en MainGameState
    }
}

void Player::updateJump(float deltaTime, bool jumpKeyHeld) {
    if (jumpKeyHeld && velocity.y < 0 && jump_hold_timer < MAX_JUMP_HOLD_TIME) {
        velocity.y -= JUMP_BOOST * deltaTime; 
        jump_hold_timer += deltaTime;
    }
}

void Player::updateLifeCount(int change) {
    life_count += change;
    
    if (life_count < 0) {
        life_count = 0;
    }
    
    // Activar invencibilidad cuando pierde vida
    if (change < 0) {
        activateInvincibility();
    }
}

/*
    Resuelve la física de colisión con una plataforma sólida.
    Retorna true si el jugador está sobre la plataforma (en el suelo).
    
    Este método determina desde qué dirección viene la colisión y resuelve
    el solapamiento de manera apropiada para cada caso.
*/
bool Player::resolvePlatformCollision(const Platform & platform) {
    // Calcular el solapamiento en cada eje
    float overlapLeft = (this->bounds.x + this->bounds.width) - platform.bounds.x;
    float overlapRight = (platform.bounds.x + platform.bounds.width) - this->bounds.x;
    float overlapTop = (this->bounds.y + this->bounds.height) - platform.bounds.y;
    float overlapBottom = (platform.bounds.y + platform.bounds.height) - this->bounds.y;
    
    // Encontrar el solapamiento mínimo para determinar la dirección de la colisión
    float minOverlap = fminf(fminf(overlapLeft, overlapRight), fminf(overlapTop, overlapBottom));
    
    // Definir un umbral de prioridad para colisiones laterales durante saltos
    const float LATERAL_PRIORITY_THRESHOLD = 3.0f;
    
    bool isOnGround = false;
    
    // Si estamos saltando (velocity.y < 0), priorizar colisiones laterales sobre verticales
    // para evitar que colisiones ambiguas corten el salto
    if (this->velocity.y < 0) {
        float minHorizontal = fminf(overlapLeft, overlapRight);
        if (minHorizontal < overlapTop + LATERAL_PRIORITY_THRESHOLD && 
            minHorizontal < overlapBottom + LATERAL_PRIORITY_THRESHOLD) {
            minOverlap = minHorizontal;
        }
    }
    
    // Resolver según la dirección con menor solapamiento
    if (minOverlap == overlapTop && this->velocity.y > 0) {
        // Colisión desde ARRIBA (jugador cayendo sobre plataforma)
        this->position.y = platform.bounds.y - this->bounds.height;
        this->velocity.y = 0;
        isOnGround = true;
    }
    else if (minOverlap == overlapBottom && this->velocity.y < 0) {
        // Colisión desde ABAJO (jugador saltando contra plataforma)
        this->position.y = platform.bounds.y + platform.bounds.height;
        this->velocity.y = 0;
    }
    else if (minOverlap == overlapLeft) {
        // Colisión desde la IZQUIERDA (jugador moviéndose a la derecha)
        this->position.x = platform.bounds.x - this->bounds.width;
        // Solo detener velocidad horizontal si NO está en medio de un salto activo
        if (this->velocity.x > 0 && this->velocity.y >= -50.0f) {
            this->velocity.x = 0;
        }
    }
    else if (minOverlap == overlapRight) {
        // Colisión desde la DERECHA (jugador moviéndose a la izquierda)
        this->position.x = platform.bounds.x + platform.bounds.width;
        // Solo detener velocidad horizontal si NO está en medio de un salto activo
        if (this->velocity.x < 0 && this->velocity.y >= -50.0f) {
            this->velocity.x = 0;
        }
    }
    
    // Actualizar bounds después de corregir la posición
    this->bounds.x = this->position.x;
    this->bounds.y = this->position.y;
    
    return isOnGround;
}


/*
    Los valores que se interpretan de la colisión son:
     - 0: Colisión con plataforma neutra
     - 1: Colisión con plataforma de contraria
     - 2: Colisión con plataforma amiga
     - 3: Colisión con plataforma meta
     - 4: No hay colisión
     - 5: Colisión con botón
     - 6: Colisión con cartel
     - -1: Error
*/
int Player::checkPlatformCollision(const Platform & platform) const {

    bool collisionX = this->bounds.x + this->bounds.width >= platform.bounds.x &&
                      platform.bounds.x + platform.bounds.width >= this->bounds.x;
    bool collisionY = this->bounds.y + this->bounds.height >= platform.bounds.y &&
                      platform.bounds.y + platform.bounds.height >= this->bounds.y;

    if(collisionX && collisionY){
        switch (platform.type) {
            case NONE:
                return 0;
            case GOAL:
                return 3;
            case FIRE:
                return (this->type == FIRE) ? 2 : 1;
            case WATER:
                return (this->type == WATER) ? 2 : 1;
            case BUTTON:
                return 5;
            case SIGN:
                return 6;
            default:
                return -1; // Será una plataforma con tipo inválido
        }
    }


    return 4;
}

// Determina si el jugador debe ser visible (para el efecto de parpadeo)
bool Player::shouldRenderVisible() const {
    if (!isInvincible()) {
        return true; // Siempre visible si no está invencible
    }
    
    // Usar el tiempo del juego para un parpadeo consistente
    // Parpadea cada BLINK_INTERVAL segundos
    float currentTime = GetTime();
    int blinkCycle = (int)(currentTime / BLINK_INTERVAL);
    return (blinkCycle % 2) == 0; // Alterna entre visible/invisible
}