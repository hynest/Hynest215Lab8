
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

Event event;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(crossbow.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        crossbow.setCenter(newPos);
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}
int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunt");
    World world(Vector2f(0, 0));
    int score(0);
    int i(0);
    int arrows(6);
    bool gameon(true);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "images/red_balloon.png");
    PhysicsShapeList<PhysicsSprite> balloons;
    

    top.onCollision = [&drawingArrow, &world, &arrow, &arrows]
    (PhysicsBodyCollisionResult result) {
        arrows = arrows - 1;
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };

    Font fnt;
    if (!fnt.loadFromFile("ARIAL.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    while (true) {

        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();

        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            if (arrows == 0) {
                gameon = false;
            }
          
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

            }
            for (i; i < 100; i++) {
                PhysicsSprite& balloon = balloons.Create();
                balloon.setTexture(redTex);
                int x = 0 - 150 * i;
                Vector2f sz = balloon.getSize();
                balloon.setCenter(Vector2f(x, 20 + (sz.y / 2)));
                balloon.setVelocity(Vector2f(0.25, 0));
                if (arrows != 0) {
                    world.AddPhysicsBody(balloon);
                }
                
                if (arrows == 0) {
                    world.RemovePhysicsBody(balloon);
                    balloons.QueueRemove(balloon);
                }
                balloon.onCollision =
                    [&gameon, &drawingArrow, &world, &arrow, &arrows, &balloon, &balloons, &score, &right]
                    (PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        drawingArrow = false;
                        arrows = arrows - 1;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(balloon);
                        balloons.QueueRemove(balloon);
                        score += 10;
                    }
                    if (result.object2 == right) {
                        world.RemovePhysicsBody(balloon);
                        balloons.QueueRemove(balloon);
                    }

                    };
            }

            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            balloons.DoRemovals();
            for (PhysicsShape& balloon : balloons) {
                window.draw((PhysicsSprite&)balloon);
            }
            window.draw(crossBow);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(fnt);
            arrowCountText.setPosition(Vector2f(790 - GetTextSize(arrowCountText).x, 0));
            if (arrows != 0) {
                window.draw(scoreText);
                window.draw(arrowCountText);
            }
            //world.VisualizeAllBounds(window);

            while (arrows <= 0) {
                currentTime = clock.getElapsedTime();
                Time deltaTime = currentTime - lastTime;
                long deltaMS = deltaTime.asMilliseconds();

                Text gameOverText;
                gameOverText.setString("GAME OVER");
                gameOverText.setFont(fnt);
                gameon = false;
                Vector2f szt = GetTextSize(gameOverText);
                gameOverText.setPosition(400 - (szt.x / 2), 300 - (szt.y / 2));
                Text gameOverScore;
                gameOverScore.setString(to_string(score));
                gameOverScore.setFont(fnt);
                Vector2f szs = GetTextSize(gameOverScore);
                gameOverScore.setPosition(400 - (szs.x / 2), 350 - (szs.y / 2));
                window.draw(gameOverText);
                window.draw(gameOverScore);
                window.display();

                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                    window.clear();
                    arrows = 5;
                    score = 0;
                    gameon = true;
                }
            }

            window.display();
                
        }
    }
    while (true);
}