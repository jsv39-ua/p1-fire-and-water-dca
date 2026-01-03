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
#include "systems/AudioManager.hpp"
#include "systems/LocalizationManager.hpp"
#include <raytmx.h>
#include <chrono>
#include <vector>
#include <memory>

// Fixture para tests que requieren contexto de raylib/raytmx
struct RaylibFixture {
    bool audioInitialized = false;
    bool windowInitialized = false;
    
    RaylibFixture() {
        SetTraceLogLevel(LOG_NONE);
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
        InitWindow(1, 1, "Test");
        windowInitialized = IsWindowReady();
        if (windowInitialized) {
            InitAudioDevice();
            audioInitialized = IsAudioDeviceReady();
        }
    }
    ~RaylibFixture() {
        if (windowInitialized && IsWindowReady()) {
            ResourceManager::cleanup();
            if (audioInitialized && IsAudioDeviceReady()) {
                CloseAudioDevice();
            }
            CloseWindow();
        }
    }
};

// FUNCIONALIDADES DEL JUEGO
BOOST_FIXTURE_TEST_SUITE(GameFunctionalityTests, RaylibFixture)

// Test 1: El jugador pierde una vida al recibir daño
BOOST_AUTO_TEST_CASE(test_player_loses_one_life) {
    Player p;
    p.setLifeCount(3);
    
    p.updateLifeCount(-1);
    
    BOOST_TEST(p.getLifeCount() == 2);
}

// Test 2: Las vidas no pueden ser negativas
BOOST_AUTO_TEST_CASE(test_player_lives_cannot_be_negative) {
    Player p;
    p.setLifeCount(0);
    
    p.updateLifeCount(-5);
    
    BOOST_TEST(p.getLifeCount() == 0);
}

// Test 3: El jugador obtiene invencibilidad tras recibir daño
BOOST_AUTO_TEST_CASE(test_player_becomes_invincible_after_damage) {
    Player p;
    
    p.updateLifeCount(-1);
    
    BOOST_TEST(p.isInvincible());
}

// Test 4: La invencibilidad desaparece con el tiempo
BOOST_AUTO_TEST_CASE(test_player_invincibility_expires) {
    Player p;
    p.updateLifeCount(-1);
    
    for (int i = 0; i < 15; i++) {
        p.update(0.1f);
    }
    
    BOOST_TEST(!p.isInvincible());
}

// Test 5: El jugador se mueve a la derecha
BOOST_AUTO_TEST_CASE(test_player_walks_right) {
    Player p;
    p.velocity = {0.0f, 0.0f};
    
    p.walk(1);
    
    BOOST_TEST(p.velocity.x > 0.0f);
}

// Test 6: El jugador salta
BOOST_AUTO_TEST_CASE(test_player_jumps) {
    Player p;
    p.velocity = {0.0f, 0.0f};
    
    p.jump();
    
    BOOST_TEST(p.velocity.y < 0.0f);
}

// Test 7: La gravedad se aplica al jugador
BOOST_AUTO_TEST_CASE(test_gravity_applies_to_player) {
    Player p;
    p.velocity = {0.0f, 0.0f};
    
    p.update(0.1f);
    
    BOOST_TEST(p.velocity.y > 0.0f);
}

// Test 8: La velocidad de caída tiene límite máximo
BOOST_AUTO_TEST_CASE(test_fall_speed_has_limit) {
    Player p;
    p.velocity = {0.0f, 10000.0f};
    
    p.update(0.1f);
    
    BOOST_TEST(p.velocity.y <= 900.0f);
}

// Test 9: Jugador de fuego recibe daño en agua
BOOST_AUTO_TEST_CASE(test_fire_player_damaged_by_water) {
    Player firePlayer(0, 0, 32, 32, FIRE);
    firePlayer.bounds = Rectangle{10, 10, 32, 32};
    Platform waterPlatform(Rectangle{0, 0, 50, 50}, WATER);
    
    int result = firePlayer.checkPlatformCollision(waterPlatform);
    
    BOOST_TEST(result == 1);
}

// Test 10: Jugador de fuego seguro en fuego
BOOST_AUTO_TEST_CASE(test_fire_player_safe_on_fire) {
    Player firePlayer(0, 0, 32, 32, FIRE);
    firePlayer.bounds = Rectangle{10, 10, 32, 32};
    Platform firePlatform(Rectangle{0, 0, 50, 50}, FIRE);
    
    int result = firePlayer.checkPlatformCollision(firePlatform);
    
    BOOST_TEST(result == 2);
}

// Test 11: Jugador alcanza la meta
BOOST_AUTO_TEST_CASE(test_player_reaches_goal) {
    Player player(0, 0, 32, 32, FIRE);
    player.bounds = Rectangle{10, 10, 32, 32};
    Platform goal(Rectangle{0, 0, 50, 50}, GOAL);
    
    int result = player.checkPlatformCollision(goal);
    
    BOOST_TEST(result == 3);
}

// Test 12: El enemigo patrulla horizontalmente
BOOST_AUTO_TEST_CASE(test_enemy_patrols_horizontally) {
    Rectangle bounds = {100, 100, 32, 32};
    Enemy horizontalEnemy(bounds, FIRE, 50.0f, false);
    float startX = horizontalEnemy.position.x;
    
    for (int i = 0; i < 10; i++) {
        horizontalEnemy.update(0.1f);
    }
    
    BOOST_TEST(horizontalEnemy.position.x != startX);
}

// Test 13: El proyectil se mueve según su velocidad
BOOST_AUTO_TEST_CASE(test_projectile_moves) {
    Vector2 startPos = {100.0f, 100.0f};
    Vector2 vel = {200.0f, 0.0f};
    Projectile proj(startPos, vel, FIRE);
    
    proj.update(0.1f);
    
    BOOST_TEST(proj.getBounds().x > startPos.x);
}

// Test 14: El proyectil expira después de su tiempo de vida
BOOST_AUTO_TEST_CASE(test_projectile_expires) {
    Vector2 startPos = {100.0f, 100.0f};
    Vector2 vel = {200.0f, 0.0f};
    Projectile proj(startPos, vel, FIRE);
    
    proj.update(0.5f);
    
    BOOST_TEST(!proj.isActive());
}

// Test 15: El jugador no puede atacar durante cooldown
BOOST_AUTO_TEST_CASE(test_player_cannot_attack_during_cooldown) {
    Player p;
    
    p.attack();
    
    BOOST_TEST(!p.canAttack());
}

// Test 16: El cooldown de ataque expira
BOOST_AUTO_TEST_CASE(test_attack_cooldown_expires) {
    Player p;
    p.attack();
    
    p.update(0.6f);
    
    BOOST_TEST(p.canAttack());
}

// Test 17: La animación avanza al siguiente frame
BOOST_AUTO_TEST_CASE(test_animation_advances_frame) {
    Animation anim(3, 0.1f, true, {0}, 0, 2);
    
    anim.animationUpdate(0.1f);
    
    BOOST_TEST(anim.getCurrentFrame() == 1);
}

// Test 18: La animación hace loop al inicio
BOOST_AUTO_TEST_CASE(test_animation_loops) {
    Animation anim(3, 0.1f, true, {0}, 0, 2);
    anim.animationUpdate(0.1f);
    anim.animationUpdate(0.1f);
    
    anim.animationUpdate(0.1f);
    
    BOOST_TEST(anim.getCurrentFrame() == 0);
}

// Test 19: El zoom de cámara respeta límites
BOOST_AUTO_TEST_CASE(test_camera_respects_zoom_limits) {
    GameCamera cam;
    cam.init({0, 0}, 1.0f, {0, 0, 1000, 1000});
    cam.setZoomLimits(0.5f, 2.0f);
    
    cam.setZoom(0.1f);
    
    BOOST_CHECK_CLOSE(cam.getTargetZoom(), 0.5f, 0.1);
}

// Test 20: Configuración de teclas del jugador
BOOST_AUTO_TEST_CASE(test_player_key_configuration) {
    Player player;
    
    player.setKeys(KEY_A, KEY_D, KEY_W, KEY_SPACE);
    
    BOOST_TEST(player.getKeyLeft() == KEY_A);
    BOOST_TEST(player.getKeyRight() == KEY_D);
}

// Test 21: Volumen de audio configurable
BOOST_AUTO_TEST_CASE(test_audio_volume_configurable) {
    AudioManager* audio = AudioManager::getInstance();
    audio->init();
    
    audio->setMasterVolume(0.5f);
    
    BOOST_CHECK_CLOSE(audio->getMasterVolume(), 0.5f, 0.01);
}

BOOST_AUTO_TEST_SUITE_END()

// INTEGRACIÓN CON SISTEMAS EXTERNOS
BOOST_FIXTURE_TEST_SUITE(ExternalSystemsTests, RaylibFixture)

// Test 22: raytmx carga correctamente un archivo TMX válido
BOOST_AUTO_TEST_CASE(test_raytmx_loads_valid_tmx_file) {
    MapLoader loader;
    
    loader.loadMapa("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    
    BOOST_TEST(loader.getMapa() != nullptr);
}

// Test 23: raytmx parsea las dimensiones del mapa
BOOST_AUTO_TEST_CASE(test_raytmx_parses_map_dimensions) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    
    TmxMap* mapa = loader.getMapa();
    
    BOOST_REQUIRE(mapa != nullptr);
    BOOST_TEST(mapa->width > 0);
    BOOST_TEST(mapa->height > 0);
}

// Test 24: MapLoader calcula los límites del mapa en píxeles
BOOST_AUTO_TEST_CASE(test_maploader_calculates_map_bounds) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    TmxMap* mapa = loader.getMapa();
    BOOST_REQUIRE(mapa != nullptr);
    float expectedWidth = (float)(mapa->width * mapa->tileWidth);
    
    Rectangle bounds = loader.getMapBounds();
    
    BOOST_CHECK_CLOSE(bounds.width, expectedWidth, 0.01);
}

// Test 25: MapLoader extrae plataformas
BOOST_AUTO_TEST_CASE(test_maploader_extracts_platforms) {
    MapLoader loader("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    
    std::vector<Platform> platforms = loader.extractPlatforms();
    
    BOOST_TEST(platforms.size() > 0);
}

// Test 26: MapLoader retorna vacío sin mapa cargado
BOOST_AUTO_TEST_CASE(test_maploader_handles_null_map) {
    MapLoader loader;
    
    std::vector<Platform> platforms = loader.extractPlatforms();
    
    BOOST_TEST(platforms.empty());
}

// Test 27: ResourceManager es singleton y cachea texturas
BOOST_AUTO_TEST_CASE(test_resource_manager_singleton_and_cache) {
    ResourceManager* rm = ResourceManager::getInstance();
    
    Texture2D tex1 = rm->loadTexture("assets/images/p1-fire-and-water-dca-logo.png");
    Texture2D tex2 = rm->loadTexture("assets/images/p1-fire-and-water-dca-logo.png");
    
    BOOST_TEST(tex1.id == tex2.id);
}

// Test 28: LocalizationManager cambia idioma
BOOST_AUTO_TEST_CASE(test_localization_changes_language) {
    LocalizationManager* loc = LocalizationManager::getInstance();
    loc->init();
    
    loc->setLanguage("es");
    
    BOOST_TEST(loc->getCurrentLanguage() == "es");
}

// Test 29: MapLoader soporta ciclos carga/descarga
BOOST_AUTO_TEST_CASE(test_maploader_load_unload_cycle) {
    MapLoader loader;
    loader.loadMapa("assets/levels/niveles/nivel-tutorial/zona-1.tmx");
    
    loader.unloadMapa();
    
    BOOST_TEST(loader.getMapa() == nullptr);
}

// Test 30: Rendimiento - actualización de múltiples entidades
BOOST_AUTO_TEST_CASE(test_entity_update_performance) {
    std::vector<Player> players;
    std::vector<Enemy> enemies;
    for (int i = 0; i < 50; i++) {
        players.emplace_back((float)i * 20, 100.0f, 32.0f, 32.0f, FIRE);
        enemies.emplace_back(Rectangle{(float)i * 20, 200.0f, 32, 32}, WATER, 30.0f, false);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int frame = 0; frame < 60; frame++) {
        for (auto& p : players) p.update(0.016f);
        for (auto& e : enemies) e.update(0.016f);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    BOOST_TEST(duration.count() < 1000);
}

BOOST_AUTO_TEST_SUITE_END()
