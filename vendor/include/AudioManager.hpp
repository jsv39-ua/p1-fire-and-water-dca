#pragma once

extern "C" {
    #include <raylib.h>
}

class AudioManager {
private:
    static AudioManager* instance;
    Sound buttonClickSound;
    Sound buttonHoverSound;
    
    AudioManager(); // Constructor privado para Singleton
    
public:
    static AudioManager* getInstance();
    static void cleanup();
    
    void init();
    void playButtonClick();
    void playButtonHover();
    
    ~AudioManager();
};