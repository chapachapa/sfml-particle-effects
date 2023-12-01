
//#include "crtdbg.h"
//#include "mydbgnew.h"
//
//#ifdef _DEBUG
//    #define new DEBUG_CLIENTBLOCK
//#endif

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__)
#define new DBG_NEW
#endif
#endif

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>


const int maxParticles = 64;
const float PI = 3.141592654;
const int maxSpeed = 500;
const int maxRandomSize = 32;

int rollRandomColorValue() {
    return abs(rand() % 255 + 1);
}

class Particle {
public:
    sf::Vector2f pixelPos;
    sf::Vector2f pixelVelocity;
    float pixelRadius;
    float timeLeftToLive;
    float totalTime;
    sf::CircleShape shape;
    sf::Color color;


    Particle() : Particle(32, sf::Vector2f(0,0), sf::Vector2f(0,0), 1.f, sf::Color::White) {
    }

    Particle(float pixelRadius, sf::Vector2f initPos, sf::Vector2f initVelocity, float timeLeftToLive, sf::Color color) {
        this->pixelPos = initPos;
        this->pixelVelocity = initVelocity;
        this->pixelRadius = pixelRadius;
        this->timeLeftToLive = timeLeftToLive;
        this->totalTime = timeLeftToLive;
        this->color = color;
    }


    sf::CircleShape getShape() {
        shape = sf::CircleShape(pixelRadius);
        shape.setPosition(pixelPos);
        shape.setFillColor(color);
        return shape;
    }

    void update(float dt) {
        pixelPos += pixelVelocity * dt;
        timeLeftToLive -= dt;
        color.a = sf::Uint8(255 * timeLeftToLive / totalTime);
        color.a = sf::Uint8(255 * (1 - pow(2, -10 * timeLeftToLive / totalTime)));
        shape.setFillColor(color);
    }

    bool isDead() {
        return timeLeftToLive <= 0.f;
    }
    
};

int rollRandomPositiveInteger(int maxValue_exclusive) {
    return abs(rand()) % maxValue_exclusive;
}

class ParticleEffects {
public: 
    std::vector<Particle> particles;
    sf::Color particleColor;
    ParticleEffects(sf::Vector2f initPos) {
        this->particles.reserve(maxParticles);
        
        particleColor = sf::Color(rollRandomColorValue(), rollRandomColorValue(), rollRandomColorValue());

        for (int i = 0; i < maxParticles; i++) {
            float randomSpeed = rollRandomPositiveInteger(maxSpeed + 1);
            float randomAngleInDegrees = rollRandomPositiveInteger(360);
            float randomAngleInRadians = randomAngleInDegrees * PI / 180.f;
            auto randomDirection = sf::Vector2f(
                cos(randomAngleInRadians),
                sin(randomAngleInRadians)
            );
            float randomSize = rollRandomPositiveInteger(maxRandomSize + 1);
            float randomDuration = rollRandomPositiveInteger(3);
            auto randVelocity = randomDirection * randomSpeed;
            auto newParticle = Particle(randomSize, initPos - sf::Vector2f(randomSize, randomSize), randVelocity, 5+randomDuration, particleColor);
            particles.push_back(newParticle);
        }

    }

    void updateParticles(float dt) {
        for (int i = particles.size() - 1; i >= 0; i--) {

            particles[i].update(dt);

            auto curPart = particles[i];
            if (curPart.isDead()) {
                particles.erase(particles.begin() + i);
            }
        }
    }

    void drawParticles(sf::RenderWindow& window) {
        for (int i = 0; i < particles.size(); i++) {
            window.draw(particles[i].getShape());
        }
    }

    bool allDead() {
        return particles.size() <= 0;
    }
};


// random sizes
// random directions
// random velocities
// random lifetimes

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    sf::RenderWindow window(sf::VideoMode(1600, 900), "Particle Effects - Click anyone on screen");
    bool leftButtonDown = false;
    
    ParticleEffects* particleEffects;
    std::vector<ParticleEffects*> particleEffectsCollection;

    sf::Clock deltaSrc;

    while (window.isOpen())
    {
        sf::Time dt_time = deltaSrc.restart();
        float dt = dt_time.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(window));
            sf::Vector2f windowSize = sf::Vector2f(window.getSize());

            if (mousePosition.x >= 0 && mousePosition.x <= windowSize.x && mousePosition.y >= 0 && mousePosition.y <= windowSize.y) {

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (!leftButtonDown) {
                        leftButtonDown = true;
                        particleEffectsCollection.push_back(new ParticleEffects(mousePosition));
                    }
                }
                else {
                    if (leftButtonDown) {
                        leftButtonDown = false;
                    }
                }
            }
        }


        window.clear();

        for (int x = particleEffectsCollection.size() - 1; x >= 0; x--) {
            if (particleEffectsCollection[x]->allDead()) {
                ParticleEffects* p = particleEffectsCollection[x];
                particleEffectsCollection.erase(particleEffectsCollection.begin() + x);
                delete p;
            }
            else {
                particleEffectsCollection[x]->updateParticles(dt);
                particleEffectsCollection[x]->drawParticles(window);
            }
        }
        

        window.display();
    }

    /*_CrtDumpMemoryLeaks();*/

    return 0;
}