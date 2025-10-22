#include <AudioManager.hpp>
#include <iostream>


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

AudioManager::~AudioManager() {

    if (IsSoundValid(buttonClickSound)) {
        UnloadSound(buttonClickSound);
    }
    if (IsSoundValid(buttonHoverSound)) {
        UnloadSound(buttonHoverSound);
    }
}