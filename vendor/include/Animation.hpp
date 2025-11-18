#pragma once

extern "C" {
    #include <raylib.h>
}

class Animation{

    public: 
        Animation();
        Animation(int frameCount, float frameTime, bool isLooping, Texture2D sprite, int primerFrame, int ultimoFrame);
        ~Animation();

        void animationUpdate(float deltaTime);

        Rectangle animationFrame(int frames_por_columna, int tamanyo_tileset);

        void dibujarAnimacion();


    private:
        int primerFrame;
        int ultimoFrame;
        int frameActual;

        int frameCount;
        float frameTime;


};