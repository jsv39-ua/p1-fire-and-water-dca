#include <AudioManager.hpp>
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
    // Cargar y configurar música de fondo
    backgroundMusic = LoadMusicStream("assets/music/backgroundMusic.mp3");
    PlayMusicStream(backgroundMusic); // Iniciar la reproducción de la música
    backgroundMusic.looping = true; // Hacer que la música se repita en bucle
    SetMusicVolume(backgroundMusic, musicVolumeLevel); // Volumen inicial al 25%

    // Cargar sonidos de botones
    buttonClickSound = LoadSound("assets/ui_sounds/boton_click.ogg");
    buttonHoverSound = LoadSound("assets/ui_sounds/boton_hover.ogg");
    
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

    if (IsSoundValid(buttonClickSound)) {
        UnloadSound(buttonClickSound);
    }
    if (IsSoundValid(buttonHoverSound)) {
        UnloadSound(buttonHoverSound);
    }
    if (IsMusicValid(backgroundMusic)) {
        UnloadMusicStream(backgroundMusic);
    }
}

