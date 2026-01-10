#pragma once

#include <vector>

extern "C" {
    #include <raylib.h>
}

class SoundPlayer;

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
    void updateRegisteredSoundPlayers();
    void updateRegisteredWalkMusic();
    
    // Listas de SoundPlayers y walkMusic registrados
    std::vector<SoundPlayer*> registeredSoundPlayers;
    std::vector<Music*> registeredWalkMusic;
    
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
    
    // Sistema de registro de SoundPlayers
    void registerSoundPlayer(SoundPlayer* player);
    void unregisterSoundPlayer(SoundPlayer* player);
    
    // Sistema de registro de walkMusic
    void registerWalkMusic(Music* music);
    void unregisterWalkMusic(Music* music);
    
    ~AudioManager(); 
};