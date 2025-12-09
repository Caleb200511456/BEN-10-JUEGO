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
    worldDef.gravity = {0.0f, 500.0f}; 
    worldId = b2CreateWorld(&worldDef);

    // 3. Diseñamos el nivel
    //3.1. El suelo Principal (Que sea Grande y este por debajo)
    createPlatform(400.0f, 580.0f, 800.0f, 40.0f);

    //3.2.Plataformas flotantes(Para hacer PARKOUUUR :D)
    createPlatform(600.0f, 450.0f, 200.0f, 20.0f); //Derecha baja
    createPlatform(200.0f, 350.0f, 200.0f, 20.0f); //Izquierda al medio
    createPlatform(500.0f, 200.0f, 150.0f, 20.0f); //Arriba centro

    //3.3.Muro alto a la izquierda para que no se salga el ben
    //createPlatform(-10.0f, 300.0f, 20.0f, 600.0f);
    //3.4.Un muro a la derecha
    //createPlatform(810.0f, 300.0f, 20.0f, 600.0f);
    

    // 4. Crear Cuerpo Físico de Ben (La caja invisible que choca)
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {400.0f, -100.0f}; //Aseguramos que inicie en el aire y que caiga a cierta altura
    benBodyId = b2CreateBody(worldId, &bodyDef);
    b2Body_SetFixedRotation(benBodyId, true);

    b2Polygon dynamicBox = b2MakeBox(30.0f, 60.0f); // Tamaño ajustado al cuerpo de Ben
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    b2CreatePolygonShape(benBodyId, &shapeDef, &dynamicBox);

    // --- 5. CONFIGURAR GRÁFICOS DE BEN 10 (EL SPRITE) ---
    
    // A. Cargar la imagen
    sf::Image benImage;
    if (!benImage.loadFromFile("assets/images/ben10.png")) {
        std::cerr << "ERROR cargando imagen ben10.png" << std::endl;
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
    benSprite.setScale(1.5f, 1.5f);
    animationTimer = 0.0f;
    currentFrame = 0; 

    //5.1.Cargar Graficos de FUEGO
    //5.1.1.Cargar la imagen de fuego 
    sf::Image heatblastImage;
    if(!heatblastImage.loadFromFile("assets/images/Fuego.png")){
        std::cerr << "ERROR cargando imagen Fuego.png" << std::endl;
    }
    //5.1.2.Quitar el fondo Magenta 
    heatblastImage.createMaskFromColor(sf::Color(255, 0, 255));
    //5.1.3. Pasar la imagen limpia a la textura
    heatblastTexture.loadFromImage(heatblastImage);
    //5.1.4.Inicializar estado(Empezar sin transformacion)
    isHeatblast = false;
    //6.Crear el enemigo(Dron)
    b2BodyDef enemyDef = b2DefaultBodyDef();
    enemyDef.type = b2_kinematicBody;//Hacer que se mueva pero nada lo empuja
    enemyDef.position = {600.0f, 400.0f};
    //Bloqueamos rotación para que no ruede
    enemyDef.fixedRotation = true;
    enemyBodyId = b2CreateBody(worldId, &enemyDef);

    //Cramos forma fisica de la caja
    b2Polygon enemyBox = b2MakeBox(15.0f, 15.0f); 
    b2ShapeDef enemyShapeDef = b2DefaultShapeDef();
    enemyShapeDef.density = 1.0f;
    b2CreatePolygonShape(enemyBodyId, &enemyShapeDef, &enemyBox);

    //Forma Gráfica (Cubito rojo)
    enemyShape.setSize(sf::Vector2f(30.0f, 30.0f));
    enemyShape.setOrigin(15.0f, 15.0f);
    enemyShape.setFillColor(sf::Color::Red); 

    //Velocidad Inicial
    enemySpeed = 100.0f; //Se mueva a la derecha

    //7.Crear la META(TARJETA SUMO)
    goalShape.setRadius(20.0f); //Tamaño de la esfera
    goalShape.setFillColor(sf::Color::Yellow);//Amarillo para la tarjeta de Sumo
    goalShape.setOutlineThickness(2.0f);
    goalShape.setOutlineColor(sf::Color::White);

    //Centrar el punto de origen
    goalShape.setOrigin(20.0f, 20.0f);

    //Posicion: En la plataforma floatante del centro-arriba
    goalShape.setPosition(500.0f,150.0f);

    shootCooldown = 0.0f; //Listo para disparar

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
        
        if(event.type == sf::Event::KeyPressed){
            if(event.key.code == sf::Keyboard::Space){
                //Obtenemos aqui la velocidad solo para revisar si esta ben en el suelo
                b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);

                //Si la velocidad vertical es baja (esta en el suelo o casi)
                if (std::abs(velocity.y)<0.5){
                    velocity.y = -900.0f; //Impulso del salto
                    b2Body_SetLinearVelocity(benBodyId, velocity);
                }
            }

                //Transformacion (Nuevo)
                if(event.key.code == sf::Keyboard::Z){
                    //Cambiamos el estado(Si era falso, ahora verdadero y viceversa)
                    isHeatblast = !isHeatblast;
                    
                    if(isHeatblast){
                        std::cout << "¡FUEGO!" << std::endl;
                        //1.Cambiar la imagen a la de fuego
                        benSprite.setTexture(heatblastTexture);
                        //2.Usar coordenadas de la imagen de fuego (X=9, Y=11, Ancho=35, Alto=53)
                        benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
                        //3.Ajustar el centro (Mitad de 35 y 53)
                        benSprite.setScale(2.2f, 2.2f);
                        benSprite.setOrigin(17.5f, 26.5f);
                    }else{
                        //Volver a Ben
                        std::cout << "Destransformado..." << std::endl;
                        //1.Volver a la imagen de Ben
                        benSprite.setTexture(benTexture);
                        //2.Coordenadas originales de Ben quieto
                        benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
                        //3.Centro original de Ben
                        benSprite.setOrigin(25.5f, 42.0f);
                        //Regresar la escala de Ben a la normal
                        benSprite.setScale(1.5f, 1.5f);
                    }
                }
                //DISPARO
                if(event.key.code == sf::Keyboard::X && isHeatblast && shootCooldown <= 0.0f){
                    Projectile newProj;
                
                //1.Apariencia
                newProj.shape.setRadius(10.0f);
                newProj.shape.setFillColor(sf::Color(255, 165, 0)); //Naranja de fuego
                newProj.shape.setOrigin(10.0f, 10.0f); //Ajuste de centro
                //2.Posicion de salida(Desde el centro de fuego)
                sf::Vector2f benPos = benSprite.getPosition();
                //Ajustamos un poco la altura para que salga de las manos/pecho
                newProj.shape.setPosition(benPos.x, benPos.y - 15.0f);
                //3.Direccion
                if(benSprite.getScale().x > 0){
                    newProj.speed = 600.0f; //Disparo a la derecha
                }else{
                    newProj.speed = -600.0f; //Disparo a la izquiersa
                }
                newProj.destroy = false;
                newProj.lifetime = 0.0f; 
                //4.Agregar al cargador
                projectiles.push_back(newProj);
                //Recalentar el arma
                shootCooldown = 0.4f; //Esperar 0.4 seg para el siguiente tiro
                std::cout << "¡Fiuuu! (Disparo)" << std::endl;
                }
            }
        }
    }

void Game::update() {
    // Esto mide el tiempo real entre "fotos". Arregla la velocidad loca.
    float dt = dtClock.restart().asSeconds(); 

    // 1. Obtener la velocidad física de ben 
    b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);
    bool isMoving = false; // Aplicamos bandera para saber si corremos
    
    // Definimos que tamaño debemos tener
    float currentScale = 1.5f; // Tamaño normal de ben
    if(isHeatblast){
        currentScale = 2.2f; // Tamaño para fuego grande
    }

    // 2. Controles
    // Vamo a la derecha
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        velocity.x = 200.0f; // Asi aplicamos que se mueva la derecha en el eje x
        benSprite.setScale(currentScale, currentScale);
        isMoving = true; // Aqui aplica que ben se esta moviendo
    }
    // Vamo a la Izquierda
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        velocity.x = -200.0f; // Aplicamos que se mueva a la izquierda en el eje x
        benSprite.setScale(-currentScale, currentScale);
        isMoving = true; // Se esta moviendo a la izquierda :D
    }
    // QUIETOO
    else{
        velocity.x = 0.0f;
        isMoving = false;
    }

    // 3. Sistema de Animación
    // Si somos Fuego, No hacemos nada en este, se queda con la pose que pusimos al presionar Z)
    if(isHeatblast){
        if(isMoving){
            // A. Cronometro
            // CAMBIO: Usamos 'dt' en vez de 1.0/60.0f para suavidad
            animationTimer += dt; 
            
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f;
                currentFrame++;
                if(currentFrame >= 6) currentFrame = 0;
            }
            // B. Listas Maestras
            int fuegoX[] = {16, 67, 106, 155, 210, 250};
            int fuegoW[] = {25, 20, 34, 32, 21, 36};
            
            // C. Obtener cuadro actual 
            int currentX = fuegoX[currentFrame];
            int currentW = fuegoW[currentFrame];
            
            // D. Aplicar recorte (Con Y=82)
            benSprite.setTextureRect(sf::IntRect(currentX, 82, currentW, 55));
            
            // E. Centrado Dinamico
            benSprite.setOrigin(currentW / 2.0f, 26.5f);
        }
        else{
            // QUIETO
            benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
            benSprite.setOrigin(17.5f, 26.5f);
        }
    }
    else{ 
        // Si somos BEN
        if(isMoving){
            // 3.1. Avanzar el cronómetro (Se suma el tiempo real dt)
            // CAMBIO: Usamos 'dt' aquí también
            animationTimer += dt; 

            // 3.2. Aplicar el cambio de dibujo
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f; // Reiniciamos el cronometro
                currentFrame++; // Siguiente dibujo
            
                // Si llegamos al dibujo 6, regresar al primer dibujo(Loop)
                if(currentFrame >= 6){
                    currentFrame = 0;
                }
            }
            // 3.3. Calcular el recorte de correr obtenidos en la imagen
            // Lista exacta de tus coordenadas X:
            int walkingFramesX[] = {17, 78, 137, 181, 238, 295};

            // Usamos el 'currentFrame' para elegir el número de la lista
            int frameX = walkingFramesX[currentFrame]; 

            // Actualizamos el recorte usando la X exacta de la lista
            // (Y=382, Ancho=47, Alto=79 según tus medidas anteriores)
            benSprite.setTextureRect(sf::IntRect(frameX, 382, 47, 79));

            // Ajustamos el origen para el ancho de 47
            benSprite.setOrigin(23.5f, 39.5f);
        }
        else{
            // 3.4. Si esta quieto volver al dibujo original 
            // Datos del dibujo original: X=8, Y=23, Alto= 51, Largo=84 aprox
            currentFrame = 0; // Resetear para que al correr empiece en el dibujo 1
            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
            benSprite.setOrigin(25.5f, 42.0f);
        }
    }

    // 4. Aplicar las fisicas y sincronizamos
    b2Body_SetLinearVelocity(benBodyId, velocity);
    // Fisica se queda fija en 1/60 para que Box2D no se rompa
    b2World_Step(worldId, 1.0f / 60.0f, 4);

    b2Vec2 pos = b2Body_GetPosition(benBodyId);
    benSprite.setPosition(pos.x,pos.y);

    // Hacemos la zona de Muerte GAME OVER
    // Si ben cae muy abajo (Y mayor a 800 pixeles)
    if (pos.y > 800.0f) {
        // 1. Lo regresamos al inicio (400, -100)
        // El ángulo lo dejamos en 0 (b2Rot_Identity si usas v3, o simplemente 0.0f en la función SetTransform)
        b2Body_SetTransform(benBodyId, {400.0f, -100.0f}, {1.0f, 0.0});// 1.0 es Seno y 0.0 es Coseno asi se movera Ben
        
        // 2. Le quitamos la velocidad para que no siga cayendo a toda pastilla
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    }

    // IA DEL ENEMIGO 
    
    // 1. Obtener posición actual
    b2Vec2 enemyPos = b2Body_GetPosition(enemyBodyId);
    enemyShape.setPosition(enemyPos.x, enemyPos.y);

    // 2. Lógica de Patrulla
    // Si llegó al borde DERECHO (680) Y todavía quiere ir a la derecha (> 0)
    if (enemyPos.x > 680.0f && enemySpeed > 0) {
        enemySpeed = -100.0f; // ¡Cambio a Izquierda!
    }
    // Si llegó al borde IZQUIERDO (520) Y todavía quiere ir a la izquierda (< 0)
    else if (enemyPos.x < 520.0f && enemySpeed < 0) {
        enemySpeed = 100.0f;  // ¡Cambio a Derecha!
    }

    // 3. Aplicar la velocidad decidida
    b2Vec2 enemyVel = b2Body_GetLinearVelocity(enemyBodyId);
    enemyVel.x = enemySpeed; 
    b2Body_SetLinearVelocity(enemyBodyId, enemyVel);

    // Colision(hacer que ben muera al tocar el dron)
    // Preguntamos: ¿El rctangulo de Ben se cruza con el del Enemigo?
    if(benSprite.getGlobalBounds().intersects(enemyShape.getGlobalBounds())){
    
        // GAME OVER PARA BEN
        // 1.Teletransportar al inicio (400, -100) de pie (1.0, 0.0) significa (seno,coseno)
        b2Body_SetTransform(benBodyId, {400.0f, -100.0f}, {1.0, 0.0f});

        // 2.Quitamos velocidad(franar)
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});

        std::cout << "¡Ben ha sido capturado!" << std::endl;
    }

    // VICTORIA: CUANDO BEN TOCA LA META
    if(benSprite.getGlobalBounds().intersects(goalShape.getGlobalBounds())){
        // GANASTEEE
        std::cout << "¡NIVEL COMPLTADO! ERES UN HEROE." << std::endl;

        // PReiniciamos el nivel para vovler a jugar 
        // Proximamente agraremos nivel 2 (por el momento se queda asi)
        b2Body_SetTransform(benBodyId, {400.0f, -100.0f}, {1.0, 0.0f});
        b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    }
    if (shootCooldown > 0.0f){
        shootCooldown -= dt; //Restamos el tiempo que pasó
    }
    //2.Actualización de balas
    for(size_t i = 0; i < projectiles.size(); i++){
        
        // 1. Mover USANDO dt (Tiempo real)
        // Esto evita que salgan como líneas
        projectiles[i].shape.move(projectiles[i].speed * dt, 0.0f); 
        //Envejecer la bala
        projectiles[i].lifetime += dt;
        //¿Murio bala vieja?
        if (projectiles[i].lifetime > 2.0f){
            projectiles[i].destroy = true;
        }
        //Colision de bala vs dron
        //Si la bala toca al enemigo:
        if(projectiles[i].shape.getGlobalBounds().intersects(enemyShape.getGlobalBounds())){
            projectiles[i].destroy = true; //la bala explota
        //Matar al enemigo 
        b2Body_SetTransform(enemyBodyId, {-1000.0f, -1000.0f}, {1.0f, 0.0f});
        enemySpeed = 0; //Para que ya no se mueva
        
        std::cout << "¡ENEMIGO ELIMINADO!" << std::endl;
        }
    }
    // BORRAR BALAS VIEJAS
    auto iterator = std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p){ return p.destroy;});
    projectiles.erase(iterator, projectiles.end());
}

void Game::render(){
    window.clear(sf::Color::Black); // 1. Limpiar pantalla

    // --- CÁMARA ---
    sf::Vector2f benPos = benSprite.getPosition();
    sf::View camera(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f, 600.0f));
    camera.setCenter(benPos);
    window.setView(camera);

    // --- DIBUJAR TODO ---

    // 1. Plataformas
    for (const auto& shape : platformShapes) {
        window.draw(shape);
    }
    
    // 2. Objetos
    window.draw(goalShape); 
    window.draw(enemyShape);
    
    // 3. Balas (¡AQUÍ ES SU LUGAR!)
    for (const auto& proj : projectiles){
        window.draw(proj.shape);
    }

    // 4. Ben (Lo dibujamos al final para que salga encima de todo)
    window.draw(benSprite);

    // --- MOSTRAR PANTALLA (SOLO UNA VEZ AL FINAL) ---
    window.display(); 
}

//Aplicacion de las plataformas
void Game::createPlatform(float x, float y, float width, float height){
//1.Fisicas (Box2D)
b2BodyDef bodyDef = b2DefaultBodyDef();
bodyDef.position = {x, y}; //Box2D aplicamos para usar el centro
b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

//Box2D pide la mitad del ancho y alto
b2Polygon box = b2MakeBox(width / 2.0f, height / 2.0f);
b2ShapeDef shapeDef = b2DefaultShapeDef();
b2CreatePolygonShape(bodyId, &shapeDef, &box);

//2.SFML (Para aplicar los graficos)
sf::RectangleShape shape(sf::Vector2f(width, height));
shape.setOrigin(width / 2.0f, height / 2.0); //Origen al centro igual que Box2D
shape.setPosition(x, y);

//Aplicamos Color 
shape.setFillColor(sf::Color(0, 180, 0)); //Verde Oscuro
shape.setOutlineThickness(2.0f);
shape.setOutlineColor(sf::Color::White);

//3.Guardamos en la lista para despues dibujarlo
platformShapes.push_back(shape);
}
