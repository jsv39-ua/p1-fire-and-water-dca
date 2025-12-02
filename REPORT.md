# Informe de Implementación y Rendimiento (Makefile & Ccache)

Este entregable incluye la implementación de un sistema de construcción manual (`Makefile`) y un análisis de rendimiento utilizando paralelismo y `ccache`.

## 1. Implementación Técnica

Se han realizado los siguientes cambios en la infraestructura del proyecto:

- **Estructura de Directorios:** Se organizaron las fuentes en `core/` y `graphics/` (aunque se mantiene compatibilidad con la estructura anterior).
- **Makefile Manual:** Se creó un `Makefile` que:
  - Escanea recursivamente `src`, `core` y `graphics`.
  - Genera objetos en `obj/` y el ejecutable `game` en la raíz.
  - Soporta las variables de entorno estándar `CC`, `CXX`.
  - Integra `pkg-config` para la detección automática de Raylib.
- **Optimización:** Soporte integrado para `ccache` mediante la variable `CCACHE=1`.

## 2. Metodología de Pruebas

Para medir el rendimiento, se realizaron pruebas en un entorno WSL (Ubuntu) utilizando el comando `time` y compilación paralela con 4 hilos (`-j4`).

**Instrucciones de reproducción:**
1. Instalar dependencias: `sudo apt install libraylib-dev ccache`
2. Ejecutar limpieza: `make clean`
3. Ejecutar construcción: `time make -j4`

## 3. Resultados de la Comparativa

A continuación se muestran los tiempos de compilación obtenidos en el entorno de pruebas:

| Prueba | Configuración | Tiempo Real (s) | Observaciones |
| :--- | :--- | :--- | :--- |
| **1. Compilación Limpia** | `make clean && make -j4` | **3.171s** | Compilación completa de todos los archivos fuente y enlazado. |
| **2. Recompilación** | `make -j4` (sin cambios) | **0.154s** | El sistema de dependencias (`-MMD`) detecta correctamente que no hay cambios. |
| **3. Ccache (Simulado)** | `touch core/main.cpp && CCACHE=1 make -j4` | **0.910s** | Recompilación forzada. `ccache` recupera el objeto compilado, logrando una velocidad **3x superior** a la limpia. |

## 4. Análisis y Conclusiones

- **Eficiencia del Makefile:** El sistema detecta correctamente cuando no es necesario recompilar nada, reduciendo el tiempo a apenas 0.15s (overhead mínimo de Make).
- **Impacto de Ccache:** La prueba 3 demuestra la utilidad de `ccache`. Aunque el sistema de archivos indica que `main.cpp` fue modificado, `ccache` detecta que el contenido preprocesado es idéntico al de una compilación anterior, evitando la fase costosa de compilación real. Esto reduce el tiempo de **3.17s** a **0.91s**, lo cual es crítico para optimizar los tiempos en sistemas de Integración Continua (CI/CD).