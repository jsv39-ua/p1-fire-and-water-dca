#pragma once

extern "C" {
    #include <raylib.h>
}

class AudioManager {
private:
    static AudioManager* instance;
    Sound buttonClickSound;
    Sound buttonHoverSound;
    float musicVolumeLevel = 0.25f;
    
    AudioManager(); // Constructor privado para Singleton

    Music backgroundMusic;

    
public:
    static AudioManager* getInstance();
    static void cleanup();
    
    void init();
    void playButtonClick();
    void playButtonHover();

    void updateMusicStream();
    void setMusicVolume(float volume) ;
    float getMusicVolume() ;

    
    ~AudioManager();
};