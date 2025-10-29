# Memoria del Proyecto: (Hito 1: Alpha)

- **Versión del documento:** 1.1
- **Fecha:** 29 de Octubre de 2025
- **Autores:**
    - Fabian Godoy Silvestre
    - Saúl Conejo Mínguez
    - Javier Sánchez Villaescusa
    - Juan Carlos Ponce de León Ruiz

## 1. Game Design Document (GDD)

### Introducción

- **Título:** Elements
- **Estudio o Diseñadores:** Team Lemon.
- **Género:** Plataformas 2D, Cooperativo.
- **Plataforma:** PC (Windows, Linux).
- **Versión del juego:** 0.1.0-alpha.

### Sinopsis y Contenido

"Elements" es un juego de plataformas cooperativo en 2D donde dos jugadores controlan a dos personajes elementales. Un jugador maneja al personaje de Fuego, que es inmune al fuego pero vulnerable al agua, y el otro al personaje de Agua, que puede atravesar zonas de agua pero es vulnerable al fuego. El objetivo es que ambos jugadores colaboren para superar obstáculos, resolver puzzles y llegar al final de cada nivel, utilizando sus habilidades complementarias. También es posible jugar un solo jugador, pero los niveles estarán pensados para completarse rápidamente, por lo que puede ser un desafío.

### Alcance del Proyecto (Hito 1)

Para esta primera entrega (Alpha), el juego es un mínimo producto jugable que incluye:

- Un nivel de prueba completamente funcional.
- Las mecánicas principales de movimiento, salto y colisiones.
- Peligros elementales (lava y agua) y enemigos básicos que reaccionan a los personajes.
- Un ciclo de juego completo: Menú Principal -> Juego -> Pantalla de Game Over -> Reinicio.

### Categoría y Referentes

El juego se inspira en la popular saga de juegos Flash "Fireboy and Watergirl", compartiendo la mecánica central de cooperación entre dos personajes con habilidades elementales opuestas. Sin embargo, nuestro proyecto busca una jugabilidad más fluida y una física moderna gracias al uso de la librería Raylib. También estamos pensando en implementar enemigos móviles y tal vez otros elementos, de ahí el cambio de nombre a Elements. De igual manera, en nuestro videojuego queremos implementar mecánicas de ataque y diferentes habilidades exclusivas para cada tipo de elemento

### Guión

En esta fase del desarrollo, el juego no cuenta con una narrativa profunda. La premisa es simple: los dos líderes elementales, Fuego y Agua, deben colaborar para escapar de una serie de mazmorras. Más adelante se implementarán los reinos de Tierra y Aire si nos da el tiempo. El guión se centra en la jugabilidad emergente que surge de la interacción y la necesidad de cooperación entre los jugadores.

### Mecánicas de Juego

- **Cámara:** La vista del juego es en 2D con una cámara estática que muestra la totalidad del nivel en una sola pantalla. Cada nivel estará formado por varias pantallas, pudiendo los jugadores desplazarse entre estas.
- **Periféricos:** El juego está diseñado para ser jugado con un teclado.
- **Controles:**
    - **Personaje de Fuego:**
        - Movimiento: Teclas A (izquierda) y D (derecha).
        - Salto: Tecla W.
        - Ataque: Por determinar
    - **Personaje de Agua:**
        - Movimiento: Flechas Izquierda y Derecha.
        - Salto: Flecha Arriba.
        - Ataque: Por determinar
- **Físicas y Colisiones:**
    - Se aplica una gravedad constante a ambos personajes.
    - Los jugadores colisionan con las plataformas, lo que les permite caminar y saltar sobre ellas.
    - Si el personaje de Fuego entra en contacto con un peligro de agua (estático o un enemigo), la partida termina.
    - Si el personaje de Agua entra en contacto con un peligro de fuego, la partida termina.
- **Estados de Juego:** El juego se gestiona a través de una máquina de estados que controla las diferentes pantallas: Menú Principal, Juego, Pausa, Opciones y Game Over.
- **Guardar/Cargar:** Esta funcionalidad no está implementada en la versión Alpha.

### Interfaces (UI)

La interfaz de usuario es minimalista y funcional, diseñada para ser clara e intuitiva.

- **Menú Principal (MainMenuState):** Esto es lo primero que ves. Hemos puesto el logo del juego arriba y centrado y, abajo, tres botones básicos: "JUGAR", "OPCIONES" y "SALIR". Desde aquí puedes empezar a jugar o ir a los ajustes.
- **Pantalla de Juego (MainGameState):** Cuando le das a jugar, te lleva a esta pantalla, que es donde ocurre toda la acción. Para esta versión alfa no nos hemos complicado con un HUD ni nada de eso; simplemente ves a los personajes, las plataformas y los enemigos. La idea es que todo lo importante se entienda viendo lo que pasa en el nivel. Este es un nivel de prueba para mostrar la funcionalidad básica. En el Hito 2 tenemos pensado rehacer todo el sistema de niveles desde cero y separar todos los elementos por clases.
- **Menú de Pausa (PauseMenuState):** Si estás en medio de la partida y pulsas la tecla ESC, el juego se para y aparece este menú. Aquí tienes las opciones típicas: "REANUDAR" para seguir jugando, "REINICIAR" para empezar el nivel de nuevo, "OPCIONES" por si quieres cambiar algo del sonido, y "MENU PRINCIPAL" para volver al inicio.
- **Menú de Opciones (OptionsState):** Desde el menú principal o el de pausa puedes entrar aquí. De momento, hemos puesto un control para subir y bajar el volumen general del juego y un botón para cambiar el idioma, aunque eso último todavía está en desarrollo.
- **Pantalla de Game Over (GameOverState):** Esta pantalla salta cuando uno de los dos personajes muere. Te muestra un mensaje de "Game Over" y te da la opción de volver a intentarlo pulsando la tecla ESPACIO.

### Niveles

La versión Alpha cuenta con un único nivel de prueba diseñado para mostrar todas las mecánicas implementadas. Este nivel incluye:

- Una plataforma base y una plataforma elevada.
- Un charco de lava (peligro de fuego).
- Un charco de agua (peligro de agua).
- Un enemigo de fuego que patrulla en la plataforma superior.
- Un enemigo de agua que patrulla en la plataforma inferior.

### Progreso del Juego

El flujo de estados del juego es el siguiente:

### Personajes y Enemigos

- **Personaje de Fuego:**
    - **Descripción:** Representado por un rectángulo de color ROJO. Más tarde se añadirá un sprite con animaciones.
    - **Habilidades:** Inmune al fuego, puede moverse y saltar.
    - **Debilidad:** Muere al contacto con el agua.
- **Personaje de Agua:**
    - **Descripción:** Representado por un rectángulo de color AZUL. Lo mismo que el personaje de fuego, más tarde tendrá un sprite
    - **Habilidades:** Inmune al agua, puede moverse y saltar.
    - **Debilidad:** Muere al contacto con el fuego.
- **Enemigos:**
    - **Descripción:** Son cuadrados de color rojo o azul que se mueven en un patrón de patrulla simple (izquierda a derecha).

### Comportamiento

Actúan como peligros móviles, eliminando al personaje del elemento opuesto al contacto.

### Música y Efectos de Sonido

El apartado sonoro ha sido gestionado a través de una clase AudioManager para centralizar la carga y reproducción de sonidos.

- **Música:** Una pista de música de fondo (backgroundMusic.mp3) se reproduce en bucle durante toda la partida.
- **Efectos de Sonido (SFX):**
    - **boton_hover.ogg:** Sonido al pasar el ratón sobre un botón.
    - **boton_click.ogg:** Sonido al hacer clic en un botón.

### Arte y Concepts

Siguiendo los requisitos del Hito 1, el apartado gráfico se ha mantenido simple, utilizando figuras geométricas básicas de Raylib para representar a los personajes, plataformas y peligros. La diferenciación se logra mediante un código de colores intuitivo (rojo para fuego, azul para agua, gris para plataformas, etc.). El único recurso gráfico externo es el logo del juego (logo.png), que se muestra en el menú principal.

## 2. Metodología de Desarrollo

### Elección de Estrategia de Gestión de Ramas

Para la gestión del control de versiones, hemos optado por la estrategia GitHub Flow.

**Justificación:** GitHub Flow es un modelo ligero y ágil, ideal para proyectos pequeños y equipos que necesitan iterar rápidamente. La principal ventaja para nosotros es su simplicidad: todo el desarrollo se realiza en ramas de características que parten de main y se fusionan de nuevo en main una vez completadas.

Elegimos esta estrategia porque nos permitía agregar cambios de forma rápida y directa. En un equipo reducido como el nuestro, los ciclos de revisión complejos de modelos como Git Flow habrían ralentizado el desarrollo. Con GitHub Flow, cada miembro podía trabajar en una funcionalidad de forma aislada y, una vez probada, integrarla sin necesidad de revisiones exhaustivas por parte de todo el equipo, agilizando enormemente el progreso en esta fase inicial.

### Sistema de Bugtracking Elegido

Hemos utilizado GitHub Projects como nuestro sistema de seguimiento de errores y tareas.

**Justificación:** La elección de GitHub Projects fue natural, ya que se integra perfectamente con nuestro repositorio y con la estrategia de GitHub Flow. Nos permite:

- **Centralizar la información:** Todas las tareas, mejoras y bugs están en el mismo lugar que nuestro código.
- **Crear y asignar tareas (Issues):** Cada nueva funcionalidad o corrección de error se documentaba como un Issue, que se podía asignar a un miembro del equipo.
- **Visualizar el progreso:** Utilizando un tablero Kanban en GitHub Projects, podíamos ver de un vistazo el estado de cada tarea ("To Do", "In Progress", "Done").
- **Vincular código y tareas:** Podíamos vincular commits y Pull Requests directamente a los Issues, lo que dejaba una trazabilidad clara de qué cambio solucionaba qué problema o implementaba qué característica.

Esta integración nos proporcionó un flujo de trabajo muy eficiente y organizado sin necesidad de recurrir a herramientas externas.

## 3. Librerías Adicionales

Para el desarrollo de este proyecto, hemos utilizado la siguiente librería externa:

- **Raylib:** Una librería de programación de videojuegos simple y fácil de usar, escrita en C. La hemos elegido por su bajo nivel de abstracción, su excelente rendimiento y su facilidad para prototipar rápidamente, lo cual era ideal para los objetivos de este primer hito.
