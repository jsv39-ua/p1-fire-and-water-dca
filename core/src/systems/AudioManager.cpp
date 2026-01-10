#include <systems/AudioManager.hpp>
#include <systems/ResourceManager.hpp>
#include <systems/SoundPlayer.hpp>
#include <iostream>
#include <algorithm>
#include <raylib.h>

// De esta clase se crea una instancia para poder cargar los sonidos

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() {
    // Inicializar structs a cero para evitar crashes en IsMusicValid/IsSoundValid
    backgroundMusic = {};
    buttonClickSound = {};
    buttonHoverSound = {};
}

AudioManager* AudioManager::getInstance() {
    if (instance == nullptr) {
        instance = new AudioManager();
    }
    return instance;
}

void AudioManager::cleanup() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void AudioManager::init() {
    // Cargar musica y sonidos desde ResourceManager
    backgroundMusic = ResourceManager::getInstance()->loadMusic("assets/music/backgroundMusic.mp3");
    buttonClickSound = ResourceManager::getInstance()->loadSound("assets/ui_sounds/boton_click.ogg");
    buttonHoverSound = ResourceManager::getInstance()->loadSound("assets/ui_sounds/boton_hover.ogg");
    
    // Verificar validez de los sonidos
    if (!IsSoundValid(buttonClickSound)) {
        std::cerr << "Error: No se pudo cargar boton_click.ogg" << std::endl;
    }
    if (!IsSoundValid(buttonHoverSound)) {
        std::cerr << "Error: No se pudo cargar boton_hover.ogg" << std::endl;
    }
    
    // Configurar y reproducir musica de fondo
    if (IsMusicValid(backgroundMusic)) {
        PlayMusicStream(backgroundMusic);
        backgroundMusic.looping = true;
    }
    
    // Aplicar volúmenes iniciales
    updateMusicVolume();
    updateSfxVolumes();
}

void AudioManager::playButtonClick() {
    if (IsSoundValid(buttonClickSound)) {
        PlaySound(buttonClickSound);
    }
}

void AudioManager::playButtonHover() {
    if (IsSoundValid(buttonHoverSound)) {
        PlaySound(buttonHoverSound);
    }
}

void AudioManager::updateMusicStream() {
    if (IsMusicValid(backgroundMusic)) {
        UpdateMusicStream(backgroundMusic);
    }
}

// Métodos privados de actualización de volúmenes
void AudioManager::updateMusicVolume() {
    float finalVolume = masterVolume * musicBaseVolume;
    if (IsMusicValid(backgroundMusic)) {
        SetMusicVolume(backgroundMusic, finalVolume);
    }
}

void AudioManager::updateSfxVolumes() {
    float finalVolume = masterVolume * sfxBaseVolume;
    if (IsSoundValid(buttonClickSound)) {
        SetSoundVolume(buttonClickSound, finalVolume);
    }
    if (IsSoundValid(buttonHoverSound)) {
        SetSoundVolume(buttonHoverSound, finalVolume);
    }
    // Actualizar todos los SoundPlayers registrados
    updateRegisteredSoundPlayers();
    // Actualizar todos los walkMusic registrados
    updateRegisteredWalkMusic();
}

// Setters de volumen con validación de rango
void AudioManager::setMasterVolume(float volume) {
    // Clamp manual para asegurar rango [0.0, 1.0]
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
    masterVolume = volume;
    
    // Actualizar ambos tipos de audio
    updateMusicVolume();
    updateSfxVolumes();
}

void AudioManager::setMusicVolume(float volume) {
    // Clamp manual para asegurar rango [0.0, 1.0]
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
    musicBaseVolume = volume;
    updateMusicVolume();
}

void AudioManager::setSfxVolume(float volume) {
    // Clamp manual para asegurar rango [0.0, 1.0]
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
    sfxBaseVolume = volume;
    updateSfxVolumes();
}

// Getters de volumen
float AudioManager::getMasterVolume() const {
    return masterVolume;
}

float AudioManager::getMusicVolume() const {
    return musicBaseVolume;
}

float AudioManager::getSfxVolume() const {
    return sfxBaseVolume;
}



AudioManager::~AudioManager() {
    registeredSoundPlayers.clear();
    registeredWalkMusic.clear();
}

void AudioManager::registerSoundPlayer(SoundPlayer* player) {
    if (player == nullptr) return;
    
    // Evitar duplicados
    auto it = std::find(registeredSoundPlayers.begin(), registeredSoundPlayers.end(), player);
    if (it == registeredSoundPlayers.end()) {
        registeredSoundPlayers.push_back(player);
        // Aplicar volumen actual inmediatamente
        float finalVolume = masterVolume * sfxBaseVolume;
        player->setVolume(finalVolume);
        TraceLog(LOG_INFO, "[AudioManager] SoundPlayer registrado. Total: %d", (int)registeredSoundPlayers.size());
    }
}

void AudioManager::unregisterSoundPlayer(SoundPlayer* player) {
    if (player == nullptr) return;
    
    auto it = std::find(registeredSoundPlayers.begin(), registeredSoundPlayers.end(), player);
    if (it != registeredSoundPlayers.end()) {
        registeredSoundPlayers.erase(it);
        TraceLog(LOG_INFO, "[AudioManager] SoundPlayer desregistrado. Total: %d", (int)registeredSoundPlayers.size());
    }
}

void AudioManager::updateRegisteredSoundPlayers() {
    float finalVolume = masterVolume * sfxBaseVolume;
    for (SoundPlayer* player : registeredSoundPlayers) {
        if (player != nullptr && player->isInitialized()) {
            player->setVolume(finalVolume);
        }
    }
}

void AudioManager::registerWalkMusic(Music* music) {
    if (music == nullptr) return;
    
    // Evitar duplicados
    auto it = std::find(registeredWalkMusic.begin(), registeredWalkMusic.end(), music);
    if (it == registeredWalkMusic.end()) {
        registeredWalkMusic.push_back(music);
        // Aplicar volumen actual inmediatamente
        float finalVolume = masterVolume * sfxBaseVolume;
        if (IsMusicValid(*music)) {
            SetMusicVolume(*music, finalVolume);
        }
        TraceLog(LOG_INFO, "[AudioManager] walkMusic registrado. Total: %d", (int)registeredWalkMusic.size());
    }
}

void AudioManager::unregisterWalkMusic(Music* music) {
    if (music == nullptr) return;
    
    auto it = std::find(registeredWalkMusic.begin(), registeredWalkMusic.end(), music);
    if (it != registeredWalkMusic.end()) {
        registeredWalkMusic.erase(it);
        TraceLog(LOG_INFO, "[AudioManager] walkMusic desregistrado. Total: %d", (int)registeredWalkMusic.size());
    }
}

void AudioManager::updateRegisteredWalkMusic() {
    float finalVolume = masterVolume * sfxBaseVolume;
    for (Music* music : registeredWalkMusic) {
        if (music != nullptr && IsMusicValid(*music)) {
            SetMusicVolume(*music, finalVolume);
        }
    }
}

