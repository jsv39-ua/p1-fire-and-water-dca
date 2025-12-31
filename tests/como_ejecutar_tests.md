# Cómo ejecutar los Tests Unitarios

Este proyecto utiliza **Boost.Test** como framework de pruebas (según el temario del tema 7) y **CTest** como gestor de ejecución para verificar la lógica del juego.

Los tests están **desactivados por defecto** para no ralentizar el desarrollo normal del juego.

## Guía Rápida

### Paso 1: Configurar (Activando Tests)

```bash
# Limpiar carpeta build si existe
rm -rf build 

# Configurar con tests habilitados
cmake -S . -B build -DBUILD_GAME_TESTS=ON
```

### Paso 2: Compilar

```bash
cmake --build build
```

### Paso 3: Ejecutar Tests con CTest

```bash
cd build
ctest --output-on-failure
```

### Opciones útiles de CTest

```bash
# Ver output detallado
ctest --verbose

# Ejecutar tests en paralelo
ctest -j4

# Ver lista de suites disponibles
./tests/unit_tests --list_content

# Ejecutar una suite específica
./tests/unit_tests --run_test=PlayerLogicTests
```

## Solución de Problemas

### Error: "Could not find Boost"
```bash
# Ubuntu/Debian
sudo apt-get install libboost-test-dev
```

### Warnings de audio
Los warnings de audio (`Failed to create buffer`) son normales en entornos sin dispositivo de audio. Los tests manejan esto correctamente.

## Suites de Tests (30 tests)

| Suite | Tests | Descripción |
|-------|-------|-------------|
| `SingletonTests` | 3 | Patrones Singleton |
| `EntityTests` | 4 | Inicialización de entidades |
| `AssetTests` | 2 | Verificación de archivos |
| `RaylibTests` | 2 | Tests de librería externa |
| `PlayerLogicTests` | 7 | Lógica del jugador |
| `GameplayLogicTests` | 1 | Mecánica Fire vs Water |
| `EnemyLogicTests` | 2 | IA de enemigos |
| `CombatLogicTests` | 1 | Sistema de proyectiles |
| `AnimationSystemTests` | 2 | Sistema de animaciones |
| `CameraSystemTests` | 2 | Sistema de cámara |
| `SettingsSystemTests` | 2 | Configuración |
| `AudioSystemTests` | 1 | Sistema de audio |
| `ResourceSystemTests` | 1 | Gestión de recursos |

## CI

Los tests se ejecutan automáticamente en GitHub Actions en:
- Pull requests hacia `main`, `master` o `develop`
- Push a `main`, `master` o `develop`

Ver `.github/workflows/tests.yml`
