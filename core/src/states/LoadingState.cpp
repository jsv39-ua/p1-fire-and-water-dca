#include <states/LoadingState.hpp>
#include <states/StateMachine.hpp>
#include <states/MainMenuState.hpp>
#include <systems/ResourceManager.hpp>
#include <systems/AudioManager.hpp>
#include "systems/LocalizationManager.hpp"
extern "C" {
    #include <raylib.h>
}

LoadingState::LoadingState() {}

LoadingState::~LoadingState() {}

void LoadingState::init() {
    setupLoadTasks();
}

void LoadingState::setupLoadTasks() {
    // Precarga de texturas principales
    loadTasks.push_back(LoadTask{
        "Cargando logo...",
        []() { ResourceManager::getInstance()->loadTexture("assets/images/logo.png"); }
    });
    
    loadTasks.push_back(LoadTask{
        "Cargando personajes...",
        []() { 
            ResourceManager::getInstance()->loadTexture("assets/characters/fire_char.png");
            ResourceManager::getInstance()->loadTexture("assets/characters/water_char.png");
        }
    });
    
    // Precarga de música y sonidos UI
    loadTasks.push_back(LoadTask{
        "Cargando música...",
        []() { ResourceManager::getInstance()->loadMusic("assets/music/backgroundMusic.mp3"); }
    });
    
    loadTasks.push_back(LoadTask{
        "Cargando sonidos...",
        []() {
            ResourceManager::getInstance()->loadSound("assets/ui_sounds/boton_click.ogg");
            ResourceManager::getInstance()->loadSound("assets/ui_sounds/boton_hover.ogg");
        }
    });
}

void LoadingState::handleInput() {
    // No hay input durante la carga
}

void LoadingState::update(float deltaTime) {
    elapsedTime += deltaTime;
    
    // Cargar un recurso por frame para no bloquear el renderizado
    if (currentTaskIndex < loadTasks.size()) {
        loadTasks[currentTaskIndex].loadFunction();
        currentTaskIndex++;
    } else if (!loadingComplete) {
        loadingComplete = true;
    }
    
    // Transicionar al menú principal cuando termine la carga y haya pasado el tiempo mínimo
    if (loadingComplete && elapsedTime >= minLoadTime) {
        // Inicializar AudioManager ahora que los recursos están cargados
        AudioManager::getInstance()->init();
        
        // Cambiar al menú principal
        state_machine->add_state(std::make_unique<MainMenuState>(), true);
    }
}

void LoadingState::render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    
    // Calcular progreso
    float progress = (loadTasks.empty()) ? 1.0f : 
                     static_cast<float>(currentTaskIndex) / static_cast<float>(loadTasks.size());
    
    // Posición centrada de la barra
    float barX = (GetScreenWidth() - barWidth) / 2.0f;
    float barY = GetScreenHeight() / 2.0f;
    
    // Título
    const char* title = "Cargando...";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, (GetScreenWidth() - titleWidth) / 2, barY - 80, 40, WHITE);
    
    // Barra de fondo
    DrawRectangle(barX, barY, barWidth, barHeight, GRAY);
    
    // Barra de progreso
    DrawRectangle(barX, barY, barWidth * progress, barHeight, SKYBLUE);
    
    // Borde de la barra
    DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
    
    // Porcentaje
    char percentText[16];
    snprintf(percentText, sizeof(percentText), "%d%%", static_cast<int>(progress * 100));
    int percentWidth = MeasureText(percentText, 20);
    DrawText(percentText, (GetScreenWidth() - percentWidth) / 2, barY + barHeight + 10, 20, WHITE);
    
    // Nombre del recurso actual
    if (currentTaskIndex < loadTasks.size()) {
        const char* taskName = loadTasks[currentTaskIndex].name.c_str();
        int taskWidth = MeasureText(taskName, 16);
        DrawText(taskName, (GetScreenWidth() - taskWidth) / 2, barY + barHeight + 40, 16, LIGHTGRAY);
    }
    
    EndDrawing();
}

void LoadingState::pause() {}

void LoadingState::resume() {}
