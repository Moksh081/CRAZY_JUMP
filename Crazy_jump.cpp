#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <limits> // For high score

int windowWidth = 400;
int windowHeight = 600;

float playerX = windowWidth / 2.0f;
float playerY = windowHeight / 5.0f;
float playerWidth = 50.0f;
float playerHeight = 60.0f;
float playerVelX = 0.0f;
float playerVelY = 0.0f;
float moveSpeed = 4.0f;
float gravity = 0.3f;
float jumpStrength = 10.0f;
float boostedJumpStrength = 18.0f; // For spring power-up
bool hasBoost = false;
int boostDuration = 60; // Frames
int boostTimer = 0;

struct Platform
{
    float x, y;
    float width = 60.0f;
    float height = 10.0f;
    bool moving = false;
    float velX = 2.0f;
    bool breakable = false;
    bool broken = false;
};

struct PowerUp
{
    float x, y;
    float size = 20.0f;
    bool active = true;
};

std::vector<Platform> platforms;
std::vector<PowerUp> powerUps;

int initialPlatforms = 10;
float platformSpacing = 80.0f;

float cameraY = 0.0f;
int score = 0;
int highScore = 0;

enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER
};
GameState gameState = MENU;

void drawRect(float x, float y, float width, float height)
{
    glBegin(GL_QUADS);
    glVertex2f(x - width / 2, y - height / 2);
    glVertex2f(x + width / 2, y - height / 2);
    glVertex2f(x + width / 2, y + height / 2);
    glVertex2f(x - width / 2, y + height / 2);
    glEnd();
}

void renderBitmapString(float x, float y, void *font, const char *string)
{
    glRasterPos2f(x, y);
    while (*string)
    {
        glutBitmapCharacter(font, *string);
        ++string;
    }
}

void drawPlayer()
{
    glColor3f(0.9f, 0.1f, 0.1f);
    drawRect(playerX, playerY, playerWidth, playerHeight);
}

void drawPlatforms()
{
    for (const auto &p : platforms)
    {
        if (p.broken)
            continue;
        if (p.breakable)
            glColor3f(0.8f, 0.5f, 0.5f);
        else if (p.moving)
            glColor3f(0.4f, 0.4f, 0.9f);
        else
            glColor3f(0.5f, 0.25f, 0.0f);
        drawRect(p.x, p.y, p.width, p.height);
    }
}

void drawPowerUps()
{
    for (const auto &pu : powerUps)
    {
        if (!pu.active)
            continue;
        glColor3f(1.0f, 0.8f, 0.2f);
        drawRect(pu.x, pu.y, pu.size, pu.size);
    }
}

void generateInitialPlatforms()
{
    platforms.clear();
    platforms.push_back({windowWidth / 2.0f, 50.0f});
    float currentY = platforms[0].y + platformSpacing;
    for (int i = 1; i < initialPlatforms; ++i)
    {
        float randX = (rand() % (windowWidth - 60)) + 30;
        Platform p = {randX, currentY};
        // Randomly assign moving/breakable
        int type = rand() % 10;
        if (type < 2)
        {
            p.moving = true;
        } // 20% chance
        else if (type < 4)
        {
            p.breakable = true;
        } // 20% chance
        platforms.push_back(p);
        currentY += platformSpacing;
    }

    // Power-ups
    powerUps.clear();
    for (int i = 0; i < 3; ++i)
    {
        float randX = (rand() % (windowWidth - 40)) + 20;
        float randY = (rand() % 1000) + 200;
        powerUps.push_back({randX, randY});
    }
}

void generateNewPlatforms()
{
    while (platforms.back().y < cameraY + windowHeight + platformSpacing)
    {
        float lastY = platforms.back().y;
        float randX = (rand() % (windowWidth - 60)) + 30;
        Platform p = {randX, lastY + platformSpacing};
        int type = rand() % 10;
        if (type < 2)
            p.moving = true;
        else if (type < 4)
            p.breakable = true;
        platforms.push_back(p);
        score += 10;
    }

    // Occasionally generate new power-up
    if (rand() % 15 == 0)
    {
        float randX = (rand() % (windowWidth - 40)) + 20;
        float randY = platforms.back().y + 50;
        powerUps.push_back({randX, randY});
    }
}

void removeOldPlatforms()
{
    platforms.erase(std::remove_if(platforms.begin(), platforms.end(),
                                   [&](const Platform &p)
                                   {
                                       return p.y < cameraY - p.height;
                                   }),
                    platforms.end());

    powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(),
                                  [&](const PowerUp &pu)
                                  {
                                      return pu.y < cameraY - pu.size;
                                  }),
                   powerUps.end());
}

void resetGame()
{
    playerX = windowWidth / 2.0f;
    playerY = windowHeight / 5.0f;
    playerVelX = 0.0f;
    playerVelY = 0.0f;
    cameraY = 0.0f;
    score = 0;
    hasBoost = false;
    boostTimer = 0;
    generateInitialPlatforms();
}

void updatePlatforms()
{
    for (auto &p : platforms)
    {
        if (p.moving && !p.broken)
        {
            p.x += p.velX;
            if (p.x < p.width / 2 || p.x > windowWidth - p.width / 2)
            {
                p.velX *= -1;
            }
        }
    }
}

void update()
{
    if (gameState != PLAYING)
        return;

    playerVelY -= gravity;
    playerY += playerVelY;
    playerX += playerVelX;

    if (playerX > windowWidth + playerWidth / 2)
        playerX = -playerWidth / 2;
    else if (playerX < -playerWidth / 2)
        playerX = windowWidth + playerWidth / 2;

    updatePlatforms();

    if (playerVelY < 0)
    {
        for (auto &p : platforms)
        {
            if (p.broken)
                continue;
            bool xOverlap = playerX + playerWidth / 2 > p.x - p.width / 2 &&
                            playerX - playerWidth / 2 < p.x + p.width / 2;
            bool yOverlap = playerY - playerHeight / 2 < p.y + p.height / 2 &&
                            playerY - playerHeight / 2 > p.y - p.height / 2;
            if (xOverlap && yOverlap)
            {
                playerY = p.y + p.height / 2 + playerHeight / 2;
                playerVelY = hasBoost ? boostedJumpStrength : jumpStrength;
                if (p.breakable)
                    p.broken = true;
                // playJumpSound();  <-- SOUND placeholder
                break;
            }
        }
    }

    // Power-up collision
    for (auto &pu : powerUps)
    {
        if (!pu.active)
            continue;
        bool xOverlap = playerX + playerWidth / 2 > pu.x - pu.size / 2 &&
                        playerX - playerWidth / 2 < pu.x + pu.size / 2;
        bool yOverlap = playerY + playerHeight / 2 > pu.y - pu.size / 2 &&
                        playerY - playerHeight / 2 < pu.y + pu.size / 2;
        if (xOverlap && yOverlap)
        {
            hasBoost = true;
            boostTimer = boostDuration;
            pu.active = false;
            // playPowerUpSound();  <-- SOUND placeholder
        }
    }

    if (hasBoost)
    {
        boostTimer--;
        if (boostTimer <= 0)
            hasBoost = false;
    }

    if (playerY > cameraY + windowHeight / 2.0f)
    {
        cameraY = playerY - windowHeight / 2.0f;
    }

    generateNewPlatforms();
    removeOldPlatforms();

    if (playerY < cameraY - playerHeight)
    {
        gameState = GAME_OVER;
        if (score > highScore)
            highScore = score;
        std::cout << "Game Over! Final Score: " << score << std::endl;
        // playGameOverSound();  <-- SOUND placeholder
    }

    glutPostRedisplay();
}

void setBackgroundColorByScore()
{
    int stage = score / 100;
    switch (stage % 4)
    {
    case 0:
        glClearColor(0.8f, 0.9f, 1.0f, 1.0f);
        break;
    case 1:
        glClearColor(0.9f, 0.8f, 0.9f, 1.0f);
        break;
    case 2:
        glClearColor(0.9f, 0.9f, 0.7f, 1.0f);
        break;
    case 3:
        glClearColor(0.7f, 0.9f, 0.7f, 1.0f);
        break;
    }
}

void display()
{
    setBackgroundColorByScore();
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (gameState == MENU)
    {
        glColor3f(0.2f, 0.6f, 1.0f);
        renderBitmapString(windowWidth / 2 - 80, windowHeight / 2 + 20, GLUT_BITMAP_HELVETICA_18, "Simple Jump Game");
        glColor3f(0.0f, 0.0f, 0.0f);
        renderBitmapString(windowWidth / 2 - 100, windowHeight / 2 - 20, GLUT_BITMAP_HELVETICA_18, "Press SPACE to Start");
    }
    else if (gameState == GAME_OVER)
    {
        glColor3f(0.8f, 0.1f, 0.1f);
        renderBitmapString(windowWidth / 2 - 60, windowHeight / 2 + 20, GLUT_BITMAP_HELVETICA_18, "Game Over!");
        std::stringstream ss;
        ss << "Final Score: " << score;
        renderBitmapString(windowWidth / 2 - 70, windowHeight / 2 - 10, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());
        ss.str("");
        ss.clear();
        ss << "High Score: " << highScore;
        renderBitmapString(windowWidth / 2 - 70, windowHeight / 2 - 30, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());
        glColor3f(0.0f, 0.0f, 0.0f);
        renderBitmapString(windowWidth / 2 - 90, windowHeight / 2 - 50, GLUT_BITMAP_HELVETICA_18, "Press R to Restart or ESC to Menu");
    }
    else if (gameState == PLAYING)
    {
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, windowWidth, 0, windowHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glColor3f(0.0f, 0.0f, 0.0f);
        std::stringstream ss;
        ss << "Score: " << score;
        renderBitmapString(10.0f, windowHeight - 20.0f, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glTranslatef(0.0f, -cameraY, 0.0f);
        drawPlatforms();
        drawPowerUps();
        drawPlayer();
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (gameState == MENU && key == 32)
    {
        resetGame();
        gameState = PLAYING;
    }
    else if (gameState == GAME_OVER)
    {
        if (key == 'r')
        {
            resetGame();
            gameState = PLAYING;
        }
        else if (key == 27)
        {
            gameState = MENU;
        }
    }
    else if (gameState == PLAYING)
    {
        if (key == 'a')
            playerVelX = -moveSpeed;
        else if (key == 'd')
            playerVelX = moveSpeed;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (gameState == PLAYING && (key == 'a' || key == 'd'))
    {
        playerVelX = 0.0f;
    }
}

void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int)
{
    update();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char **argv)
{
    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Simple Jump Game");

    glClearColor(0.8f, 0.9f, 1.0f, 1.0f);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
