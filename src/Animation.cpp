#include <Animation.hpp>




Animation::Animation(){}

Animation::Animation(int frameCount, float frameTime, bool isLooping, Texture2D sprite, int primerFrame, int ultimoFrame){
    this->frameCount = frameCount;
    this->frameTime = frameTime;
    this->primerFrame = primerFrame;
    this->ultimoFrame = ultimoFrame;

}

Animation::~Animation(){}

void Animation::animationUpdate(float deltaTime){
    
}


// Tamaño tileset debe ser la resolucion del sprite del tileset
Rectangle Animation::animationFrame(int frames_por_columna, int tamanyo_tileset){
    float x = this->frameActual % frames_por_columna;
    float y = this->frameActual / frames_por_columna;
}

    