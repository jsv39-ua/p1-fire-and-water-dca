#include <Animation.hpp>

Animation::Animation() : 
    primerFrame(0), 
    ultimoFrame(0), 
    frameActual(0), 
    frameCount(0), 
    frameTime(0.1f), 
    currentTime(0.0f),
    isLooping(true),
    sprite({0})
{}

Animation::Animation(int frameCount, float frameTime, bool isLooping, Texture2D sprite, int primerFrame, int ultimoFrame) :
    frameCount(frameCount),
    frameTime(frameTime),
    isLooping(isLooping),
    sprite(sprite),
    primerFrame(primerFrame),
    ultimoFrame(ultimoFrame),
    frameActual(primerFrame),
    currentTime(0.0f)
{}

Animation::~Animation(){}

void Animation::animationUpdate(float deltaTime){
    currentTime += deltaTime;
    
    // Cambiar de frame cuando pase el tiempo necesario
    if (currentTime >= frameTime) {
        currentTime = 0.0f;
        frameActual++;
        
        // Si llegamos al último frame
        if (frameActual > ultimoFrame) {
            if (isLooping) {
                frameActual = primerFrame; // Volver al inicio
            } else {
                frameActual = ultimoFrame; // Quedarse en el último frame
            }
        }
    }
}

// Tamaño tileset debe ser la resolución de cada frame individual (ej: 32x32)
Rectangle Animation::animationFrame(int frames_por_columna, int tamanyo_tileset){
    float x = (frameActual % frames_por_columna) * tamanyo_tileset;
    float y = (frameActual / frames_por_columna) * tamanyo_tileset;
    
    return Rectangle{x, y, (float)tamanyo_tileset, (float)tamanyo_tileset};
}

void Animation::reset() {
    frameActual = primerFrame;
    currentTime = 0.0f;
}

int Animation::getCurrentFrame() const {
    return frameActual;
}

    