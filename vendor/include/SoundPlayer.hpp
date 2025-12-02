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

        // Inicializa el sistema de audio concurrente cargando el sonido desde el archivo
        void init(const std::string& filePath);
        
        // Reproduce el sonido usando uno de los canales disponibles
        void play();
        
        // Ajusta el volumen de todos los canales
        void setVolume(float volume);

    private:
        static const int MAX_SOUNDS = 10;
        Sound soundArray[MAX_SOUNDS];
        int currentSound;
        bool initialized;
};



