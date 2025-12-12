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
#include "Projectile.hpp" 
#include "Platform.hpp"
#include "Animation.hpp"
#include "Camera.hpp"
#include "SoundPlayer.hpp"

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

// ==========================================
// GRUPO 1: TEST BÁSICOS (SMOKE TESTS)
// ==========================================

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

// ==========================================
// GRUPO 2: TESTS AVANZADOS (LÓGICA DEL JUEGO)
// ==========================================

// Test 11: Sistema de Vidas del Jugador
// Verifica que se puede establecer y modificar la vida correctamente
TEST(PlayerLogic, LifeSystem) {
    Player p;
    // Establecemos vida inicial
    p.setLifeCount(3);
    EXPECT_EQ(p.getLifeCount(), 3);

    // Recibimos daño (restar vida)
    p.updateLifeCount(-1);
    EXPECT_EQ(p.getLifeCount(), 2);

    // Ganamos vida
    p.updateLifeCount(1);
    EXPECT_EQ(p.getLifeCount(), 3);
}

// Test 12: Lógica de Movimiento (VELOCIDAD)
// Verifica solamente que la velocidad cambia (aceleración/frenado)
TEST(PlayerLogic, MovementPhysics_Velocity) {
    Player p;
    
    // Caminar a la derecha
    p.walk(1);
    EXPECT_GT(p.velocity.x, 0.0f); 

    // Caminar a la izquierda
    p.walk(-1);
    EXPECT_LT(p.velocity.x, 0.0f); 

    // Parar
    p.walk(0);
    EXPECT_FLOAT_EQ(p.velocity.x, 0.0f);
}

// Test 13: Lógica de Salto (Jump)
// Verifica que saltar aplica una fuerza vertical negativa (hacia arriba)
TEST(PlayerLogic, JumpPhysics) {
    Player p;
    // Reseteamos velocidad por si acaso
    p.velocity = {0.0f, 0.0f};

    p.jump();

    // En Raylib/2D, "arriba" suele ser Y negativo. 
    // Verificamos que la velocidad vertical ha cambiado a negativo.
    EXPECT_LT(p.velocity.y, 0.0f);
}

// Test 14: Sistema de Invencibilidad
// Verifica que la invencibilidad se activa y el estado cambia
TEST(PlayerLogic, InvincibilityState) {
    Player p;
    
    // Por defecto no debe ser invencible
    EXPECT_FALSE(p.isInvincible());

    // Activamos invencibilidad (como si recibiera daño)
    p.activateInvincibility();

    // Ahora debe ser invencible
    EXPECT_TRUE(p.isInvincible());
    
    // Verificar que debe parpadear (shouldRenderVisible suele depender de esto)
    // Al inicio del timer suele ser visible o invisible dependiendo de tu lógica,
    // pero probamos que la llamada no crashea.
    EXPECT_NO_THROW(p.shouldRenderVisible());
}

// Test 15: Colisiones (AABB - Axis Aligned Bounding Box)
// Verifica que la función nativa de Raylib para colisiones funciona como esperamos
TEST(PhysicsTest, CollisionLogic) {
    // Definimos dos rectángulos que se solapan
    Rectangle box1 = {0, 0, 50, 50};   // En el origen, 50x50
    Rectangle box2 = {25, 25, 50, 50}; // Desplazado pero solapando

    // Definimos un rectángulo lejano
    Rectangle box3 = {200, 200, 50, 50};

    // CheckCollisionRecs es una función de Raylib
    EXPECT_TRUE(CheckCollisionRecs(box1, box2)); // Deben chocar
    EXPECT_FALSE(CheckCollisionRecs(box1, box3)); // No deben chocar
}

// Test 16: Lógica de Movimiento (ORIENTACIÓN)
// Verifica EXCLUSIVAMENTE que el sprite mira al lado correcto
// ESTE ES EL TEST QUE FALLARÁ hasta que se arregle el bug
TEST(PlayerLogic, MovementPhysics_Orientation) {
    Player p;

    // Caminar a la derecha -> Debe mirar a la derecha (true)
    p.walk(1);
    EXPECT_TRUE(p.facingRight);

    // Caminar a la izquierda -> Debe mirar a la izquierda (false)
    p.walk(-1);
    EXPECT_FALSE(p.facingRight); 
}

// ==========================================
// GRUPO 3: ELEMENTOS Y COMBATE (NUEVOS)
// ==========================================

// Test 17: Interacción Elemental
TEST(GameplayLogic, ElementalInteraction) {
    // Jugador de Fuego
    Player firePlayer(0, 0, 10, 10, FIRE);
    firePlayer.bounds = Rectangle{10, 10, 10, 10}; 
    
    // Plataformas (usamos el constructor que recibe Rectangle y Type)
    Platform waterPlatform(Rectangle{0, 0, 50, 50}, WATER);
    Platform firePlatform(Rectangle{0, 0, 50, 50}, FIRE);
    
    // Fuego vs Agua -> Debe devolver 1 (Daño/Contrario)
    EXPECT_EQ(firePlayer.checkPlatformCollision(waterPlatform), 1);

    // Fuego vs Fuego -> Debe devolver 2 (Amigo/Seguro)
    EXPECT_EQ(firePlayer.checkPlatformCollision(firePlatform), 2);
}

// Test 18: Patrulla de Enemigos
TEST(EnemyLogic, PatrolMovement) {
    // Enemigo en (0,0) con rango 50
    Rectangle bounds = {0, 0, 32, 32};
    Enemy enemy(bounds, FIRE, 50.0f, false); // false = movimiento horizontal
    
    // Estado inicial: moviéndose a la derecha
    EXPECT_TRUE(enemy.movingRight);
    
    // Forzamos velocidad alta para simular que llega al límite rápido
    enemy.velocity = {1000.0f, 0.0f}; 
    enemy.update(1.0f); // Avanza 1000px, superando el rango de 50px
    
    // Debería haber cambiado de dirección al superar el límite
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
    Enemy e; // Constructor por defecto (life=3)
    
    EXPECT_EQ(e.getLifeCount(), 3);
    
    // Recibir daño (-1 vida)
    e.updateLifeCount(-1);
    
    EXPECT_EQ(e.getLifeCount(), 2);
    // Al recibir daño debe volverse invencible temporalmente
    EXPECT_TRUE(e.isInvincible());
}

// ==========================================
// GRUPO 6: SISTEMAS AUXILIARES (NUEVO)
// ==========================================

// Test 21: Lógica de Animación (Bucle)
// Verifica que la animación avanza y vuelve al inicio al terminar
TEST(AnimationSystem, AnimationLoop) {
    // Animación de 2 frames (0 y 1), tiempo por frame 0.1s
    Animation anim(2, 0.1f, true, {0}, 0, 1);
    
    // Inicio: Frame 0
    EXPECT_EQ(anim.getCurrentFrame(), 0);
    
    // Avanzar 0.1s -> Frame 1
    anim.animationUpdate(0.1f);
    EXPECT_EQ(anim.getCurrentFrame(), 1);
    
    // Avanzar 0.1s más -> Volver a Frame 0 (Loop)
    anim.animationUpdate(0.1f);
    EXPECT_EQ(anim.getCurrentFrame(), 0);
}

// Test 22: Configuración de Teclas (SettingsManager)
// Verifica que se pueden reasignar las teclas correctamente
TEST(SettingsSystem, KeyRebinding) {
    SettingsManager* sm = SettingsManager::getInstance();
    
    // Guardar teclas originales
    int originalJump = sm->getFireJump();
    
    // Cambiar tecla de salto a ESPACIO (32)
    sm->setFireKeys(sm->getFireLeft(), sm->getFireRight(), 32, sm->getFireAttack());
    
    EXPECT_EQ(sm->getFireJump(), 32);
    
    // Restaurar original para no afectar otros tests
    sm->setFireKeys(sm->getFireLeft(), sm->getFireRight(), originalJump, sm->getFireAttack());
}

// Test 23: Límites de Zoom de Cámara
// Verifica que el zoom no excede los límites establecidos
TEST(CameraSystem, ZoomLimits) {
    GameCamera cam;
    cam.init({0,0}, 1.0f, {0,0,100,100});
    cam.setZoomLimits(0.5f, 2.0f);
    
    // Intentar poner zoom muy pequeño
    cam.setZoom(0.1f);
    // Debería quedarse en el mínimo (0.5f)
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 0.5f);
    
    // Intentar poner zoom muy grande
    cam.setZoom(10.0f);
    // Debería quedarse en el máximo (2.0f)
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 2.0f);
}

// Test 24: SoundPlayer (Variantes Múltiples)
// Verifica que SoundPlayer acepta inicialización con múltiples archivos
TEST(AudioSystem, MultiSoundInit) {
    SoundPlayer sp;
    std::vector<std::string> sounds = {
        "assets/characters/sounds/Hero Jump.mp3",
        "assets/characters/sounds/Hero Walk Footsteps Stone.mp3"
    };
    
    // Nota: Este test requiere que los archivos existan para devolver true
    // Si falla porque no hay audio device en CI, verificamos solo que no crashee
    EXPECT_NO_THROW(sp.init(sounds));
    
    // Si tenemos dispositivo de audio (local), debería estar inicializado
    if (IsAudioDeviceReady()) {
        EXPECT_TRUE(sp.isInitialized());
    }
}

// Test 25: Gestión de Recursos (Seguridad)
// Verifica que intentar descargar un recurso no cargado no rompe el programa
TEST(ResourceSystem, SafeUnload) {
    ResourceManager* rm = ResourceManager::getInstance();
    
    // Intentar descargar algo que nunca cargamos
    EXPECT_NO_THROW(rm->unloadTexture("textura_inexistente.png"));
    EXPECT_NO_THROW(rm->unloadSound("sonido_inexistente.mp3"));
}

// ==========================================
// GRUPO 7: MECÁNICAS FINAS (NUEVO - TESTS 26-30)
// ==========================================

// Test 26: Reseteo de Animación
// Verifica que al llamar a reset(), la animación vuelve al frame inicial
TEST(AnimationSystem, ResetLogic) {
    // Animación de 5 frames (0 al 4)
    Animation anim(5, 0.1f, true, {0}, 0, 4);
    
    // Avanzamos un poco
    anim.animationUpdate(0.25f);
    // Debería haber avanzado (al menos estar en frame 1 o 2)
    EXPECT_GT(anim.getCurrentFrame(), 0);
    
    // Reseteamos
    anim.reset();
    
    // Debe volver a 0
    EXPECT_EQ(anim.getCurrentFrame(), 0);
}

// Test 27: Ajuste Incremental de Zoom (Cámara)
// Verifica que adjustZoom suma correctamente al targetZoom
TEST(CameraSystem, ZoomAdjustment) {
    GameCamera cam;
    cam.init({0,0}, 1.0f, {0,0,100,100});
    
    // Zoom inicial 1.0
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 1.0f);
    
    // Aumentar zoom en 0.5
    cam.adjustZoom(0.5f);
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 1.5f);
    
    // Disminuir zoom en 0.2
    cam.adjustZoom(-0.2f);
    EXPECT_FLOAT_EQ(cam.getTargetZoom(), 1.3f);
}

// Test 28: Cooldown de Ataque del Jugador
// Verifica que no se puede atacar inmediatamente después de otro ataque
TEST(PlayerLogic, AttackCooldown) {
    Player p;
    // Estado inicial: debería poder atacar
    EXPECT_TRUE(p.canAttack());
    
    // Atacamos
    p.attack();
    
    // Inmediatamente después, NO debería poder atacar (cooldown activo)
    EXPECT_FALSE(p.canAttack());
    
    // Simulamos paso del tiempo (0.6 segundos, el cooldown es 0.5s en Player.hpp)
    p.update(0.6f);
    
    // Ahora debería poder atacar de nuevo
    EXPECT_TRUE(p.canAttack());
}

// Test 29: Nombres de Teclas (Settings)
// Verifica que el sistema devuelve nombres legibles para teclas ASCII
TEST(SettingsSystem, KeyNameParsing) {
    SettingsManager* sm = SettingsManager::getInstance();
    
    // Prueba con tecla 'A' (código 65)
    std::string nameA = sm->getKeyName(65);
    EXPECT_EQ(nameA, "A");
    
    // Prueba con tecla 'Z' (código 90)
    std::string nameZ = sm->getKeyName(90);
    EXPECT_EQ(nameZ, "Z");
}

// Test 30: Mecánica de Salto Variable
// Verifica que mantener la tecla de salto impulsa más al jugador
TEST(PlayerLogic, VariableJumpHeight) {
    Player p;
    p.velocity.y = -100.0f; // Simulamos que está subiendo (saltando)
    float initialSpeed = p.velocity.y;
    
    // Actualizamos con la tecla de salto MANTENIDA (true)
    // Debería aplicar impulso extra (JUMP_BOOST), haciendo la velocidad más negativa
    p.updateJump(0.016f, true); 
    
    // La velocidad debe haber aumentado hacia arriba (más negativa que antes)
    EXPECT_LT(p.velocity.y, initialSpeed);
    
    // Caso contrario: Actualizamos SIN mantener la tecla
    float speedAfterBoost = p.velocity.y;
    p.updateJump(0.016f, false);
    
    // La velocidad no debería cambiar (por el boost)
    EXPECT_FLOAT_EQ(p.velocity.y, speedAfterBoost);
}