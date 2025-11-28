// Como raylib no soporta reproducir varios sonidos simultaneos, usamos esta clase para simular varios canales de sonido


#pragma once

extern "C" {
    #include <raylib.h>
}



class SoundPlayer {

    public:

        SoundPlayer();
        ~SoundPlayer();


};



