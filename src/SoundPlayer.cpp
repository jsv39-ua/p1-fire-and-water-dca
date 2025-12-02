#include <SoundPlayer.hpp>
#include <ResourceManager.hpp>

SoundPlayer::SoundPlayer() : currentSound(0), initialized(false) {
    for (int i = 0; i < MAX_SOUNDS; i++) {
        soundArray[i] = { 0 };
    }
}

SoundPlayer::~SoundPlayer() {
    if (initialized) {
        for (int i = 0; i < MAX_SOUNDS; i++) {
            UnloadSoundAlias(soundArray[i]);
        }
    }
}

void SoundPlayer::init(const std::string& filePath) {
    // Obtenemos el sonido fuente del ResourceManager
    // El ResourceManager mantiene la propiedad de los datos del sonido (buffer)
    Sound source = ResourceManager::getInstance()->loadSound(filePath);

    // Creamos aliases para permitir reproducción simultánea
    // LoadSoundAlias comparte los datos de audio pero tiene su propio stream
    for (int i = 0; i < MAX_SOUNDS; i++) {
        soundArray[i] = LoadSoundAlias(source);
    }

    currentSound = 0;
    initialized = true;
}

void SoundPlayer::play() {
    if (!initialized) return;

    PlaySound(soundArray[currentSound]);
    
    currentSound++;
    if (currentSound >= MAX_SOUNDS) {
        currentSound = 0;
    }
}

void SoundPlayer::setVolume(float volume) {
    if (!initialized) return;
    
    for (int i = 0; i < MAX_SOUNDS; i++) {
        SetSoundVolume(soundArray[i], volume);
    }
}
