#include <SoundPlayer.hpp>
#include <ResourceManager.hpp>

SoundPlayer::SoundPlayer() : currentSound(0), initialized(false), loadedFilePath(""), currentVolume(1.0f) {
    for (int i = 0; i < MAX_SOUNDS; i++) {
        soundArray[i] = { 0 };
    }
}

SoundPlayer::~SoundPlayer() {
    cleanup();
}

// Copy constructor
SoundPlayer::SoundPlayer(const SoundPlayer& other) 
    : currentSound(0), initialized(false), loadedFilePath(""), currentVolume(other.currentVolume) {
    for (int i = 0; i < MAX_SOUNDS; i++) {
        soundArray[i] = { 0 };
    }
    
    // Si el otro está inicializado, inicializamos este con el mismo archivo
    if (other.initialized && !other.loadedFilePath.empty()) {
        init(other.loadedFilePath);
        // Aplicar el volumen del otro
        if (initialized) {
            setVolume(other.currentVolume);
        }
    }
}

// Copy assignment operator
SoundPlayer& SoundPlayer::operator=(const SoundPlayer& other) {
    if (this != &other) {
        // Limpiar recursos actuales
        cleanup();
        
        currentVolume = other.currentVolume;
        
        // Si el otro está inicializado, inicializamos este con el mismo archivo
        if (other.initialized && !other.loadedFilePath.empty()) {
            init(other.loadedFilePath);
            // Aplicar el volumen del otro
            if (initialized) {
                setVolume(other.currentVolume);
            }
        }
    }
    return *this;
}

void SoundPlayer::cleanup() {
    if (initialized) {
        // Descargar los aliases (solo los streams, no los datos del buffer)
        for (int i = 0; i < MAX_SOUNDS; i++) {
            if (soundArray[i].stream.buffer != nullptr) {
                UnloadSoundAlias(soundArray[i]);
                soundArray[i] = { 0 };
            }
        }
        initialized = false;
        loadedFilePath = "";
    }
}

void SoundPlayer::init(const std::string& filePath) {
    // Si ya está inicializado con otro archivo, limpiamos primero
    if (initialized && loadedFilePath != filePath) {
        cleanup();
    }
    
    // Si ya está inicializado con el mismo archivo, no hacemos nada
    if (initialized && loadedFilePath == filePath) {
        return;
    }

    // Obtenemos el sonido fuente del ResourceManager
    // El ResourceManager mantiene la propiedad de los datos del sonido (buffer)
    Sound source = ResourceManager::getInstance()->loadSound(filePath);
    
    // Verificar que el sonido se cargó correctamente
    if (!IsSoundValid(source)) {
        TraceLog(LOG_ERROR, "[SoundPlayer] Failed to load sound: %s", filePath.c_str());
        return;
    }

    // Creamos aliases para permitir reproducción simultánea
    // LoadSoundAlias comparte los datos de audio pero tiene su propio stream
    for (int i = 0; i < MAX_SOUNDS; i++) {
        soundArray[i] = LoadSoundAlias(source);
        if (!IsSoundValid(soundArray[i])) {
            TraceLog(LOG_ERROR, "[SoundPlayer] Failed to create sound alias %d for: %s", i, filePath.c_str());
        }
    }

    currentSound = 0;
    initialized = true;
    loadedFilePath = filePath;
    
    TraceLog(LOG_INFO, "[SoundPlayer] Initialized with %d sound channels for: %s", MAX_SOUNDS, filePath.c_str());
}

void SoundPlayer::play() {
    if (!initialized) {
        TraceLog(LOG_WARNING, "[SoundPlayer] Cannot play: not initialized");
        return;
    }

    if (!IsSoundValid(soundArray[currentSound])) {
        TraceLog(LOG_WARNING, "[SoundPlayer] Cannot play: sound alias %d is invalid", currentSound);
        return;
    }

    PlaySound(soundArray[currentSound]);
    TraceLog(LOG_INFO, "[SoundPlayer] Playing sound on channel %d", currentSound);
    
    currentSound++;
    if (currentSound >= MAX_SOUNDS) {
        currentSound = 0;
    }
}

void SoundPlayer::setVolume(float volume) {
    if (!initialized) return;
    
    currentVolume = volume;  // Guardar el volumen actual
    
    for (int i = 0; i < MAX_SOUNDS; i++) {
        SetSoundVolume(soundArray[i], volume);
    }
}
