#include <components/Animation.hpp>
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

// Calcular el frame actual del spritesheet
Rectangle Animation::animationFrame(int frames_por_columna, int frame_width, int frame_height){
    float x = (frameActual % frames_por_columna) * frame_width;
    float y = (frameActual / frames_por_columna) * frame_height;
    
    return Rectangle{x, y, (float)frame_width, (float)frame_height};
}

void Animation::reset() {
    frameActual = primerFrame;
    currentTime = 0.0f;
}

int Animation::getCurrentFrame() const {
    return frameActual;
}

    