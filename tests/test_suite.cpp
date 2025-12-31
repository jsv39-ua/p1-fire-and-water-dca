#define BOOST_TEST_MODULE FireAndWater_TestSuite
#include <boost/test/included/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include <memory>
#include <cstdio>

#include "SettingsManager.hpp"
#include "ResourceManager.hpp"
#include "StateMachine.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "AudioManager.hpp"
#include "MapLoader.hpp"
#include "GameState.hpp"
#include "Projectile.hpp" 
#include "Platform.hpp"
#include "Animation.hpp"
#include "Camera.hpp"
#include "SoundPlayer.hpp"
#include "raymath.h"

// Mock para testear la máquina de estados
class MockState : public GameState {
public:
    void init() override {}
    void update(float dt) override { (void)dt; } 
    void render() override {}    
    void handleInput() override {} 
    void pause() override {}    
    void resume() override {}   
};

// SUITE: Singletons - Verificar patrones de diseño
BOOST_AUTO_TEST_SUITE(SingletonTests)

// Test 1: SettingsManager es Singleton único
BOOST_AUTO_TEST_CASE(test_settings_manager_singleton) {
    SettingsManager* s1 = SettingsManager::getInstance();
    SettingsManager* s2 = SettingsManager::getInstance();
    BOOST_TEST(s1 == s2);
    BOOST_TEST(s1 != nullptr);
}

// Test 2: ResourceManager es Singleton único
BOOST_AUTO_TEST_CASE(test_resource_manager_singleton) {
    ResourceManager* r1 = ResourceManager::getInstance();
    ResourceManager* r2 = ResourceManager::getInstance();
    BOOST_TEST(r1 == r2);
    BOOST_TEST(r1 != nullptr);
}

// Test 3: AudioManager es Singleton único
BOOST_AUTO_TEST_CASE(test_audio_manager_singleton) {
    AudioManager* a1 = AudioManager::getInstance();
    AudioManager* a2 = AudioManager::getInstance();
    BOOST_TEST(a1 == a2);
    BOOST_TEST(a1 != nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Entidades - Inicialización de clases del juego
BOOST_AUTO_TEST_SUITE(EntityTests)

// Test 4: Player se inicializa correctamente
BOOST_AUTO_TEST_CASE(test_player_initialization) {
    BOOST_CHECK_NO_THROW(Player p);
}

// Test 5: Enemy se inicializa correctamente
BOOST_AUTO_TEST_CASE(test_enemy_initialization) {
    BOOST_CHECK_NO_THROW(Enemy e);
}

// Test 6: StateMachine acepta estados
BOOST_AUTO_TEST_CASE(test_state_machine_add_state) {
    StateMachine sm;
    auto state = std::make_unique<MockState>();
    BOOST_CHECK_NO_THROW(sm.add_state(std::move(state), false));
}

// Test 7: MapLoader existe y se puede instanciar
BOOST_AUTO_TEST_CASE(test_map_loader_exists) {
    BOOST_CHECK_NO_THROW(MapLoader loader);
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Assets - Verificación de recursos del juego
BOOST_AUTO_TEST_SUITE(AssetTests)

// Test 8: Archivo de configuración existe
BOOST_AUTO_TEST_CASE(test_settings_file_exists) {
    FILE* f = fopen("assets/config/settings.json", "r");
    bool exists = (f != NULL);
    if(f) fclose(f);
    BOOST_TEST(exists, "El archivo settings.json debe existir");
}

// Test 9: Música de fondo existe
BOOST_AUTO_TEST_CASE(test_background_music_exists) {
    FILE* f = fopen("assets/music/backgroundMusic.mp3", "r");
    bool exists = (f != NULL);
    if(f) fclose(f);
    BOOST_TEST(exists, "El archivo backgroundMusic.mp3 debe existir");
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Raylib - Tests de la librería externa
BOOST_AUTO_TEST_SUITE(RaylibTests)

// Test 10: Raymath - Suma de vectores
BOOST_AUTO_TEST_CASE(test_vector_addition) {
    Vector2 v1 = {10.0f, 10.0f};
    Vector2 v2 = {20.0f, 20.0f};
    Vector2 result = Vector2Add(v1, v2);
    BOOST_CHECK_CLOSE(result.x, 30.0f, 0.0001);
    BOOST_CHECK_CLOSE(result.y, 30.0f, 0.0001);
}

// Test 11: Colisiones AABB
BOOST_AUTO_TEST_CASE(test_collision_detection) {
    Rectangle box1 = {0, 0, 50, 50};   
    Rectangle box2 = {25, 25, 50, 50}; 
    Rectangle box3 = {200, 200, 50, 50};

    BOOST_TEST(CheckCollisionRecs(box1, box2)); 
    BOOST_TEST(!CheckCollisionRecs(box1, box3)); 
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Lógica del jugador
BOOST_AUTO_TEST_SUITE(PlayerLogicTests)

// Test 12: Sistema de vidas
BOOST_AUTO_TEST_CASE(test_life_system) {
    Player p;
    p.setLifeCount(3);
    BOOST_TEST(p.getLifeCount() == 3);

    p.updateLifeCount(-1);
    BOOST_TEST(p.getLifeCount() == 2);

    p.updateLifeCount(1);
    BOOST_TEST(p.getLifeCount() == 3);
}

// Test 13: Física de movimiento - Velocidad
BOOST_AUTO_TEST_CASE(test_movement_velocity) {
    Player p;
    p.walk(1);
    BOOST_TEST(p.velocity.x > 0.0f); 

    p.walk(-1);
    BOOST_TEST(p.velocity.x < 0.0f); 

    p.walk(0);
    BOOST_CHECK_CLOSE(p.velocity.x, 0.0f, 0.0001);
}

// Test 14: Física de salto
BOOST_AUTO_TEST_CASE(test_jump_physics) {
    Player p;
    p.velocity = {0.0f, 0.0f};
    p.jump();
    BOOST_TEST(p.velocity.y < 0.0f);
}

// Test 15: Sistema de invencibilidad
BOOST_AUTO_TEST_CASE(test_invincibility_state) {
    Player p;
    BOOST_TEST(!p.isInvincible());
    p.activateInvincibility();
    BOOST_TEST(p.isInvincible());
    BOOST_CHECK_NO_THROW(p.shouldRenderVisible());
}

// Test 16: Orientación del movimiento (facingRight se actualiza externamente)
BOOST_AUTO_TEST_CASE(test_movement_orientation) {
    Player p;
    BOOST_TEST(p.facingRight);
    p.walk(-1);
    BOOST_TEST(p.velocity.x < 0.0f);
    if (p.velocity.x != 0) {
        p.facingRight = p.velocity.x > 0;
    }
    BOOST_TEST(!p.facingRight); 
}

// Test 17: Cooldown de ataque
BOOST_AUTO_TEST_CASE(test_attack_cooldown) {
    Player p;
    BOOST_TEST(p.canAttack());
    p.attack();
    BOOST_TEST(!p.canAttack());
    p.update(0.6f);
    BOOST_TEST(p.canAttack());
}

// Test 18: Mecánica de salto variable
BOOST_AUTO_TEST_CASE(test_variable_jump_height) {
    Player p;
    p.velocity.y = -100.0f; 
    float initialSpeed = p.velocity.y;
    
    p.updateJump(0.016f, true); 
    BOOST_TEST(p.velocity.y < initialSpeed);
    
    float speedAfterBoost = p.velocity.y;
    p.updateJump(0.016f, false);
    BOOST_CHECK_CLOSE(p.velocity.y, speedAfterBoost, 0.0001);
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Mecánicas del juego
BOOST_AUTO_TEST_SUITE(GameplayLogicTests)

// Test 19: Interacción elemental
BOOST_AUTO_TEST_CASE(test_elemental_interaction) {
    Player firePlayer(0, 0, 10, 10, FIRE);
    firePlayer.bounds = Rectangle{10, 10, 10, 10}; 
    
    Platform waterPlatform(Rectangle{0, 0, 50, 50}, WATER);
    Platform firePlatform(Rectangle{0, 0, 50, 50}, FIRE);
    
    BOOST_TEST(firePlayer.checkPlatformCollision(waterPlatform) == 1);
    BOOST_TEST(firePlayer.checkPlatformCollision(firePlatform) == 2);
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Lógica de enemigos
BOOST_AUTO_TEST_SUITE(EnemyLogicTests)

// Test 20: Patrulla de enemigos
BOOST_AUTO_TEST_CASE(test_patrol_movement) {
    Rectangle bounds = {0, 0, 32, 32};
    Enemy enemy(bounds, FIRE, 50.0f, false); 
    
    BOOST_TEST(enemy.movingRight);
    
    enemy.velocity = {1000.0f, 0.0f}; 
    enemy.update(1.0f); 
    BOOST_TEST(!enemy.movingRight);
}

// Test 21: Sistema de daño a enemigos
BOOST_AUTO_TEST_CASE(test_enemy_damage_system) {
    Enemy e;
    BOOST_TEST(e.getLifeCount() == 3);
    
    e.updateLifeCount(-1);
    BOOST_TEST(e.getLifeCount() == 2);
    BOOST_TEST(e.isInvincible());
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Sistema de combate
BOOST_AUTO_TEST_SUITE(CombatLogicTests)

// Test 22: Ciclo de vida de proyectiles
BOOST_AUTO_TEST_CASE(test_projectile_lifecycle) {
    Vector2 pos = {0, 0};
    Vector2 vel = {100, 0};
    Projectile p(pos, vel, FIRE);
    
    BOOST_TEST(p.isActive());
    BOOST_TEST(p.getType() == FIRE);
    
    p.deactivate();
    BOOST_TEST(!p.isActive());
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Sistema de animaciones
BOOST_AUTO_TEST_SUITE(AnimationSystemTests)

// Test 23: Loop de animación
BOOST_AUTO_TEST_CASE(test_animation_loop) {
    Animation anim(2, 0.1f, true, {0}, 0, 1);
    
    BOOST_TEST(anim.getCurrentFrame() == 0);
    
    anim.animationUpdate(0.1f);
    BOOST_TEST(anim.getCurrentFrame() == 1);
    
    anim.animationUpdate(0.1f);
    BOOST_TEST(anim.getCurrentFrame() == 0);
}

// Test 24: Reset de animación
BOOST_AUTO_TEST_CASE(test_animation_reset) {
    Animation anim(5, 0.1f, true, {0}, 0, 4);
    
    anim.animationUpdate(0.25f);
    BOOST_TEST(anim.getCurrentFrame() > 0);
    
    anim.reset();
    BOOST_TEST(anim.getCurrentFrame() == 0);
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Sistema de cámara
BOOST_AUTO_TEST_SUITE(CameraSystemTests)

// Test 25: Límites de zoom
BOOST_AUTO_TEST_CASE(test_zoom_limits) {
    GameCamera cam;
    cam.init({0,0}, 1.0f, {0,0,100,100});
    cam.setZoomLimits(0.5f, 2.0f);
    
    cam.setZoom(0.1f);
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 0.5f, 0.0001);
    
    cam.setZoom(10.0f);
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 2.0f, 0.0001);
}

// Test 26: Ajuste incremental de zoom
BOOST_AUTO_TEST_CASE(test_zoom_adjustment) {
    GameCamera cam;
    cam.init({0,0}, 1.0f, {0,0,100,100});
    
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 1.0f, 0.0001);
    
    cam.adjustZoom(0.5f);
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 1.5f, 0.0001);
    
    cam.adjustZoom(-0.2f);
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 1.3f, 0.0001);
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Sistema de configuración
BOOST_AUTO_TEST_SUITE(SettingsSystemTests)

// Test 27: Rebinding de teclas
BOOST_AUTO_TEST_CASE(test_key_rebinding) {
    SettingsManager* sm = SettingsManager::getInstance();
    
    int originalJump = sm->getFireJump();
    
    sm->setFireKeys(sm->getFireLeft(), sm->getFireRight(), 32, sm->getFireAttack());
    BOOST_TEST(sm->getFireJump() == 32);
    
    // Restaurar valor original
    sm->setFireKeys(sm->getFireLeft(), sm->getFireRight(), originalJump, sm->getFireAttack());
}

// Test 28: Parsing de nombres de teclas
BOOST_AUTO_TEST_CASE(test_key_name_parsing) {
    SettingsManager* sm = SettingsManager::getInstance();
    
    std::string nameA = sm->getKeyName(65);
    BOOST_TEST(nameA == "A");
    std::string nameZ = sm->getKeyName(90);
    BOOST_TEST(nameZ == "Z");
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Sistema de audio
BOOST_AUTO_TEST_SUITE(AudioSystemTests)

// Test 29: Inicialización de SoundPlayer con múltiples sonidos
BOOST_AUTO_TEST_CASE(test_multi_sound_init) {
    SoundPlayer sp;
    std::vector<std::string> sounds = {
        "assets/characters/sounds/Hero Jump.mp3",
        "assets/characters/sounds/Hero Walk Footsteps Stone.mp3"
    };
    
    BOOST_CHECK_NO_THROW(sp.init(sounds));
    if (IsAudioDeviceReady()) {
        BOOST_TEST(sp.isInitialized());
    }
}

BOOST_AUTO_TEST_SUITE_END()

// SUITE: Gestión de recursos
BOOST_AUTO_TEST_SUITE(ResourceSystemTests)

// Test 30: Descarga segura de recursos inexistentes
BOOST_AUTO_TEST_CASE(test_safe_unload) {
    ResourceManager* rm = ResourceManager::getInstance();
    BOOST_CHECK_NO_THROW(rm->unloadTexture("textura_inexistente.png"));
    BOOST_CHECK_NO_THROW(rm->unloadSound("sonido_inexistente.mp3"));
}

BOOST_AUTO_TEST_SUITE_END()
