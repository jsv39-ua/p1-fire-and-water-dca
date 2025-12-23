#include <systems/AudioManager.hpp>
#include <systems/ResourceManager.hpp>
#include <iostream>
#include <raylib.h>

// De esta clase se crea una instancia para poder cargar los sonidos

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() {
    
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
    //no descargo porque se queda en cache
}

