#define BOOST_TEST_MODULE FireAndWater_GameTests
#include <boost/test/unit_test.hpp>

#include "Player.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Platform.hpp"
#include "Animation.hpp"
#include "Camera.hpp"
#include "raymath.h"
#include "systems/MapLoader.hpp"
#include "systems/ResourceManager.hpp"
#include <raytmx.h>

// Para tests que requieren contexto de raylib/raytmx
struct RaylibFixture {
    bool audioInitialized = false;
    
    RaylibFixture() {
        // Inicializar raylib en modo headless para tests
        SetTraceLogLevel(LOG_NONE); // Silenciar logs durante tests
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
        InitWindow(1, 1, "Test");
        // Intentar inicializar audio, pero no fallar si no es posible
        InitAudioDevice();
        audioInitialized = IsAudioDeviceReady();
    }
    ~RaylibFixture() {
        ResourceManager::cleanup();
        if (audioInitialized) {
            CloseAudioDevice();
        }
        CloseWindow();
    }
};

// Suite de test de funcionalidades del juego
BOOST_FIXTURE_TEST_SUITE(GameTests, RaylibFixture)

// Test 1: El jugador pierde vidas correctamente al recibir daño
BOOST_AUTO_TEST_CASE(test_player_loses_lives_on_damage) {
    Player p;
    p.setLifeCount(3);
    
    p.updateLifeCount(-1);
    BOOST_TEST(p.getLifeCount() == 2);
    
    p.updateLifeCount(-1);
    BOOST_TEST(p.getLifeCount() == 1);
    
    p.updateLifeCount(-1);
    BOOST_TEST(p.getLifeCount() == 0);
}

// Test 2: Las vidas no pueden ser negativas
BOOST_AUTO_TEST_CASE(test_player_lives_cannot_be_negative) {
    Player p;
    p.setLifeCount(1);
    
    p.updateLifeCount(-1); // 0
    p.updateLifeCount(-1); // Intenta -1
    p.updateLifeCount(-5); // Intenta -6
    
    BOOST_TEST(p.getLifeCount() == 0);
}

// Test 3: El jugador obtiene invencibilidad temporal tras recibir daño
BOOST_AUTO_TEST_CASE(test_player_invincibility_after_damage) {
    Player p;
    
    BOOST_TEST(!p.isInvincible());
    
    // Perder vida activa invencibilidad automáticamente
    p.updateLifeCount(-1);
    BOOST_TEST(p.isInvincible());
    
    // La invencibilidad desaparece con el tiempo
    for (int i = 0; i < 20; i++) {
        p.update(0.1f); // 2 segundos total
    }
    BOOST_TEST(!p.isInvincible());
}

// Test 4: El enemigo recibe daño y activa invencibilidad
BOOST_AUTO_TEST_CASE(test_enemy_takes_damage) {
    Enemy e;
    int initialLife = e.getLifeCount();
    
    e.updateLifeCount(-1);
    BOOST_TEST(e.getLifeCount() == initialLife - 1);
    BOOST_TEST(e.isInvincible());
}

// Test 5: El jugador se mueve horizontalmente con velocidad correcta
BOOST_AUTO_TEST_CASE(test_player_horizontal_movement) {
    Player p;
    
    p.walk(1);
    BOOST_TEST(p.velocity.x > 0.0f);
    float rightSpeed = p.velocity.x;
    
    p.walk(-1);
    BOOST_TEST(p.velocity.x < 0.0f);
    BOOST_CHECK_CLOSE(p.velocity.x, -rightSpeed, 0.1);
    
    p.walk(0);
    BOOST_CHECK_SMALL(p.velocity.x, 0.001f);
}

// Test 6: El salto aplica velocidad vertical negativa (hacia arriba)
BOOST_AUTO_TEST_CASE(test_player_jump_applies_upward_velocity) {
    Player p;
    p.velocity = {0.0f, 0.0f};
    
    p.jump();
    
    BOOST_TEST(p.velocity.y < 0.0f);
    BOOST_TEST(p.velocity.y < -100.0f);
}

// Test 7: Salto variable, mantener botón da más altura
BOOST_AUTO_TEST_CASE(test_variable_jump_height) {
    Player p;
    p.velocity.y = -100.0f;
    float initialSpeed = p.velocity.y;
    
    p.updateJump(0.016f, true);
    BOOST_TEST(p.velocity.y < initialSpeed);
    
    float boostedSpeed = p.velocity.y;
    
    p.updateJump(0.016f, false);
    BOOST_CHECK_CLOSE(p.velocity.y, boostedSpeed, 0.1);
}

// Test 8: La gravedad se aplica al jugador en cada actualizacion
BOOST_AUTO_TEST_CASE(test_gravity_applies_to_player) {
    Player p;
    p.velocity = {0.0f, 0.0f};
    
    p.update(0.1f);
    
    // La gravedad aumenta la velocidad Y hacia abajo
    BOOST_TEST(p.velocity.y > 0.0f);
}

// Test 9: La velocidad de caída tiene un límite máximo
BOOST_AUTO_TEST_CASE(test_fall_speed_is_capped) {
    Player p;
    p.velocity = {0.0f, 10000.0f}; // Velocidad absurda
    
    p.update(0.1f);
    
    // Debe estar bloqueada a MAX_FALL_SPEED (900.0f)
    BOOST_TEST(p.velocity.y <= 900.0f);
}

// Test 10: El enemigo patrulla horizontalmente
BOOST_AUTO_TEST_CASE(test_enemy_horizontal_patrol) {
    Rectangle bounds = {100, 100, 32, 32};
    Enemy enemy(bounds, FIRE, 50.0f, false); // horizontal
    
    float startX = enemy.position.x;
    
    // Actualizar varias veces
    for (int i = 0; i < 10; i++) {
        enemy.update(0.1f);
    }
    
    // Se movió desde la posición inicial
    BOOST_TEST(enemy.position.x != startX);
}

// Test 11: El enemigo patrulla verticalmente cuando está configurado
BOOST_AUTO_TEST_CASE(test_enemy_vertical_patrol) {
    Rectangle bounds = {100, 100, 32, 32};
    Enemy enemy(bounds, WATER, 50.0f, true); // vertical
    
    float startY = enemy.position.y;
    
    for (int i = 0; i < 10; i++) {
        enemy.update(0.1f);
    }
    
    // Se movió verticalmente
    BOOST_TEST(enemy.position.y != startY);
    // Pero no horizontalmente
    BOOST_CHECK_CLOSE(enemy.position.x, 100.0f, 0.1);
}

// Test 12: Jugador de fuego recibe daño al tocar plataforma de agua
BOOST_AUTO_TEST_CASE(test_fire_player_damaged_by_water) {
    Player firePlayer(0, 0, 10, 10, FIRE);
    firePlayer.bounds = Rectangle{10, 10, 10, 10};
    
    Platform waterPlatform(Rectangle{0, 0, 50, 50}, WATER);
    
    int result = firePlayer.checkPlatformCollision(waterPlatform);
    BOOST_TEST(result == 1); // 1 = elemento contrario
}

// Test 13: Jugador de agua recibe daño al tocar plataforma de fuego
BOOST_AUTO_TEST_CASE(test_water_player_damaged_by_fire) {
    Player waterPlayer(0, 0, 10, 10, WATER);
    waterPlayer.bounds = Rectangle{10, 10, 10, 10};
    
    Platform firePlatform(Rectangle{0, 0, 50, 50}, FIRE);
    
    int result = waterPlayer.checkPlatformCollision(firePlatform);
    BOOST_TEST(result == 1); // 1 = elemento contrario
}

// Test 14: Jugador de agua está seguro en plataforma de agua
BOOST_AUTO_TEST_CASE(test_water_player_safe_on_water) {
    Player waterPlayer(0, 0, 10, 10, WATER);
    waterPlayer.bounds = Rectangle{10, 10, 10, 10};
    
    Platform waterPlatform(Rectangle{0, 0, 50, 50}, WATER);
    
    int result = waterPlayer.checkPlatformCollision(waterPlatform);
    BOOST_TEST(result == 2); // 2 = elemento amigo
}

// Test 15: Colisión con plataforma GOAL (meta)
BOOST_AUTO_TEST_CASE(test_player_reaches_goal) {
    Player p(0, 0, 10, 10, FIRE);
    p.bounds = Rectangle{10, 10, 10, 10};
    
    Platform goal(Rectangle{0, 0, 50, 50}, GOAL);
    
    int result = p.checkPlatformCollision(goal);
    BOOST_TEST(result == 3); // 3 = meta alcanzada
}

// Test 16: No hay colisión cuando están separados
BOOST_AUTO_TEST_CASE(test_no_collision_when_separated) {
    Player p(0, 0, 10, 10, FIRE);
    p.bounds = Rectangle{0, 0, 10, 10};
    
    Platform platform(Rectangle{100, 100, 10, 10}, NONE);
    
    int result = p.checkPlatformCollision(platform);
    BOOST_TEST(result == 4); // 4 = sin colisión
}

// Test 17: El cooldown de ataque funciona correctamente
BOOST_AUTO_TEST_CASE(test_attack_cooldown_system) {
    Player p;
    
    BOOST_TEST(p.canAttack());
    
    p.attack();
    BOOST_TEST(!p.canAttack());
    
    p.update(0.6f);
    BOOST_TEST(p.canAttack());
}

// Test 18: Los proyectiles se crean activos y se pueden desactivar
BOOST_AUTO_TEST_CASE(test_projectile_activation) {
    Vector2 pos = {100.0f, 200.0f};
    Vector2 vel = {150.0f, 0.0f};
    Projectile proj(pos, vel, FIRE);
    
    BOOST_TEST(proj.isActive());
    BOOST_TEST(proj.getType() == FIRE);
    
    proj.deactivate();
    BOOST_TEST(!proj.isActive());
}

// Test 19: Los proyectiles se mueven en la dirección correcta
BOOST_AUTO_TEST_CASE(test_projectile_movement_direction) {
    Vector2 startPos = {100.0f, 100.0f};
    Vector2 vel = {200.0f, 0.0f};
    Projectile proj(startPos, vel, WATER);
    
    proj.update(0.1f);
    
    BOOST_TEST(proj.getBounds().x > startPos.x);
}

// Test 20: Los proyectiles expiran después de su tiempo de vida
BOOST_AUTO_TEST_CASE(test_projectile_expires_after_lifetime) {
    Vector2 pos = {0.0f, 0.0f};
    Vector2 vel = {100.0f, 0.0f};
    Projectile proj(pos, vel, FIRE);
    
    BOOST_TEST(proj.isActive());
    
    // MAX_LIFETIME es 0.4f, actualizamos más que eso
    proj.update(0.5f);
    
    BOOST_TEST(!proj.isActive());
}

// Test 21: La animación avanza frames correctamente
BOOST_AUTO_TEST_CASE(test_animation_frame_progression) {
    Animation anim(3, 0.1f, true, {0}, 0, 2);
    
    BOOST_TEST(anim.getCurrentFrame() == 0);
    
    anim.animationUpdate(0.1f);
    BOOST_TEST(anim.getCurrentFrame() == 1);
    
    anim.animationUpdate(0.1f);
    BOOST_TEST(anim.getCurrentFrame() == 2);
}

// Test 22: La animación con loop vuelve al inicio
BOOST_AUTO_TEST_CASE(test_animation_loops_correctly) {
    Animation anim(3, 0.1f, true, {0}, 0, 2);
    
    anim.animationUpdate(0.1f); // Frame 1
    anim.animationUpdate(0.1f); // Frame 2
    anim.animationUpdate(0.1f); // Vuelve a 0
    
    BOOST_TEST(anim.getCurrentFrame() == 0);
}

// Test 23: La cámara respeta los límites de zoom
BOOST_AUTO_TEST_CASE(test_camera_zoom_is_clamped) {
    GameCamera cam;
    cam.init({0, 0}, 1.0f, {0, 0, 1000, 1000});
    cam.setZoomLimits(0.5f, 2.0f);
    
    cam.setZoom(0.1f);
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 0.5f, 0.1);
    
    cam.setZoom(10.0f);
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 2.0f, 0.1);
    
    cam.setZoom(1.5f);
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 1.5f, 0.1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(RaytmxIntegrationTests, RaylibFixture)

// Test 24: raytmx carga correctamente un archivo TMX válido
BOOST_AUTO_TEST_CASE(test_raytmx_loads_valid_tmx_file) {
    MapLoader loader;
    
    // Cargar un mapa real del proyecto
    BOOST_CHECK_NO_THROW(loader.loadMapa("assets/levels/niveles/nivel-tutorial/zona-1.tmx"));
    
    TmxMap* mapa = loader.getMapa();
    BOOST_TEST(mapa != nullptr);
}

// Test 25: raytmx parsea correctamente las dimensiones del mapa
BOOST_AUTO_TEST_CASE(test_raytmx_parses_map_dimensions) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    TmxMap* mapa = loader.getMapa();
    
    BOOST_REQUIRE(mapa != nullptr);
    
    // Verificar que las dimensiones son válidas (mayores que 0)
    BOOST_TEST(mapa->width > 0);
    BOOST_TEST(mapa->height > 0);
    BOOST_TEST(mapa->tileWidth > 0);
    BOOST_TEST(mapa->tileHeight > 0);
}

// Test 26: MapLoader calcula correctamente los límites del mapa desde raytmx
BOOST_AUTO_TEST_CASE(test_maploader_calculates_map_bounds_from_raytmx) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    TmxMap* mapa = loader.getMapa();
    
    BOOST_REQUIRE(mapa != nullptr);
    
    Rectangle bounds = loader.getMapBounds();
    
    // Los límites deben coincidir con width*tileWidth y height*tileHeight
    float expectedWidth = (float)(mapa->width * mapa->tileWidth);
    float expectedHeight = (float)(mapa->height * mapa->tileHeight);
    
    BOOST_CHECK_CLOSE(bounds.width, expectedWidth, 0.01);
    BOOST_CHECK_CLOSE(bounds.height, expectedHeight, 0.01);
    BOOST_CHECK_CLOSE(bounds.x, 0.0f, 0.01);
    BOOST_CHECK_CLOSE(bounds.y, 0.0f, 0.01);
}

// Test 27: raytmx detecta capas en el mapa TMX
BOOST_AUTO_TEST_CASE(test_raytmx_detects_layers_in_tmx) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    TmxMap* mapa = loader.getMapa();
    
    BOOST_REQUIRE(mapa != nullptr);
    
    // Un mapa válido debe tener al menos una capa
    BOOST_TEST(mapa->layersLength > 0);
}

// Test 28: MapLoader extrae plataformas desde datos de raytmx
BOOST_AUTO_TEST_CASE(test_maploader_extracts_platforms_from_raytmx) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    
    std::vector<Platform> platforms = loader.extractPlatforms();
    
    // El mapa debe tener plataformas de colisión definidas
    BOOST_TEST(platforms.size() > 0);
    
    // Verificar que las plataformas tienen dimensiones válidas
    for (const auto& platform : platforms) {
        BOOST_TEST(platform.bounds.width > 0);
        BOOST_TEST(platform.bounds.height > 0);
    }
}

// Test 29: MapLoader maneja correctamente un mapa no cargado
BOOST_AUTO_TEST_CASE(test_maploader_handles_null_map) {
    MapLoader loader; // Sin cargar mapa
    
    // getMapBounds debe devolver rectángulo vacío
    Rectangle bounds = loader.getMapBounds();
    BOOST_CHECK_CLOSE(bounds.width, 0.0f, 0.01);
    BOOST_CHECK_CLOSE(bounds.height, 0.0f, 0.01);
    
    // extractPlatforms debe devolver vector vacío sin crashear
    std::vector<Platform> platforms = loader.extractPlatforms();
    BOOST_TEST(platforms.empty());
    
    // extractEnemies debe devolver vector vacío sin crashear
    std::vector<Enemy> enemies = loader.extractEnemies();
    BOOST_TEST(enemies.empty());
}

// Test 30: raytmx carga correctamente los tilesets del mapa
BOOST_AUTO_TEST_CASE(test_raytmx_loads_tilesets) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    TmxMap* mapa = loader.getMapa();
    
    BOOST_REQUIRE(mapa != nullptr);
    
    // Un mapa con tiles debe tener al menos un tileset
    BOOST_TEST(mapa->tilesetsLength > 0);
    
    // El primer tileset debe tener un firstGid válido (normalmente 1)
    if (mapa->tilesetsLength > 0) {
        BOOST_TEST(mapa->tilesets[0].firstGid >= 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
