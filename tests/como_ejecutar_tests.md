# Cómo ejecutar los Tests Unitarios

Este proyecto utiliza **Boost.Test** como framework de pruebas y **CTest** como gestor de ejecución.

## Guía Rápida

```bash
# Configurar con tests habilitados
cmake -S . -B build -DBUILD_GAME_TESTS=ON

# Compilar
cmake --build build

# Ejecutar tests en Linux
cd build && ctest --output-on-failure

# En Windows:
cd build; ctest --output-on-failure -C Debug
```

## CI

Los tests se ejecutan automáticamente en GitHub Actions (`.github/workflows/tests.yml`) en:
- Pull requests hacia `main`
- Push a `main`
