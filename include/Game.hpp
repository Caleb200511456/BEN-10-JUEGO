#pragma once
#include <SFML/Graphics.hpp>
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
    b2BodyId enemyBodyId; //El cuerpo fisico del enemigo
    
    //Enemigo
    sf::RectangleShape enemyShape; //El dibujo del enemigo(Cubo rojo)
    float enemySpeed; //Velocidad del enemigo

    //META DEL NIVEL 
    sf::CircleShape goalShape; //Esfera de energia 

    //SISTEMA DE COMBATE
    std::vector<Projectile> projectiles; //Lista que crece y se encoge

    //Reloj del sistema
    sf::Clock dtClock; //Cronometro
    float shootCooldown; //Tiempo de espera entre disparos
};
