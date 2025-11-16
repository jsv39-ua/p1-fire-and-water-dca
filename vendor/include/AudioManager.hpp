#pragma once

extern "C" {
    #include <raylib.h>
}

class AudioManager {
private:
    static AudioManager* instance;
    
    // Recursos de audio
    Music backgroundMusic;
    Sound buttonClickSound;
    Sound buttonHoverSound;
    
    // Volúmenes base (valores configurados por el usuario, rango 0.0 - 1.0)
    float masterVolume = 0.5f;      // Volumen general (multiplica a los demás)
    float musicBaseVolume = 0.5f;   // Volumen de música (sin multiplicar)
    float sfxBaseVolume = 0.5f;     // Volumen de efectos de sonido
    
    AudioManager(); // Constructor privado para Singleton
    
    // Métodos privados de actualización
    void updateMusicVolume();
    void updateSfxVolumes();
    
public:
    static AudioManager* getInstance();
    static void cleanup();
    
    void init();
    void playButtonClick();
    void playButtonHover();
    void updateMusicStream();
    
    // Setters de volumen
    void setMasterVolume(float volume);
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);
    
    // Getters de volumen
    float getMasterVolume() const;
    float getMusicVolume() const;
    float getSfxVolume() const;
    
    ~AudioManager(); 
};