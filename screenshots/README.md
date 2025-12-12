# Ken 11: Quest for the OmniClock

Un emocionante juego de acción y plataformas en 2D donde el destino del tiempo está en tus manos.

## Descripción del Proyecto

"Ken 11: Quest for the OmniClock" es un videojuego de aventura desarrollado en C++ donde el jugador controla a Ken, un joven héroe con habilidades extraordinarias. El jugador debe navegar por un nivel lleno de plataformas traicioneras, abismos y drones enemigos utilizando su capacidad para transformarse en una entidad de fuego. El proyecto implementa física realista, detección de colisiones y gestión de estados.

## LORE DEL JUEGO
Ken se enfrenta a su archienmigo BILLVAX el cual lo rapto para obtener los poderes del Omniclock, lo cual lo consigue, afortunadamente Ken pudo escapar de las garras de BILLVAX pero con un problema las transformaciones que existian dentro del Omnclock escaparon y comenzando con el ente que lo comenzo todo(LUMBRE) tiene que recolectar a los demas entes, antes que BILLVAX los encuentre primero esquivando a los secuaces de este mismo

## 🎯 Objetivo del Juego

El objetivo principal es atravesar todo el escenario, sobrevivir al ataque de los drones patrulleros y llegar a la zona final para recolectar una parte del **OmniClock** para recolectar a un nuevo ente, evitando caer al vacío o perder toda la barra de vida.

## 🎮 Controles

* **A o (<)/ Izquierda**: Moverse a la izquierda.
* **D o (>) / Derecha**: Moverse a la derecha.
* **ESPACIO**: Saltar (Permite alcanzar plataformas altas).
* **Z**: Transformarse (Alterna entre Ken humano y Forma de Fuego).
* **X**: Disparar bolas de fuego (Habilidad exclusiva de la transformación).
* **R**: Reiniciar el nivel (Disponible en pantalla de Game Over o Victoria).
* **Enter**: Iniciar juego desde el menú.

## ⚙️ Mecánicas

* **Sistema de Transformación**: El jugador puede cambiar de forma en tiempo real. La forma de Fuego es más grande y puede disparar, pero consume energía constantemente.
* **Gestión de Energía**: La energía se agota al estar transformado. Si llega a cero, el jugador vuelve a su forma humana automáticamente. La forma humana regenera energía.
* **Física Box2D**: Implementación de gravedad, fricción y colisiones precisas para el movimiento del personaje y los proyectiles.
* **Sistema de Combate**: Disparo de proyectiles con cooldown y detección de impacto contra enemigos.
* **IA de Enemigos**: Drones que patrullan áreas definidas y cambian de dirección al llegar a sus límites.

## 🏆 Características

* Gráficos 2D estilo Pixel Art (Assets libres de copyright).
* Cámara de seguimiento suave (Side-scrolling).
* Efectos de sonido para salto, disparo, transformación y daño.
* Interfaz de usuario (HUD) con barras de vida y energía dinámicas.
* Menú principal, pantalla de instrucciones y estados de victoria/derrota.

## 👥 Equipo

* **Líder**: Caleb Emiliano González Arias (@Caleb200511456-github)
* **Integrante**: Diego Alejandro Maciel Lomeli (@AlejandroLomel-github)

## 🛠️ Tecnologías

* **Lenguaje**: C++ (Standard 17)
* **Librería Gráfica/Audio**: SFML 2.6.1
* **Motor de Física**: Box2D
* **Entorno de Desarrollo**: Visual Studio Code (MinGW64)

## 📜 Créditos

* **Sprites**: Personajes y tilesets basados en recursos libres (Kenney Assets / OpenGameArt), modificados para el proyecto.
* **Música y SFX**: Efectos de sonido y música de fondo Royalty Free (Pixabay).
* **Imagenes**: Obtenidas de la IA Microsoft Bing para evitar CopyRight
* **Agradecimientos**: A la comunidad de SFML por la documentación y al profesor Ramon de Proyecto 252 por la guía en la estructura del motor.