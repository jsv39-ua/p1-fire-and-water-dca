#pragma once
#include <states/GameState.hpp>
#include <string>
#include <vector>
#include <functional>

// Estado de carga que muestra una barra de progreso mientras se cargan los recursos
class LoadingState : public GameState {
public:
    LoadingState();
    ~LoadingState() override;

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render() override;
    void pause() override;
    void resume() override;

private:
    // Estructura para representar una tarea de carga
    struct LoadTask {
        std::string name;                    // Nombre del recurso (para mostrar)
        std::function<void()> loadFunction;  // Función que carga el recurso
    };

    std::vector<LoadTask> loadTasks;  // Lista de tareas de carga
    size_t currentTaskIndex = 0;      // Índice de la tarea actual
    bool loadingComplete = false;     // Indica si la carga ha terminado
    float minLoadTime = 0.5f;         // Tiempo mínimo de carga (para que se vea la barra)
    float elapsedTime = 0.0f;         // Tiempo transcurrido

    // Configuración visual
    float barWidth = 400.0f;
    float barHeight = 30.0f;
    
    void setupLoadTasks();  // Configura las tareas de carga
};
