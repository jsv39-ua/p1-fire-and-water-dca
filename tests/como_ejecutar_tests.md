# Cómo ejecutar los Tests Unitarios

Este proyecto utiliza **Boost.Test** como framework de pruebas y **CTest** como gestor de ejecución.

## Guía Rápida

```bash
# Configurar con tests habilitados
cmake -S . -B build -DBUILD_GAME_TESTS=ON

# Compilar
cmake --build build

# Ejecutar tests
cd build && ctest --output-on-failure
```

---

## Suite de Tests: GameTests (23 tests)

Una única suite que prueba las funcionalidades más relevantes del juego.

---

### 1. Sistema de Vidas y Daño (4 tests)

#### `test_player_loses_lives_on_damage`
**Qué prueba:** Que el jugador pierde vidas correctamente cuando recibe daño.

**Cómo funciona:** 
1. Crea un jugador con 3 vidas
2. Llama a `updateLifeCount(-1)` tres veces
3. Verifica que las vidas pasan de 3 → 2 → 1 → 0

---

#### `test_player_lives_cannot_be_negative`
**Qué prueba:** Que las vidas nunca pueden ser un número negativo.

**Cómo funciona:**
1. Crea un jugador con 1 vida
2. Intenta quitarle vidas múltiples veces (-1, -1, -5)
3. Verifica que el contador se queda en 0, no en -6

---

#### `test_player_invincibility_after_damage`
**Qué prueba:** Que el jugador obtiene invencibilidad temporal tras recibir daño.

**Cómo funciona:**
1. Verifica que el jugador NO es invencible inicialmente
2. Le quita una vida (simula recibir daño)
3. Verifica que AHORA es invencible
4. Simula el paso del tiempo (2 segundos)
5. Verifica que la invencibilidad ha expirado

---

#### `test_enemy_takes_damage`
**Qué prueba:** Que los enemigos también tienen sistema de vidas e invencibilidad.

**Cómo funciona:**
1. Crea un enemigo y guarda sus vidas iniciales
2. Le quita una vida
3. Verifica que tiene una vida menos Y que está invencible

---

### 2. Sistema de Movimiento y Física (7 tests)

#### `test_player_horizontal_movement`
**Qué prueba:** Que el movimiento horizontal funciona correctamente.

**Cómo funciona:**
1. Llama a `walk(1)` → verifica que velocity.x es positiva (derecha)
2. Llama a `walk(-1)` → verifica que velocity.x es negativa (izquierda) y simétrica
3. Llama a `walk(0)` → verifica que velocity.x es 0 (parado)

---

#### `test_player_jump_applies_upward_velocity`
**Qué prueba:** Que el salto aplica velocidad hacia arriba.

**Cómo funciona:**
1. Pone la velocidad del jugador a 0
2. Llama a `jump()`
3. Verifica que velocity.y es negativa (en coordenadas de pantalla, negativo = arriba)
4. Verifica que la fuerza es significativa (< -100)

---

#### `test_variable_jump_height`
**Qué prueba:** Que mantener presionado el botón de salto da más altura.

**Cómo funciona:**
1. Pone al jugador con velocidad vertical -100 (saltando)
2. Llama a `updateJump(deltaTime, true)` → mantiene el botón
3. Verifica que la velocidad aumentó (más negativa = más arriba)
4. Llama a `updateJump(deltaTime, false)` → suelta el botón
5. Verifica que la velocidad no cambió

---

#### `test_gravity_applies_to_player`
**Qué prueba:** Que la gravedad afecta al jugador cada frame.

**Cómo funciona:**
1. Pone velocity.y a 0
2. Llama a `update(0.1f)` (simula un frame)
3. Verifica que velocity.y es ahora positiva (cayendo)

---

#### `test_fall_speed_is_capped`
**Qué prueba:** Que hay una velocidad máxima de caída.

**Cómo funciona:**
1. Pone velocity.y a 10000 (velocidad absurda)
2. Llama a `update()`
3. Verifica que velocity.y está limitada a 900

---

#### `test_enemy_horizontal_patrol`
**Qué prueba:** Que los enemigos patrullan horizontalmente.

**Cómo funciona:**
1. Crea un enemigo con patrulla horizontal
2. Guarda su posición X inicial
3. Actualiza varias veces
4. Verifica que la posición X cambió

---

#### `test_enemy_vertical_patrol`
**Qué prueba:** Que los enemigos pueden patrullar verticalmente.

**Cómo funciona:**
1. Crea un enemigo con `verticalMovement = true`
2. Guarda posición Y inicial
3. Actualiza varias veces
4. Verifica que Y cambió pero X se mantuvo igual

---

### 3. Sistema Elemental Fire vs Water (5 tests)

#### `test_fire_player_damaged_by_water`
**Qué prueba:** Que un jugador de fuego recibe daño al tocar agua.

**Cómo funciona:**
1. Crea un jugador tipo FIRE
2. Crea una plataforma tipo WATER
3. Verifica colisión → debe retornar 1 (elemento contrario = daño)

---

#### `test_water_player_damaged_by_fire`
**Qué prueba:** Lo mismo pero al revés: jugador de agua + plataforma de fuego.

---

#### `test_water_player_safe_on_water`
**Qué prueba:** Que un jugador de agua es seguro en plataformas de agua.

**Cómo funciona:**
1. Crea jugador WATER + plataforma WATER
2. Verifica colisión → debe retornar 2 (elemento amigo = seguro)

---

#### `test_player_reaches_goal`
**Qué prueba:** Que se detecta cuando el jugador llega a la meta.

**Cómo funciona:**
1. Crea jugador + plataforma tipo GOAL
2. Verifica colisión → debe retornar 3 (meta alcanzada)

---

#### `test_no_collision_when_separated`
**Qué prueba:** Que no hay colisión cuando jugador y plataforma están separados.

**Cómo funciona:**
1. Crea jugador en (0,0) y plataforma en (100,100)
2. Verifica colisión → debe retornar 4 (sin colisión)

---

### 4. Sistema de Combate (4 tests)

#### `test_attack_cooldown_system`
**Qué prueba:** Que hay un cooldown entre ataques.

**Cómo funciona:**
1. Verifica que `canAttack()` es true inicialmente
2. Llama a `attack()`
3. Verifica que `canAttack()` es false (en cooldown)
4. Simula 0.6 segundos
5. Verifica que `canAttack()` es true de nuevo

---

#### `test_projectile_activation`
**Qué prueba:** Que los proyectiles se crean activos y se pueden desactivar.

**Cómo funciona:**
1. Crea un proyectil
2. Verifica que está activo y tiene el tipo correcto (FIRE)
3. Llama a `deactivate()`
4. Verifica que ya no está activo

---

#### `test_projectile_movement_direction`
**Qué prueba:** Que los proyectiles se mueven en la dirección correcta.

**Cómo funciona:**
1. Crea proyectil en X=100 con velocidad positiva
2. Actualiza
3. Verifica que la nueva X es mayor que 100

---

#### `test_projectile_expires_after_lifetime`
**Qué prueba:** Que los proyectiles desaparecen después de su tiempo de vida.

**Cómo funciona:**
1. Crea un proyectil (MAX_LIFETIME = 0.4 segundos)
2. Verifica que está activo
3. Actualiza con 0.5 segundos
4. Verifica que ya no está activo

---

### 5. Sistema de Animación (2 tests)

#### `test_animation_frame_progression`
**Qué prueba:** Que los frames de animación avanzan correctamente.

**Cómo funciona:**
1. Crea animación de 3 frames (0.1s cada uno)
2. Verifica frame inicial = 0
3. Actualiza 0.1s → verifica frame = 1
4. Actualiza 0.1s → verifica frame = 2

---

#### `test_animation_loops_correctly`
**Qué prueba:** Que las animaciones con loop vuelven al principio.

**Cómo funciona:**
1. Crea animación de 3 frames con loop
2. Avanza: frame 0 → 1 → 2 → 0 (loop)
3. Verifica que después del último frame vuelve al 0

---

### 6. Sistema de Cámara (1 test)

#### `test_camera_zoom_is_clamped`
**Qué prueba:** Que el zoom de la cámara respeta límites mínimo y máximo.

**Cómo funciona:**
1. Configura límites de zoom: mínimo 0.5, máximo 2.0
2. Intenta zoom 0.1 → verifica que queda en 0.5 (mínimo)
3. Intenta zoom 10.0 → verifica que queda en 2.0 (máximo)
4. Pone zoom 1.5 → verifica que queda en 1.5 (válido)

---

## CI

Los tests se ejecutan automáticamente en GitHub Actions (`.github/workflows/tests.yml`) en:
- Pull requests hacia `main`, `master` o `develop`
- Push a `main`, `master` o `develop`
