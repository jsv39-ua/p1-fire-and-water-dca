# Cómo ejecutar los Tests Unitarios

Este proyecto utiliza **GoogleTest** como framework de pruebas y **CTest** como gestor de ejecución para verificar la lógica del juego.

Los tests están **desactivados por defecto** para no ralentizar el desarrollo normal del juego. Para ejecutarlos, debes seguir estas instrucciones.

## 🚀 Guía Rápida

Para habilitar y ejecutar los tests, debes configurar CMake con la opción personalizada `BUILD_GAME_TESTS=ON`.

### Paso 1: Configurar (Activando Tests)

Abre una terminal en la raíz del proyecto (donde está el `CMakeLists.txt` principal) y ejecuta:

```bash
# Recomendable: Si ya tienes una carpeta build, límpiala para evitar conflictos de caché
rm -rf build 
```
```bash
# Configura el proyecto activando el flag específico de tests
cmake -S . -B build -DBUILD_GAME_TESTS=ON
```
### Paso 2: Compilar

Compila el proyecto. Esto generará tanto el juego como el ejecutable de pruebas (unit_tests).

```Bash
cmake --build build
```
Nota: Al compilar, también se copian automáticamente los archivos de la carpeta assets/ dentro de build/ para que los tests puedan encontrarlos.

### Paso 3: Ejecutar los Tests

Entra en la carpeta de compilación y utiliza CTest. Usamos el flag --output-on-failure para que, si algo falla, nos dé detalles útiles.

```Bash
cd build
ctest --output-on-failure
```

### 🛠️ Solución de Problemas Comunes
##### Error: "No tests were found!!!"
**Causa**: CTest no encuentra pruebas registradas. Solución: Probablemente configuraste el proyecto sin el flag -DBUILD_GAME_TESTS=ON. Borra la carpeta build y repite el Paso 1.

##### Error: "SettingsFileExists Failed" (o fallos de carga de Assets)
**Causa**: Los tests intentan cargar archivos (json, mp3) y no los encuentran. Solución: Asegúrate de ejecutar cmake --build build después de cualquier cambio. CMake tiene una regla file(COPY...) que actualiza los assets en la carpeta de construcción.

##### Advertencias: "Clock skew detected"
**Causa**: Si estás usando WSL (Linux en Windows), esto ocurre por la sincronización de archivos entre sistemas. Solución: Puedes ignorarlo completamente si el mensaje final dice [100%] Built target....

## EXPLICACIÓN DE QUE HACE CADA TEST:
## 🧪 Explicación de los Tests

La suite actual consta de 10 tests automatizados. Aquí tienes el detalle de qué verifica cada uno:

### 🔹 Patrones de Diseño (Singletons)
* **Test 1 - SettingsManagerTest.IsSingleton:** Verifica que el gestor de configuración sea único en todo el programa (patrón Singleton), devolviendo siempre la misma dirección de memoria.
* **Test 2 - ResourceManagerTest.IsSingleton:** Asegura que solo exista una instancia del gestor de recursos para evitar cargas duplicadas de texturas o sonidos.
* **Test 3 - AudioManagerTest.IsSingleton:** Comprueba que el sistema de audio es único para prevenir conflictos en la reproducción de sonidos.

### 🔹 Lógica del Juego (Core)
* **Test 4 - PlayerTest.Initialization:** Valida que la clase `Player` se puede instanciar correctamente sin provocar errores de memoria o excepciones en su constructor.
* **Test 5 - EnemyTest.Initialization:** Comprueba la estabilidad al crear objetos de tipo `Enemy`.
* **Test 6 - StateMachineTest.InitialState:** Test crítico que verifica la gestión de memoria de la Máquina de Estados. Se asegura de que puede recibir y gestionar nuevos estados utilizando punteros inteligentes (`unique_ptr`) sin violaciones de segmento.

### 🔹 Sistemas y Recursos
* **Test 7 - MapLoaderTest.ClassExists:** Test de integridad básica para confirmar que el cargador de mapas se compila y enlaza correctamente con el proyecto.
* **Test 8 - ConfigTest.SettingsFileExists:** Verifica que el archivo de configuración (`assets/config/settings.json`) existe y es accesible desde el ejecutable. Si falla, suele indicar problemas en la copia de assets.
* **Test 9 - AssetsTest.BackgroundMusicExists:** Comprueba que los recursos multimedia (música de fondo) se han copiado correctamente a la carpeta de compilación.
* **Test 10 - RaylibMath.VectorAddition:** Test de integración ("Smoke Test") que realiza una operación matemática usando Raylib. Sirve para confirmar que la librería gráfica externa está correctamente enlazada y operativa.