#include "Game.hpp"
#include <cmath>
#include <iostream> // Para imprimir errores si la imagen no carga

Game::Game() {
    // 1. Iniciar Ventana
    window.create(sf::VideoMode(800, 600), "Ben 10: Savage Pursuit - Sprite Version");
    window.setFramerateLimit(60);

    // 2. Iniciar Física (Box2D v3)
    b2WorldDef worldDef = b2DefaultWorldDef();
    
    // Gravedad
    worldDef.gravity = {0.0f, 900.0f}; 
    
    worldId = b2CreateWorld(&worldDef);

    // 3. Crear Suelo
    b2BodyDef groundDef = b2DefaultBodyDef();
    groundDef.position = {400.0f, 580.0f};
    groundId = b2CreateBody(worldId, &groundDef);

    b2Polygon groundBox = b2MakeBox(400.0f, 10.0f);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    groundShape.setSize(sf::Vector2f(800.0f, 20.0f));
    groundShape.setOrigin(400.0f, 10.0f);
    groundShape.setFillColor(sf::Color::White);

    // 4. Crear Cuerpo Físico de Ben (La caja invisible que choca)
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {400.0f, -100.0f}; //Aseguramos que inicie en el aire y que caiga a cierta altura
    benBodyId = b2CreateBody(worldId, &bodyDef);
    b2Body_SetFixedRotation(benBodyId, true);

    b2Polygon dynamicBox = b2MakeBox(60.0f, 120.0f); // Tamaño ajustado al cuerpo de Ben
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    b2CreatePolygonShape(benBodyId, &shapeDef, &dynamicBox);

    // --- 5. CONFIGURAR GRÁFICOS DE BEN 10 (EL SPRITE) ---
    
    // A. Cargar la imagen
    sf::Image benImage;
    if (!benImage.loadFromFile("assets/images/ben10.png")) {
        std::cerr << "ERROR cargarndo imagen ben10.png" << std::endl;
    }
    //Especificar que lo que sea magenta lo ponga transparente
    benImage.createMaskFromColor(sf::Color::Magenta);
    //Cargamos la textura de la imagen ya actualizada papa
    benTexture.loadFromImage(benImage);

    benSprite.setTexture(benTexture);

    // (X,Y,Ancho,Alto) de la imagen
    benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84)); 

    // C. Centrar (El Pivote)
    // 51 entre 2 = 25.5
    // 84 entre 2 = 42
    benSprite.setOrigin(25.5f, 42.0f); 

    // D. Escalar (Zoom)
    // Lo hacemos 3 veces más grande para verlo bien.
    benSprite.setScale(3.0f, 3.0f);
}

Game::~Game() {
    b2DestroyWorld(worldId);
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void Game::update() {
    //1.Obtener la velocidad del ben
    b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);

    //2.Hacer que se mueva a la DERECHA (con tecla D o flecha Derecha)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        // CORRECCIÓN 2: VELOCIDAD
        // Antes 5.0f (lento), Ahora 200.0f (rápido)
        velocity.x = 200.0f; 
        
        // CORRECCIÓN 3: ESCALA
        // Mantenemos el 3.0f del inicio para que no se encoja
        benSprite.setScale(3.0f, 3.0f);
    }
    //3.Hacer que se mueva la la IZQUIERDA(Tacla A o Flecha Izquierda)
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        // CORRECCIÓN 4: VELOCIDAD NEGATIVA
        // Antes -0.5f (casi quieto), Ahora -200.0f (rápido)
        velocity.x = -200.0f; 
        
        // CORRECCIÓN 5: ESCALA NEGATIVA
        // Mantenemos el -3.0f para voltearlo sin encogerlo
        benSprite.setScale(-3.0f, 3.0f); 
    }
    //4.Frenar(Si no se toca nada)
    else{
        velocity.x = 0.0f;
    }
    
    // 5. SALTAR (Tecla Espacio)
    // Solo salta si la velocidad vertical es casi cero (o sea, si está tocando el suelo)
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && std::abs(velocity.y)< 0.1f){
        // CORRECCIÓN 6: FUERZA DE SALTO
        // Antes -12.0f (saltito), Ahora -550.0f (SALTAZO)
        velocity.y = -550.0f; 
    }
    
    //6.Aplicar la Fisica
    b2Body_SetLinearVelocity(benBodyId, velocity);

    //Avanzar la simulación
    b2World_Step(worldId, 1.0f / 60.0f, 4);

    //7.Sincronizar el dibujo
    b2Vec2 pos = b2Body_GetPosition(benBodyId);
    benSprite.setPosition(pos.x, pos.y);

    //Sincronizar tambien el suelo
    b2Vec2 groundPos = b2Body_GetPosition(groundId);
    groundShape.setPosition(groundPos.x, groundPos.y);
}

void Game::render(){
    window.clear(sf::Color::Black); // Limpiar pantalla
    window.draw(groundShape);       // Dibujar suelo
    window.draw(benSprite);         // Dibujar Ben
    window.display();               // Mostrar todo
} 