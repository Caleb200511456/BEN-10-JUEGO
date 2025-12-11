#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include <box2d/box2d.h>
#include <vector> 

struct Projectile{
    sf::CircleShape shape; //La forma(bola naranja)
    float speed;
    bool destroy;
    float lifetime; 
}; 

class Game {
public:
//Constructor y Destructor
    Game();
    ~Game();
    void run();

private:
//Funciones internas
    void processEvents();
    void update();
    void render();

    void createPlatform(float x, float y, float width, float height);

    sf::RenderWindow window;
    b2WorldId worldId;
    
    //BEN 10
    b2BodyId benBodyId;
    sf::Texture benTexture; // La imagen en memoria
    sf::Sprite benSprite;   // El dibujo en pantalla
    
    //Variables del OMNITRIX
    //Comenzamos con fuego(heatblast)
    sf::Texture heatblastTexture; //imagen de fuego en memoria
    bool isHeatblast; //Para ver si esta transformado

    //Animación
    float animationTimer; //Cronometro para cambiar el dibujo
    int currentFrame; //En que dibujo vamos del 1 al 6 
    
    //Suelo
    std::vector<sf::RectangleShape> platformShapes; //Para aplicar dibujos para el escenario
   
    //Variables del enemigo
    struct Enemy {
        b2BodyId bodyId;       // Su cuerpo físico
        sf::RectangleShape shape; // Su dibujo
        float speed;           // Su velocidad actual
        float startX;          // Donde nació (para saber patrullar alrededor)
        float patrolRange;     // Qué tan lejos puede caminar
        bool destroy;          // Si está muerto
    };

    std::vector<Enemy> enemies; // ¡La lista de todos los drones!

    // Función para crear enemigos fácilmente
    void spawnEnemy(float x, float y, float range);
     
    //META DEL NIVEL 
    sf::CircleShape goalShape; //Esfera de energia 

    //SISTEMA DE COMBATE
    std::vector<Projectile> projectiles; //Lista que crece y se encoge

    //Reloj del sistema
    sf::Clock dtClock; //Cronometro
    float shootCooldown; //Tiempo de espera entre disparos
    int maxHealth; //Vida maxima
    int currentHealth;//Vida actual
    float damageTimer; //Tiempo de invencibilidad
    sf::RectangleShape healthBar; //La barra verde
    sf::RectangleShape healthBarBack; //Fondo gris/rojo de la barra

    //Energia del omnitrix
    float maxEnergy; //Energia maxima 
    float currentEnergy; //Energia Actual
    sf::RectangleShape energyBar; //La barra amarilla
    sf::RectangleShape energyBarBack; //El fondo de la barra

    //Sistema de audio
    sf::Music bgMusic; //Musica larga
    //Efectos cortos
    sf::SoundBuffer jumpBuffer;
    sf::Sound jumpSound;

    sf::SoundBuffer shootBuffer;
    sf::Sound shootSound;

    sf::SoundBuffer transformBuffer;
    sf::Sound transformSound;

    sf::SoundBuffer hitBuffer;
    sf::Sound hitSound; //Para cuando me peguen 

};
