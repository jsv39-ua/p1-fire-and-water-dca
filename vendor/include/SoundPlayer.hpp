// Como raylib no soporta reproducir varios sonidos simultaneos, usamos esta clase para simular varios canales de sonido


#pragma once

#include <string>

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
        
        // Reproduce el sonido usando uno de los canales disponibles
        void play();
        
        // Ajusta el volumen de todos los canales
        void setVolume(float volume);
        
        // Verifica si el SoundPlayer está inicializado
        bool isInitialized() const { return initialized; }

    private:
        static const int MAX_SOUNDS = 10;
        Sound soundArray[MAX_SOUNDS];
        int currentSound;
        bool initialized;
        std::string loadedFilePath;
        float currentVolume;  // Rastrear el volumen actual
        
        // Método privado para limpiar recursos
        void cleanup();
};



