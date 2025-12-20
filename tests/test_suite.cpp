#include <gtest/gtest.h>
#include <memory>
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

class MockState : public GameState {
public:
    void init() override {}
    void update(float dt) override { (void)dt; } 
    void render() override {}    
    void handleInput() override {} 
    void pause() override {}    
    void resume() override {}   
};



// Test 1: Verificar que SettingsManager es un Singleton único
TEST(SettingsManagerTest, IsSingleton) {
    SettingsManager* s1 = SettingsManager::getInstance();
    SettingsManager* s2 = SettingsManager::getInstance();
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1, nullptr);
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
    auto state = std::make_unique<MockState>();
    EXPECT_NO_THROW(sm.add_state(std::move(state), false));
}
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

// Test 11: Sistema de Vidas del Jugador
TEST(PlayerLogic, LifeSystem) {
    Player p;
    p.setLifeCount(3);
    EXPECT_EQ(p.getLifeCount(), 3);

    p.updateLifeCount(-1);
    EXPECT_EQ(p.getLifeCount(), 2);

    p.updateLifeCount(1);
    EXPECT_EQ(p.getLifeCount(), 3);
}

// Test 12: Lógica de Movimiento (VELOCIDAD)
TEST(PlayerLogic, MovementPhysics_Velocity) {
    Player p;
    p.walk(1);
    EXPECT_GT(p.velocity.x, 0.0f); 

    p.walk(-1);
    EXPECT_LT(p.velocity.x, 0.0f); 

    p.walk(0);
    EXPECT_FLOAT_EQ(p.velocity.x, 0.0f);
}

// Test 13: Lógica de Salto (Jump)
TEST(PlayerLogic, JumpPhysics) {
    Player p;
    p.velocity = {0.0f, 0.0f};

    p.jump();
    EXPECT_LT(p.velocity.y, 0.0f);
}

// Test 14: Sistema de Invencibilidad
TEST(PlayerLogic, InvincibilityState) {
    Player p;
    
    EXPECT_FALSE(p.isInvincible());

    p.activateInvincibility();

    EXPECT_TRUE(p.isInvincible());
    EXPECT_NO_THROW(p.shouldRenderVisible());
}

// Test 15: Colisiones (AABB - Axis Aligned Bounding Box)
TEST(PhysicsTest, CollisionLogic) {
    Rectangle box1 = {0, 0, 50, 50};   
    Rectangle box2 = {25, 25, 50, 50}; 
    Rectangle box3 = {200, 200, 50, 50};

    EXPECT_TRUE(CheckCollisionRecs(box1, box2)); 
    EXPECT_FALSE(CheckCollisionRecs(box1, box3)); 
}

// Test 16: Lógica de Movimiento (ORIENTACIÓN)
TEST(PlayerLogic, MovementPhysics_Orientation) {
    Player p;

    p.walk(1);
    EXPECT_TRUE(p.facingRight);

    p.walk(-1);
    EXPECT_FALSE(p.facingRight); 
}



// Test 17: Interacción Elemental
TEST(GameplayLogic, ElementalInteraction) {
    Player firePlayer(0, 0, 10, 10, FIRE);
    firePlayer.bounds = Rectangle{10, 10, 10, 10}; 
    
    Platform waterPlatform(Rectangle{0, 0, 50, 50}, WATER);
    Platform firePlatform(Rectangle{0, 0, 50, 50}, FIRE);
    
    EXPECT_EQ(firePlayer.checkPlatformCollision(waterPlatform), 1);

    EXPECT_EQ(firePlayer.checkPlatformCollision(firePlatform), 2);
}

// Test 18: Patrulla de Enemigos
TEST(EnemyLogic, PatrolMovement) {
    Rectangle bounds = {0, 0, 32, 32};
    Enemy enemy(bounds, FIRE, 50.0f, false); 
    
    EXPECT_TRUE(enemy.movingRight);
    
    enemy.velocity = {1000.0f, 0.0f}; 
    enemy.update(1.0f); 
    EXPECT_FALSE(enemy.movingRight);
}

// Test 19: Ciclo de vida de Proyectiles
TEST(CombatLogic, ProjectileLifecycle) {
    Vector2 pos = {0, 0};
    Vector2 vel = {100, 0};
    Projectile p(pos, vel, FIRE);
    
    EXPECT_TRUE(p.isActive());
    EXPECT_EQ(p.getType(), FIRE);
    
    p.deactivate();
    EXPECT_FALSE(p.isActive());
}

// Test 20: Sistema de Daño a Enemigos
TEST(EnemyLogic, DamageSystem) {
    Enemy e;
    
    EXPECT_EQ(e.getLifeCount(), 3);
    
    e.updateLifeCount(-1);
    
    EXPECT_EQ(e.getLifeCount(), 2);
    EXPECT_TRUE(e.isInvincible());
}

// Test 21: Lógica de Animación (Bucle)
TEST(AnimationSystem, AnimationLoop) {
    Animation anim(2, 0.1f, true, {0}, 0, 1);
    
    EXPECT_EQ(anim.getCurrentFrame(), 0);
    
    anim.animationUpdate(0.1f);
    EXPECT_EQ(anim.getCurrentFrame(), 1);
    
    anim.animationUpdate(0.1f);
    EXPECT_EQ(anim.getCurrentFrame(), 0);
}

// Test 22: Configuración de Teclas (SettingsManager)
TEST(SettingsSystem, KeyRebinding) {
    SettingsManager* sm = SettingsManager::getInstance();
    
    int originalJump = sm->getFireJump();
    
    sm->setFireKeys(sm->getFireLeft(), sm->getFireRight(), 32, sm->getFireAttack());
    
    EXPECT_EQ(sm->getFireJump(), 32);
    
    sm->setFireKeys(sm->getFireLeft(), sm->getFireRight(), originalJump, sm->getFireAttack());
}

// Test 23: Límites de Zoom de Cámara
TEST(CameraSystem, ZoomLimits) {
    GameCamera cam;
    cam.init({0,0}, 1.0f, {0,0,100,100});
    cam.setZoomLimits(0.5f, 2.0f);
    
    cam.setZoom(0.1f);
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 0.5f);
    
    cam.setZoom(10.0f);
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 2.0f);
}

// Test 24: SoundPlayer (Variantes Múltiples)
TEST(AudioSystem, MultiSoundInit) {
    SoundPlayer sp;
    std::vector<std::string> sounds = {
        "assets/characters/sounds/Hero Jump.mp3",
        "assets/characters/sounds/Hero Walk Footsteps Stone.mp3"
    };
    

    EXPECT_NO_THROW(sp.init(sounds));
    if (IsAudioDeviceReady()) {
        EXPECT_TRUE(sp.isInitialized());
    }
}

// Test 25: Gestión de Recursos (Seguridad)
TEST(ResourceSystem, SafeUnload) {
    ResourceManager* rm = ResourceManager::getInstance();
    EXPECT_NO_THROW(rm->unloadTexture("textura_inexistente.png"));
    EXPECT_NO_THROW(rm->unloadSound("sonido_inexistente.mp3"));
}

// Test 26: Reseteo de Animación
TEST(AnimationSystem, ResetLogic) {
    Animation anim(5, 0.1f, true, {0}, 0, 4);
    
    anim.animationUpdate(0.25f);
    EXPECT_GT(anim.getCurrentFrame(), 0);
    
    anim.reset();
    
    EXPECT_EQ(anim.getCurrentFrame(), 0);
}

// Test 27: Ajuste Incremental de Zoom (Cámara)
TEST(CameraSystem, ZoomAdjustment) {
    GameCamera cam;
    cam.init({0,0}, 1.0f, {0,0,100,100});
    
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 1.0f);
    
    cam.adjustZoom(0.5f);
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 1.5f);
    
    cam.adjustZoom(-0.2f);
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 1.3f);
}

// Test 28: Cooldown de Ataque del Jugador
TEST(PlayerLogic, AttackCooldown) {
    Player p;
    EXPECT_TRUE(p.canAttack());
    p.attack();
    EXPECT_FALSE(p.canAttack());
    p.update(0.6f);
    EXPECT_TRUE(p.canAttack());
}

// Test 29: Nombres de Teclas (Settings)
TEST(SettingsSystem, KeyNameParsing) {
    SettingsManager* sm = SettingsManager::getInstance();
    
    std::string nameA = sm->getKeyName(65);
    EXPECT_EQ(nameA, "A");
    std::string nameZ = sm->getKeyName(90);
    EXPECT_EQ(nameZ, "Z");
}

// Test 30: Mecánica de Salto Variable
TEST(PlayerLogic, VariableJumpHeight) {
    Player p;
    p.velocity.y = -100.0f; 
    float initialSpeed = p.velocity.y;
    
    p.updateJump(0.016f, true); 
    
    EXPECT_LT(p.velocity.y, initialSpeed);
    
    float speedAfterBoost = p.velocity.y;
    p.updateJump(0.016f, false);
    
    EXPECT_FLOAT_EQ(p.velocity.y, speedAfterBoost);
}