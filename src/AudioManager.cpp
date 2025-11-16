#include <AudioManager.hpp>
#include <ResourceManager.hpp>
#include <iostream>

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
    
    // Configurar y reproducir musica de fondo
    if (IsMusicValid(backgroundMusic)) {
        PlayMusicStream(backgroundMusic);
        backgroundMusic.looping = true;
        SetMusicVolume(backgroundMusic, musicVolumeLevel);
    }
    
    // Verificar validez de los sonidos
    if (!IsSoundValid(buttonClickSound)) {
        std::cerr << "Error: No se pudo cargar boton_click.ogg" << std::endl;
    }
    if (!IsSoundValid(buttonHoverSound)) {
        std::cerr << "Error: No se pudo cargar boton_hover.ogg" << std::endl;
    }
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

void AudioManager::setMusicVolume(float volume) {
    if (IsMusicValid(backgroundMusic)) {
        SetMusicVolume(backgroundMusic, volume);
        musicVolumeLevel = volume * 100;
    }
}

    
float AudioManager::getMusicVolume() {
    
    return this->musicVolumeLevel;
    
}



AudioManager::~AudioManager() {
    //no descargo porque se queda en cache
}

