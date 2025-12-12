#include <gtest/gtest.h>
#include <memory> // Necesario para unique_ptr

// Incluimos tus cabeceras
#include "SettingsManager.hpp"
#include "ResourceManager.hpp"
#include "StateMachine.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "AudioManager.hpp"
#include "MapLoader.hpp"
#include "GameState.hpp"

// --- MOCK STATE PARA TESTEAR STATEMACHINE ---
// Esta clase debe implementar TODOS los métodos virtuales puros de GameState
class MockState : public GameState {
public:
    void init() override {}
    void update(float dt) override { (void)dt; } // (void)dt evita warnings de variable no usada
    void render() override {}    // Si tu GameState tiene render()
    void handleInput() override {} // Requerido según el error
    void pause() override {}     // Requerido según el error
    void resume() override {}    // Requerido según el error
};

// --- GRUPO 1: SINGLETONS (Patrones de Diseño) ---

// Test 1: Verificar que SettingsManager es un Singleton único
TEST(SettingsManagerTest, IsSingleton) {
    // CORRECCIÓN: Usamos punteros (*) porque tu getInstance devuelve un puntero
    SettingsManager* s1 = SettingsManager::getInstance();
    SettingsManager* s2 = SettingsManager::getInstance();
    
    // Verificamos que los punteros apunten a la misma dirección de memoria
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1, nullptr); // Aseguramos que no sea nulo
}

// Test 2: Verificar que ResourceManager es un Singleton único
TEST(ResourceManagerTest, IsSingleton) {
    ResourceManager* r1 = ResourceManager::getInstance();
    ResourceManager* r2 = ResourceManager::getInstance();
    EXPECT_EQ(r1, r2);
    EXPECT_NE(r1, nullptr);
}

// Test 3: Verificar que AudioManager es un Singleton único
TEST(AudioManagerTest, IsSingleton) {
    AudioManager* a1 = AudioManager::getInstance();
    AudioManager* a2 = AudioManager::getInstance();
    EXPECT_EQ(a1, a2);
    EXPECT_NE(a1, nullptr);
}

// --- GRUPO 2: LÓGICA DE JUEGO (Core) ---

// Test 4: Inicialización de Player
TEST(PlayerTest, Initialization) {
    EXPECT_NO_THROW({
        Player p; 
    });
}

// Test 5: Inicialización de Enemy
TEST(EnemyTest, Initialization) {
    EXPECT_NO_THROW({
        Enemy e;
    });
}

// Test 6: Máquina de Estados (CORREGIDO)
TEST(StateMachineTest, InitialState) {
    StateMachine sm;
    // CORRECCIÓN IMPORTANTE: Usamos make_unique, NO make_shared
    auto state = std::make_unique<MockState>();
    EXPECT_NO_THROW(sm.add_state(std::move(state), false));
}
// --- GRUPO 3: UTILIDADES Y SISTEMAS ---

// Test 7: MapLoader existencia
TEST(MapLoaderTest, ClassExists) {
    MapLoader loader;
    EXPECT_TRUE(true); 
}

// Test 8: Validación de rutas de configuración
TEST(ConfigTest, SettingsFileExists) {
    FILE* f = fopen("assets/config/settings.json", "r");
    bool exists = (f != NULL);
    if(f) fclose(f);
    EXPECT_TRUE(exists);
}

// Test 9: Validación de rutas de música
TEST(AssetsTest, BackgroundMusicExists) {
    FILE* f = fopen("assets/music/backgroundMusic.mp3", "r");
    bool exists = (f != NULL);
    if(f) fclose(f);
    EXPECT_TRUE(exists);
}

// Test 10: Comprobación matemática básica (Raymath)
#include "raymath.h"
TEST(RaylibMath, VectorAddition) {
    Vector2 v1 = {10.0f, 10.0f};
    Vector2 v2 = {20.0f, 20.0f};
    Vector2 result = Vector2Add(v1, v2);
    EXPECT_FLOAT_EQ(result.x, 30.0f);
    EXPECT_FLOAT_EQ(result.y, 30.0f);
}