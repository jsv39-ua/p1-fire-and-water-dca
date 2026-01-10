// Como raylib no soporta reproducir varios sonidos simultaneos, usamos esta clase para simular varios canales de sonido


#pragma once

#include <string>
#include <vector>

extern "C" {
    #include <raylib.h>
}

class SoundPlayer {

    public:

        SoundPlayer();
        ~SoundPlayer();
        
        // Copy constructor y copy assignment operator
        SoundPlayer(const SoundPlayer& other);
        SoundPlayer& operator=(const SoundPlayer& other);

        // Inicializa el sistema de audio concurrente cargando el sonido desde el archivo
        void init(const std::string& filePath);
        
        // Inicializa con múltiples archivos de sonido (se elegirá uno aleatorio al reproducir)
        void init(const std::vector<std::string>& filePaths);
        
        // Reproduce el sonido usando uno de los canales disponibles
        void play();
        
        // Ajusta el volumen de todos los canales
        void setVolume(float volume);
        
        // Verifica si el SoundPlayer está inicializado
        bool isInitialized() const { return initialized; }

    private:
        static const int MAX_SOUNDS = 10;
        static const int MAX_VARIANTS = 5;  // Máximo de variantes de sonido
        Sound soundArray[MAX_VARIANTS][MAX_SOUNDS];  // [variante][canal]
        int currentSound;
        int numVariants;  // Número de variantes cargadas
        bool initialized;
        bool registeredInAudioManager;  // Para saber si está registrado
        std::vector<std::string> loadedFilePaths;
        float currentVolume;  // Rastrear el volumen actual
        
        // Método privado para limpiar recursos
        void cleanup();
        
        // Métodos registro en AudioManager
        void registerWithAudioManager();
        void unregisterFromAudioManager();
};



