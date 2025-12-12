#include "Game.hpp"
#include <cmath>
#include <iostream> 

Game::Game() {
    // 1. Iniciar Ventana
    window.create(sf::VideoMode(800, 600), "KEN 11: QUEST FOR THE OMNICLOCK");
    window.setFramerateLimit(60); 

    // Estado inicial
    gameState = MENU; 

    // --- 2. Iniciar Física ---
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = {0.0f, 500.0f}; 
    worldId = b2CreateWorld(&worldDef);

    // --- 3. DISEÑO DE NIVEL ---
    // Zona 1. Inicio
    createPlatform(500.0f, 580.0f, 1000.0f, 40.0f);
    // Abismo
    createPlatform(1200.0f, 450.0f, 150.0f, 20.0f); 
    // Zona 2
    createPlatform(2000.0f, 580.0f, 1200.0f, 40.0f);
    // Escalada
    createPlatform(2600.0f, 480.0f, 200.0f, 20.0f); 
    createPlatform(2900.0f, 360.0f, 200.0f, 20.0f); 
    // Zona 3 (Alta)
    createPlatform(4000.0f, 240.0f, 2000.0f, 40.0f);
    // Extras
    createPlatform(600.0f, 350.0f, 200.0f, 20.0f); 
    createPlatform(1800.0f, 300.0f, 200.0f, 20.0f);

    // --- 4. CREAR CUERPO FISICO DE BEN ---
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {100.0f, 450.0f}; 
    benBodyId = b2CreateBody(worldId, &bodyDef);
    b2Body_SetFixedRotation(benBodyId, true);

    b2Polygon dynamicBox = b2MakeBox(30.0f, 60.0f); 
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    b2CreatePolygonShape(benBodyId, &shapeDef, &dynamicBox);

    // --- 5. CONFIGURAR GRÁFICOS DE BEN ---
    sf::Image benImage;
    if (!benImage.loadFromFile("assets/images/ben10.png")) {
        std::cerr << "ERROR cargando imagen ben10.png" << std::endl;
    }
    benImage.createMaskFromColor(sf::Color::Magenta);
    benTexture.loadFromImage(benImage);
    benSprite.setTexture(benTexture);
    benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84)); 
    benSprite.setOrigin(25.5f, 42.0f); 
    benSprite.setScale(1.5f, 1.5f);
    animationTimer = 0.0f;
    currentFrame = 0; 

    // 5.1. FUEGO
    sf::Image heatblastImage;
    if(!heatblastImage.loadFromFile("assets/images/Fuego.png")){
        std::cerr << "ERROR cargando imagen Fuego.png" << std::endl;
    }
    heatblastImage.createMaskFromColor(sf::Color(255, 0, 255));
    heatblastTexture.loadFromImage(heatblastImage);
    isHeatblast = false;
    
    // --- 6. EJERCITO DE DRONES
    // Zona 1 (Suelo 580 -> Drone 500)
    spawnEnemy(700.0f, 500.0f, 150.0f);
    // Zona 2 (Suelo 580 -> Drone 500)
    spawnEnemy(1600.0f, 500.0f, 100.0f);
    spawnEnemy(2200.0f, 500.0f, 100.0f);
    // Zona 3 (Suelo 240 -> Drone 160)
    spawnEnemy(3500.0f, 160.0f, 200.0f); 
    spawnEnemy(4000.0f, 160.0f, 200.0f); 
    // Guardianes (Suelo 240 -> Drone 160)
    spawnEnemy(4600.0f, 160.0f, 100.0f);
    spawnEnemy(4800.0f, 160.0f, 100.0f);

    // --- 7. Crear la META ---

    // 7.1. Configuración de la caja de colisión (Hitbox invisible)
    // Usamos un radio pequeño para la colisión
    goalShape.setRadius(20.0f);
    goalShape.setOrigin(20.0f, 20.0f); // El origen del círculo es su propio centro
    goalShape.setPosition(4800.0f, 190.0f); 
    // Opcional: Para ver dónde está la hitbox mientras pruebas, descomenta esto:
    // goalShape.setFillColor(sf::Color::Blue); 

    // 7.2. Configuración del gráfico (Sprite visible)
    if(!goalTexture.loadFromFile("assets/images/goal.png")){
        std::cerr << "ERROR: No se cargo goal.png" << std::endl;
    }
    // Cargar textura desde la imagen modificada
    goalSprite.setTexture(goalTexture);

    // CENTRAR EL SPRITE (IMPORTANTE: Aplicar a goalSprite, no a goalShape)
    sf::Vector2u gSize = goalTexture.getSize();
    goalSprite.setOrigin(gSize.x / 2.0f, gSize.y / 2.0f);

    // Poner el Sprite exactamente donde está la Hitbox
    goalSprite.setPosition(goalShape.getPosition());

    // AJUSTE DE TAMAÑO
    goalSprite.setScale(0.08f, 0.08f);

    shootCooldown = 0.0f; 

    // --- 8.CONFIGURAR VIDA ---
    maxHealth = 100;
    currentHealth = 100; 
    damageTimer = 0.0f; 
    healthBarBack.setSize(sf::Vector2f(200.0f, 20.0f)); 
    healthBarBack.setFillColor(sf::Color(50, 50, 50)); 
    healthBarBack.setPosition(20.0f, 20.0f); 
    healthBar.setSize(sf::Vector2f(200.0f, 20.0f));
    healthBar.setFillColor(sf::Color(0, 255, 0)); 
    healthBar.setPosition(20.0f, 20.0f); 

    // --- 9.CONFIGURAR ENERGÍA ---
    maxEnergy = 100.0f; 
    currentEnergy = 100.0f; 
    energyBarBack.setSize(sf::Vector2f(200.0f, 10.0f)); 
    energyBarBack.setFillColor(sf::Color(50, 50, 50));
    energyBarBack.setPosition(20.0f, 45.0f); 
    energyBar.setSize(sf::Vector2f(200.0f, 10.0f));
    energyBar.setFillColor(sf::Color::Yellow); 
    energyBar.setPosition(20.0f, 45.0f);
    
    //--- 10.Cargar audio ---
    if(bgMusic.openFromFile("assets/music/music.mp3")){
        bgMusic.setLoop(true); bgMusic.setVolume(30); bgMusic.play(); 
    }
    if(jumpBuffer.loadFromFile("assets/music/jump.wav")){
        jumpSound.setBuffer(jumpBuffer); jumpSound.setVolume(50);
    }
    if(shootBuffer.loadFromFile("assets/music/shoot.wav")){
        shootSound.setBuffer(shootBuffer); shootSound.setVolume(60);
    }
    if(transformBuffer.loadFromFile("assets/music/transform.wav")){
        transformSound.setBuffer(transformBuffer); transformSound.setVolume(70);
    }
    if(hitBuffer.loadFromFile("assets/music/hit.wav")){
        hitSound.setBuffer(hitBuffer); hitSound.setVolume(50);
    }

    // --- 11.Configurar texto ---
    if (!font.loadFromFile("assets/fonts/font.ttf")) { 
        std::cerr << "ERROR: No se encontro font.ttf" << std::endl;
    }
    //11.1.Titulo
    titleText.setFont(font);
    titleText.setString("KEN 11: QUEST FOR THE OMNICLOCK");
    titleText.setCharacterSize(30);
    titleText.setFillColor(sf::Color(56, 227, 56));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(140, 200);
    //11.2.Start
    startText.setFont(font);
    startText.setString("PRESS ENTER TO START");
    startText.setCharacterSize(30);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(200, 300);
    //11.3 Instrucciones
    instructionsText.setFont(font);
    //Usamos \n para salto de linea 
    instructionsText.setString(
        "CONTROLES:\n\n"
        "A o (<) -> Izquierda\n"
        "D o (>)-> Derecha\n"
        "SPACE -> Saltar\n"
        "Z -> Transformar (Fuego)\n"
        "X -> Disparar\n\n"
        "PRESS ENTER TO PLAY"
    );
    instructionsText.setCharacterSize(25);
    instructionsText.setFillColor(sf::Color::Yellow);
    instructionsText.setPosition(200,100);
    //11.4. GAME OVER
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(250, 200);
    //11.5.Restart
    restartText.setFont(font);
    restartText.setString("PRESS 'R' TO RESTART");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(250, 350);
    //12.Cargar fondo del Menú
    if(!menuTexture.loadFromFile("assets/images/menu_bg.png")){
        std::cerr << "ERROR: No se pudo cargar menu_bg.png" << std::endl;
    //Crear imagen de respaldo por si falla la carga
    }else { 
        menuSprite.setTexture(menuTexture);
        //Escalar la imagen para que llene la ventana 
        sf::Vector2u textureSize = menuTexture.getSize();
        menuSprite.setScale(800.0f / textureSize.x, 600.0f / textureSize.y);
        //Oscurecer el fondo para que el texto resalte 
        menuSprite.setColor(sf::Color(100, 100, 100, 255));
    }
    //13.Cargar fondo del Nivel
    if(!levelTexture.loadFromFile("assets/images/Level_bg.png")){
        std::cerr << "ERROR: No se pudo cargar level_bg.png" << std::endl;
    }else{
        levelSprite.setTexture(levelTexture);
        //Ajuste de la escala para que cubra la ventana entera
        sf::Vector2u size = levelTexture.getSize();
        levelSprite.setScale(800.0f / size.x, 600.0f / size.y);
    }
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
        if (event.type == sf::Event::Closed) window.close();
        
        // LOGICA DEL MENÚ PRINCIPAL
        if (gameState == MENU) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                gameState = INSTRUCTIONS; 
            }
        }
        //LOGICA DE INSTRUCCIONES
        else if (gameState == INSTRUCTIONS){
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter){
                gameState = PLAYING;
            }  
        }
        // LOGICA DE GAME OVER
        else if (gameState == GAME_OVER || gameState == VICTORY) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                resetLevel(); 
            }
        }
        // --- LOGICA DE JUEGO ---
        else if (gameState == PLAYING) {
            if(event.type == sf::Event::KeyPressed){
                
                // Salto
                if(event.key.code == sf::Keyboard::Space){
                    b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);
                    if (std::abs(velocity.y)<0.5){
                        velocity.y = -900.0f; 
                        b2Body_SetLinearVelocity(benBodyId, velocity);
                        jumpSound.play();
                    }
                }

                // Transformación
                if(event.key.code == sf::Keyboard::Z){
                    if (!isHeatblast && currentEnergy < 10.0f) { /* Sin energia */ }
                    else {
                        isHeatblast = !isHeatblast;
                        transformSound.play();
                        if(isHeatblast){
                            benSprite.setTexture(heatblastTexture);
                            benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
                            benSprite.setScale(2.2f, 2.2f);
                            benSprite.setOrigin(17.5f, 26.5f);
                        }else{
                            benSprite.setTexture(benTexture);
                            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
                            benSprite.setOrigin(25.5f, 42.0f);
                            benSprite.setScale(1.5f, 1.5f);
                        }
                    }
                }
                
                // Disparo
                if(event.key.code == sf::Keyboard::X && isHeatblast && shootCooldown <= 0.0f){
                    shootSound.play();
                    Projectile newProj;
                    newProj.shape.setRadius(10.0f);
                    newProj.shape.setFillColor(sf::Color(255, 165, 0)); 
                    newProj.shape.setOrigin(10.0f, 10.0f); 
                    sf::Vector2f benPos = benSprite.getPosition();
                    newProj.shape.setPosition(benPos.x, benPos.y - 15.0f);
                    if(benSprite.getScale().x > 0) newProj.speed = 600.0f;
                    else newProj.speed = -600.0f; 
                    newProj.destroy = false;
                    newProj.lifetime = 0.0f; 
                    projectiles.push_back(newProj);
                    shootCooldown = 0.4f; 
                }
                
                // Truco K bajar vida automaticamente (forzar game over)
                if(event.key.code == sf::Keyboard::K){
                    currentHealth -= 10;
                    if(currentHealth < 0) currentHealth = 0;
                    float percentage = (float)currentHealth / (float)maxHealth;
                    healthBar.setSize(sf::Vector2f(200.0f * percentage, 20.0f));
                }
            }
        }
    }
}

void Game::update() {
    float dt = dtClock.restart().asSeconds(); 

    if (gameState != PLAYING) return;

    // 1. Física Ben 
    b2Vec2 velocity = b2Body_GetLinearVelocity(benBodyId);
    bool isMoving = false; 
    float currentScale = 1.5f; 
    if(isHeatblast) currentScale = 2.2f; 

    // 2. Controles
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        velocity.x = 200.0f; 
        benSprite.setScale(currentScale, currentScale);
        isMoving = true; 
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        velocity.x = -200.0f; 
        benSprite.setScale(-currentScale, currentScale);
        isMoving = true; 
    }
    else{
        velocity.x = 0.0f;
        isMoving = false;
    }

    // Energía
    if (isHeatblast) {
        currentEnergy -= 15.0f * dt;
        if (currentEnergy <= 0.0f) {
            currentEnergy = 0.0f;
            isHeatblast = false; 
            benSprite.setTexture(benTexture);
            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
            benSprite.setOrigin(25.5f, 42.0f);
            benSprite.setScale(1.5f, 1.5f); 
        }
    } else {
        currentEnergy += 5.0f * dt;
        if (currentEnergy > maxEnergy) currentEnergy = maxEnergy;
    }
    float energyPct = currentEnergy / maxEnergy;
    energyBar.setSize(sf::Vector2f(200.0f * energyPct, 10.0f));

    // 3. Animación
    if(isHeatblast){
        if(isMoving){
            animationTimer += dt; 
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f; currentFrame++; if(currentFrame >= 6) currentFrame = 0;
            }
            int fuegoX[] = {16, 67, 106, 155, 210, 250};
            int fuegoW[] = {25, 20, 34, 32, 21, 36};
            benSprite.setTextureRect(sf::IntRect(fuegoX[currentFrame], 82, fuegoW[currentFrame], 55));
            benSprite.setOrigin(fuegoW[currentFrame] / 2.0f, 26.5f);
        } else {
            benSprite.setTextureRect(sf::IntRect(9, 11, 35, 53));
            benSprite.setOrigin(17.5f, 26.5f);
        }
    } else { 
        if(isMoving){
            animationTimer += dt; 
            if(animationTimer >= 0.1f){
                animationTimer = 0.0f; currentFrame++; if(currentFrame >= 6) currentFrame = 0;
            }
            int walkingFramesX[] = {17, 78, 137, 181, 238, 295};
            benSprite.setTextureRect(sf::IntRect(walkingFramesX[currentFrame], 382, 47, 79));
            benSprite.setOrigin(23.5f, 39.5f);
        } else {
            currentFrame = 0; 
            benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
            benSprite.setOrigin(25.5f, 42.0f);
        }
    }

    // 4. Físicas
    b2Body_SetLinearVelocity(benBodyId, velocity);
    b2World_Step(worldId, 1.0f / 60.0f, 4);

    b2Vec2 pos = b2Body_GetPosition(benBodyId);
    benSprite.setPosition(pos.x,pos.y);

    if (pos.y > 800.0f) {
        currentHealth = 0;
        gameState = GAME_OVER; 
    }

    // EJERCITO DE ENEMIGOS
    for (size_t i = 0; i < enemies.size(); i++) {
        b2Vec2 ePos = b2Body_GetPosition(enemies[i].bodyId);
        enemies[i].shape.setPosition(ePos.x, ePos.y);

        float limitRight = enemies[i].startX + enemies[i].patrolRange;
        float limitLeft  = enemies[i].startX - enemies[i].patrolRange;
        if (ePos.x > limitRight && enemies[i].speed > 0) enemies[i].speed = -100.0f; 
        else if (ePos.x < limitLeft && enemies[i].speed < 0) enemies[i].speed = 100.0f;  
        b2Vec2 vel = b2Body_GetLinearVelocity(enemies[i].bodyId);
        vel.x = enemies[i].speed;
        b2Body_SetLinearVelocity(enemies[i].bodyId, vel);

        if(benSprite.getGlobalBounds().intersects(enemies[i].shape.getGlobalBounds())){
             if(damageTimer <= 0.0f){
                hitSound.play();
                currentHealth -= 25;
                std::cout << "¡GOLPE DE DRON! Vida: " << currentHealth << std::endl;
                damageTimer = 2.0f; 
                b2Vec2 knockback = {-500.0, -300.0};
                if (benSprite.getScale().x < 0) knockback.x = 500.0f;
                b2Body_SetLinearVelocity(benBodyId, knockback);
                float percentage = (float)currentHealth / (float)maxHealth;
                if(percentage < 0) percentage = 0;
                healthBar.setSize(sf::Vector2f(200.0f * percentage, 20.0f));
                if (currentHealth <= 0) gameState = GAME_OVER; 
             }
        }
    }
    
    if (damageTimer > 0.0f) {
        damageTimer -= dt;
        healthBar.setFillColor(sf::Color::Red); 
    } else {
        healthBar.setFillColor(sf::Color::Green); 
    }
    // VICTORIA
    if(benSprite.getGlobalBounds().intersects(goalShape.getGlobalBounds())){
        gameState = VICTORY; 
        gameOverText.setString("YOU WIN!");
        gameOverText.setFillColor(sf::Color::Green);
    }
    
    if (shootCooldown > 0.0f) shootCooldown -= dt; 
    
    // Balas
    for(size_t i = 0; i < projectiles.size(); i++){
        projectiles[i].shape.move(projectiles[i].speed * dt, 0.0f); 
        projectiles[i].lifetime += dt;
        if (projectiles[i].lifetime > 2.0f) projectiles[i].destroy = true;
        
        for (size_t k = 0; k < enemies.size(); k++) {
            if (!enemies[k].destroy && projectiles[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds())) {
                projectiles[i].destroy = true; 
                b2Body_SetTransform(enemies[k].bodyId, {-1000.0f, -1000.0f}, {1.0f, 0.0f});
                enemies[k].shape.setPosition(-1000.0f, -1000.0f);
                enemies[k].destroy = true; 
                std::cout << "¡DRON ELIMINADO!" << std::endl;
            }
        }
    }
    auto iterator = std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p){ return p.destroy;});
    projectiles.erase(iterator, projectiles.end());
}

void Game::render(){
    window.clear(sf::Color::Black); 

    // --- ESTADOS QUE USAN EL FONDO ---
    if (gameState == MENU || gameState == INSTRUCTIONS || gameState == GAME_OVER || gameState == VICTORY) {
        window.setView(window.getDefaultView());
        // 1. DIBUJAR EL FONDO PRIMERO
        window.draw(menuSprite);

        // 2. DIBUJAR EL TEXTO CORRESPONDIENTE ENCIMA
        if (gameState == MENU) {
            window.draw(titleText);
            window.draw(startText);
        }
        else if (gameState == INSTRUCTIONS) {
            window.draw(instructionsText);
        }
        else if (gameState == GAME_OVER || gameState == VICTORY) {
            window.draw(gameOverText);
            window.draw(restartText);
        }
    }
    // --- ESTADO DE JUEGO ---
    else if (gameState == PLAYING) {
        //1.Dibujar fondo estatico
        window.setView(window.getDefaultView());
        window.draw(levelSprite);
        //2.Configurar camara de juegos
        sf::Vector2f benPos = benSprite.getPosition();
        sf::View camera(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f, 600.0f));
        camera.setCenter(benPos);
        window.setView(camera);

        for (const auto& shape : platformShapes) window.draw(shape);
        window.draw(goalSprite); 
        for (const auto& enemy : enemies) {
            if (!enemy.destroy) window.draw(enemy.shape);
        }
        for (const auto& proj : projectiles) window.draw(proj.shape);
        window.draw(benSprite);
        
        window.setView(window.getDefaultView());
        window.draw(healthBarBack); window.draw(healthBar); 
        window.draw(energyBarBack); window.draw(energyBar);
    }
    window.display(); 
}
void Game::spawnEnemy(float x, float y, float range) {
    Enemy newEnemy;
    b2BodyDef enemyDef = b2DefaultBodyDef();
    enemyDef.type = b2_kinematicBody; 
    enemyDef.position = {x, y};
    enemyDef.fixedRotation = true;
    newEnemy.bodyId = b2CreateBody(worldId, &enemyDef);
    b2Polygon enemyBox = b2MakeBox(15.0f, 15.0f);
    b2ShapeDef enemyShapeDef = b2DefaultShapeDef();
    enemyShapeDef.density = 1.0f;
    b2CreatePolygonShape(newEnemy.bodyId, &enemyShapeDef, &enemyBox);
    newEnemy.shape.setSize(sf::Vector2f(30.0f, 30.0f));
    newEnemy.shape.setOrigin(15.0f, 15.0f);
    newEnemy.shape.setFillColor(sf::Color::Red);
    newEnemy.shape.setPosition(x, y);
    newEnemy.speed = 100.0f; newEnemy.startX = x; newEnemy.patrolRange = range; 
    newEnemy.destroy = false;
    enemies.push_back(newEnemy);
}

void Game::createPlatform(float x, float y, float width, float height){
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.position = {x, y}; 
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon box = b2MakeBox(width / 2.0f, height / 2.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(bodyId, &shapeDef, &box);
    sf::RectangleShape shape(sf::Vector2f(width, height));
    shape.setOrigin(width / 2.0f, height / 2.0); 
    shape.setPosition(x, y);
    shape.setFillColor(sf::Color(0, 180, 0)); 
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::White);
    platformShapes.push_back(shape);
}

void Game::resetLevel() {
    b2Body_SetTransform(benBodyId, {100.0f, 450.0f}, {1.0f, 0.0f});
    b2Body_SetLinearVelocity(benBodyId, {0.0f, 0.0f});
    currentHealth = 100;
    currentEnergy = 100.0f;
    isHeatblast = false;
    benSprite.setTexture(benTexture);
    benSprite.setTextureRect(sf::IntRect(8, 23, 51, 84));
    benSprite.setOrigin(25.5f, 42.0f);
    benSprite.setScale(1.5f, 1.5f);

    for(auto& e : enemies) {
        if(b2Body_IsValid(e.bodyId)){ 
             b2DestroyBody(e.bodyId);
        }
    }
    enemies.clear();

    // Spawn sincronizado (VOLADORES)
    spawnEnemy(700.0f, 500.0f, 150.0f);
    spawnEnemy(1600.0f, 500.0f, 100.0f);
    spawnEnemy(2200.0f, 500.0f, 100.0f);
    spawnEnemy(3500.0f, 160.0f, 200.0f); 
    spawnEnemy(4000.0f, 160.0f, 200.0f); 
    spawnEnemy(4600.0f, 160.0f, 100.0f);
    spawnEnemy(4800.0f, 160.0f, 100.0f);

    gameState = PLAYING;
    if(gameOverText.getString() == "YOU WIN!") {
        gameOverText.setString("GAME OVER");
        gameOverText.setFillColor(sf::Color::Red);
    }
}