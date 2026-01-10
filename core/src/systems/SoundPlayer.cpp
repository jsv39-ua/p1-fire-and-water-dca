#include <systems/SoundPlayer.hpp>
#include <systems/ResourceManager.hpp>
#include <systems/AudioManager.hpp>
#include <cstdlib>  // Para rand()

SoundPlayer::SoundPlayer() : currentSound(0), numVariants(0), initialized(false), registeredInAudioManager(false), currentVolume(1.0f) {
    for (int v = 0; v < MAX_VARIANTS; v++) {
        for (int i = 0; i < MAX_SOUNDS; i++) {
            soundArray[v][i] = { 0 };
        }
    }
}

SoundPlayer::~SoundPlayer() {
    unregisterFromAudioManager();
    cleanup();
}

// Copy constructor
SoundPlayer::SoundPlayer(const SoundPlayer& other) 
    : currentSound(0), numVariants(0), initialized(false), registeredInAudioManager(false), currentVolume(-1.0f) {
    for (int v = 0; v < MAX_VARIANTS; v++) {
        for (int i = 0; i < MAX_SOUNDS; i++) {
            soundArray[v][i] = { 0 };
        }
    }
    
    // Guardar el volumen objetivo
    float targetVolume = other.currentVolume;
    
    // Si el otro está inicializado, inicializamos este con los mismos archivos
    if (other.initialized && !other.loadedFilePaths.empty()) {
        init(other.loadedFilePaths);
        // Aplicar el volumen del otro (currentVolume es -1, así que se aplicará)
        if (initialized) {
            setVolume(targetVolume);
        }
    } else {
        currentVolume = targetVolume;
    }
}

// Copy assignment operator
SoundPlayer& SoundPlayer::operator=(const SoundPlayer& other) {
    if (this != &other) {
        // Desregistrar antes de limpiar
        unregisterFromAudioManager();
        // Limpiar recursos actuales
        cleanup();
        
        // Guardar el volumen pero resetear para forzar aplicación
        float targetVolume = other.currentVolume;
        currentVolume = -1.0f; // Forzar que setVolume aplique el nuevo valor
        
        // Si el otro está inicializado, inicializamos este con los mismos archivos
        if (other.initialized && !other.loadedFilePaths.empty()) {
            init(other.loadedFilePaths);
            // Aplicar el volumen del otro (ahora sí se aplicará porque currentVolume != targetVolume)
            if (initialized) {
                setVolume(targetVolume);
            }
        } else {
            currentVolume = targetVolume;
        }
    }
    return *this;
}

void SoundPlayer::cleanup() {
    if (initialized && IsWindowReady() && IsAudioDeviceReady()) {
        // Descargar los aliases de todas las variantes
        for (int v = 0; v < numVariants; v++) {
            for (int i = 0; i < MAX_SOUNDS; i++) {
                if (soundArray[v][i].stream.buffer != nullptr) {
                    UnloadSoundAlias(soundArray[v][i]);
                    soundArray[v][i] = { 0 };
                }
            }
        }
        initialized = false;
        numVariants = 0;
        loadedFilePaths.clear();
    } else if (initialized) {
        initialized = false;
        numVariants = 0;
        loadedFilePaths.clear();
    }
}

void SoundPlayer::init(const std::string& filePath) {
    // Convertir a vector y llamar al método con múltiples archivos
    std::vector<std::string> paths = { filePath };
    init(paths);
}

void SoundPlayer::init(const std::vector<std::string>& filePaths) {
    // Si ya está inicializado con otros archivos, limpiamos primero
    if (initialized && loadedFilePaths != filePaths) {
        cleanup();
    }
    
    // Si ya está inicializado con los mismos archivos, no hacemos nada
    if (initialized && loadedFilePaths == filePaths) {
        return;
    }
    
    if (filePaths.empty()) {
        TraceLog(LOG_ERROR, "[SoundPlayer] No file paths provided");
        return;
    }
    
    TraceLog(LOG_INFO, "[SoundPlayer] Initializing with %d file(s)", (int)filePaths.size());
    
    // Limitar el número de variantes
    numVariants = (filePaths.size() > MAX_VARIANTS) ? MAX_VARIANTS : filePaths.size();
    
    int loadedVariants = 0;

    // Cargar cada variante de sonido
    for (int v = 0; v < numVariants; v++) {
        TraceLog(LOG_INFO, "[SoundPlayer] Loading variant %d: %s", v, filePaths[v].c_str());
        
        // Obtenemos el sonido fuente del ResourceManager
        Sound source = ResourceManager::getInstance()->loadSound(filePaths[v]);
        
        // Verificar que el sonido se cargó correctamente
        if (!IsSoundValid(source)) {
            TraceLog(LOG_ERROR, "[SoundPlayer] Failed to load sound variant %d: %s", v, filePaths[v].c_str());
            continue;
        }
        
        TraceLog(LOG_INFO, "[SoundPlayer] Source sound valid, creating %d aliases", MAX_SOUNDS);

        // Creamos aliases para permitir reproducción simultánea
        int validAliases = 0;
        for (int i = 0; i < MAX_SOUNDS; i++) {
            soundArray[v][i] = LoadSoundAlias(source);
            if (IsSoundValid(soundArray[v][i])) {
                validAliases++;
            } else {
                TraceLog(LOG_ERROR, "[SoundPlayer] Failed to create sound alias %d for variant %d: %s", i, v, filePaths[v].c_str());
            }
        }
        
        if (validAliases > 0) {
            TraceLog(LOG_INFO, "[SoundPlayer] Created %d valid aliases for variant %d", validAliases, v);
            loadedVariants++;
        }
    }

    if (loadedVariants == 0) {
        TraceLog(LOG_ERROR, "[SoundPlayer] No variants loaded successfully!");
        initialized = false;
        return;
    }

    currentSound = 0;
    initialized = true;
    loadedFilePaths = filePaths;
    
    // Registrar en AudioManager para recibir actualizaciones de volumen
    registerWithAudioManager();
    
    TraceLog(LOG_INFO, "[SoundPlayer] Successfully initialized with %d variants and %d channels per variant", loadedVariants, MAX_SOUNDS);
}

void SoundPlayer::play() {
    if (!initialized || numVariants == 0) {
        TraceLog(LOG_WARNING, "[SoundPlayer] Attempted to play uninitialized SoundPlayer");
        return;
    }

    // Elegir una variante aleatoria
    int variant = rand() % numVariants;
    
    if (!IsSoundValid(soundArray[variant][currentSound])) {
        TraceLog(LOG_WARNING, "[SoundPlayer] Cannot play: sound alias %d of variant %d is invalid", currentSound, variant);
        return;
    }

    PlaySound(soundArray[variant][currentSound]);
    
    // Avanzar al siguiente canal (round-robin)
    currentSound++;
    if (currentSound >= MAX_SOUNDS) {
        currentSound = 0;
    }
}

void SoundPlayer::setVolume(float volume) {
    // Evitar actualizaciones redundantes si el volumen no cambió
    if (currentVolume == volume) {
        return;
    }
    
    currentVolume = volume;
    
    if (!initialized) {
        return;
    }
    
    // Aplicar volumen a todos los canales de todas las variantes
    for (int v = 0; v < numVariants; v++) {
        for (int i = 0; i < MAX_SOUNDS; i++) {
            if (IsSoundValid(soundArray[v][i])) {
                SetSoundVolume(soundArray[v][i], volume);
            }
        }
    }
}

void SoundPlayer::registerWithAudioManager() {
    if (!registeredInAudioManager && initialized) {
        AudioManager::getInstance()->registerSoundPlayer(this);
        registeredInAudioManager = true;
    }
}

void SoundPlayer::unregisterFromAudioManager() {
    if (registeredInAudioManager) {
        AudioManager::getInstance()->unregisterSoundPlayer(this);
        registeredInAudioManager = false;
    }
}