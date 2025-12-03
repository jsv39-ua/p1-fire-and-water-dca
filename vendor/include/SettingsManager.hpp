#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <map>

extern "C" {
    #include <raylib.h>
}

class SettingsManager {
public:
    static SettingsManager* getInstance();
    
    void loadSettings();
    void saveSettings();
    void loadKeyNames(); // New method to load key names from JSON

    // Audio
    float getMasterVolume() const { return masterVolume; }
    float getMusicVolume() const { return musicVolume; }
    float getSfxVolume() const { return sfxVolume; }
    
    void setMasterVolume(float volume) { masterVolume = volume; }
    void setMusicVolume(float volume) { musicVolume = volume; }
    void setSfxVolume(float volume) { sfxVolume = volume; }
    
    // Language
    std::string getLanguage() const { return language; }
    void setLanguage(const std::string& lang) { language = lang; }
    
    // Controls - Fire Player
    int getFireLeft() const { return fireLeft; }
    int getFireRight() const { return fireRight; }
    int getFireJump() const { return fireJump; }
    int getFireAttack() const { return fireAttack; }
    
    void setFireKeys(int left, int right, int jump, int attack) {
        fireLeft = left;
        fireRight = right;
        fireJump = jump;
        fireAttack = attack;
    }
    
    // Controls - Water Player
    int getWaterLeft() const { return waterLeft; }
    int getWaterRight() const { return waterRight; }
    int getWaterJump() const { return waterJump; }
    int getWaterAttack() const { return waterAttack; }
    
    void setWaterKeys(int left, int right, int jump, int attack) {
        waterLeft = left;
        waterRight = right;
        waterJump = jump;
        waterAttack = attack;
    }

    // Best Time
    float getBestTime() const { return bestTime; }
    void setBestTime(float time) { bestTime = time; }

    // Utility
    std::string getKeyName(int key);

private:
    SettingsManager();
    ~SettingsManager() = default;
    
    static SettingsManager* instance;
    
    // Audio settings
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    
    // Language
    std::string language;
    
    // Controls
    int fireLeft, fireRight, fireJump, fireAttack;
    int waterLeft, waterRight, waterJump, waterAttack;

    // Best Time
    float bestTime;
    
    const char* settingsPath = "assets/config/settings.json";
    const char* keyNamesPath = "assets/config/key_names.json";
    
    std::map<int, std::string> keyNames;
};