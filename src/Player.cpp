#include <MainGameState.hpp>
#include <iostream>
<<<<<<< HEAD
#include <ResourceManager.hpp>
#include <AudioManager.hpp>
#include <filesystem>
#include <vector>
=======

>>>>>>> main

extern "C" {
    #include <raylib.h>
}

#include <Player.hpp>

Player::Player() : Entity() {
    key_left = 0;
    key_right = 0;
    key_jump = 0;
    jump_hold_timer = 0.0f;
<<<<<<< HEAD
    isWalkMusicPlaying = false;
    
    jumpSound.init("assets/characters/sounds/Hero Damage.mp3");
    jumpSound.setVolume(1.0f);
=======
>>>>>>> main
    life_count = 3;
    invincibility_timer = 0.0f;
}

Player::Player(float x, float y, float height, float width, ElementType t) 
    : Entity({x, y}, {0.0f, 0.0f}, height, width, t) {
    // Inicializar teclas a 0 por defecto
    key_left = 0;
    key_right = 0;
    key_jump = 0;
    jump_hold_timer = 0.0f;
<<<<<<< HEAD
    isWalkMusicPlaying = false;

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
            
            // Fallback: si no hay sonidos específicos de agua, usar los de fuego
            if (sonidosJump.empty()) {
                loadSoundsFromDirectory("assets/characters/sounds/fire_sounds/jump", sonidosJump);
                TraceLog(LOG_INFO, "[Player] Water player using fire jump sounds as fallback");
            }
            if (sonidosDamage.empty()) {
                loadSoundsFromDirectory("assets/characters/sounds/fire_sounds/damage", sonidosDamage);
                TraceLog(LOG_INFO, "[Player] Water player using fire damage sounds as fallback");
            }
            if (sonidosDeath.empty()) {
                loadSoundsFromDirectory("assets/characters/sounds/fire_sounds/death", sonidosDeath);
                TraceLog(LOG_INFO, "[Player] Water player using fire death sounds as fallback");
            }
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
        jumpSound.setVolume(1.0f);
    }
    if (!sonidosDeath.empty()) {
        deathSound.init(sonidosDeath);
    }
    
    walkMusic = ResourceManager::getInstance()->loadMusic("assets/characters/sounds/Hero Walk Footsteps Stone.mp3");
    
    if (IsMusicValid(walkMusic)) {
        walkMusic.looping = true;
        // Establecer volumen alto para los pasos - no usar volumen de SFX porque es música
        SetMusicVolume(walkMusic, 0.8f); // Volumen directo sin multiplicadores
        TraceLog(LOG_INFO, "[Player] Walk music loaded successfully with volume: 0.8");
    } else {
        TraceLog(LOG_ERROR, "[Player] Failed to load walk music");
    }
=======
>>>>>>> main
    life_count = 3;
    invincibility_timer = 0.0f;
}

void Player::setKeys(int left, int right, int jump) {
    key_left = left;
    key_right = right;
    key_jump = jump;
}

void Player::update(float deltaTime) {
    // Actualizar timer de invencibilidad
    if (invincibility_timer > 0.0f) {
        invincibility_timer -= deltaTime;
        if (invincibility_timer < 0.0f) {
            invincibility_timer = 0.0f;
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
*/
bool Player::resolvePlatformCollision(const Platform & platform) {
    // Solo corregir si viene desde arriba (cayendo)
    if (this->velocity.y > 0) {
        float playerBottom = this->position.y + this->height;
        // Verificar que el jugador está encima de la plataforma (no dentro por los lados)
        if (playerBottom > platform.bounds.y && playerBottom < platform.bounds.y + platform.bounds.height) {
            this->velocity.y = 0;
            this->position.y = platform.bounds.y - this->bounds.height;
            this->bounds.y = this->position.y;
            return true; // Está en el suelo
        }
    }
    return false; // No está en el suelo
}


/*
    Los valores que se interpretan de la colisión son:
     - 0: Colisión con plataforma neutra
     - 1: Colisión con plataforma de contraria
     - 2: Colisión con plataforma amiga
     - 3: Colsión con plataforma meta
     - 4: No hay colisión
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