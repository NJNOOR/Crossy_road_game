#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <string>

using namespace std;

// Game constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int LANE_COUNT = 5;
const int LANE_HEIGHT = WINDOW_HEIGHT / LANE_COUNT;
const float PLAYER_SIZE = 50.0f;

// Game state
enum GameState { START_SCREEN, PLAYING, LEVEL_COMPLETE, GAME_OVER };
GameState currentState = START_SCREEN;
int currentLevel = 1;
int score = 0;

// Player position
float playerX = WINDOW_WIDTH / 2;
float playerY = LANE_HEIGHT / 2;

// Color structure
struct Color {
    float r, g, b;
};

// Car obstacle structure
struct Obstacle {
    float x;
    int lane;
    float speed;
    float width;
    float height;
    bool active;
    bool movesRight;
    Color bodyColor;
};

vector<Obstacle> obstacles;
float obstacleSpawnTimer = 0;
const float OBSTACLE_SPAWN_INTERVAL = 2.0f;

// Colors
const Color ROAD_COLOR = { 0.2f, 0.2f, 0.2f };
const Color GRASS_COLOR = { 0.0f, 0.5f, 0.0f };
const Color SKIN_COLOR = { 0.96f, 0.80f, 0.69f };
const Color SHIRT_COLOR = { 0.0f, 0.5f, 1.0f };
const Color PANTS_COLOR = { 0.2f, 0.2f, 0.6f };
const Color CAR_BODY_COLORS[] = {
    {0.9f, 0.2f, 0.2f}, {0.2f, 0.4f, 0.8f}, {0.1f, 0.7f, 0.1f},
    {0.9f, 0.7f, 0.1f}, {0.8f, 0.3f, 0.8f}
};

// Function declarations with default arguments
void drawText(const string& text, float x, float y, float scale, bool center,
    Color color, Color outlineColor);
void drawMultiLineText(const vector<string>& lines, float startY, float lineHeight,
    float scale, Color color, Color outlineColor);

void init() {
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    srand(time(0));
}

void drawRect(float x, float y, float width, float height, Color color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawCircle(float x, float y, float radius, Color color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 360; i++) {
        glVertex2f(x + (radius * cos(i * 3.14159f / 180)),
            y + (radius * sin(i * 3.14159f / 180)));
    }
    glEnd();
}

void drawCar(const Obstacle& car) {
    // Car body
    glColor3f(car.bodyColor.r, car.bodyColor.g, car.bodyColor.b);
    glBegin(GL_POLYGON);
    glVertex2f(car.x, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.3f);
    glVertex2f(car.x + car.width, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.3f);
    glVertex2f(car.x + car.width, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.7f);
    glVertex2f(car.x, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.7f);
    glEnd();

    // Car top
    glBegin(GL_POLYGON);
    glVertex2f(car.x + car.width * 0.2f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.7f);
    glVertex2f(car.x + car.width * 0.8f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.7f);
    glVertex2f(car.x + car.width * 0.7f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.9f);
    glVertex2f(car.x + car.width * 0.3f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.9f);
    glEnd();

    // Windows
    glColor3f(0.7f, 0.8f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(car.x + car.width * 0.3f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.7f);
    glVertex2f(car.x + car.width * 0.7f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.7f);
    glVertex2f(car.x + car.width * 0.7f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.9f);
    glVertex2f(car.x + car.width * 0.3f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.9f);
    glEnd();

    // Wheels
    drawCircle(car.x + car.width * 0.2f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.2f,
        LANE_HEIGHT * 0.1f, { 0.1f, 0.1f, 0.1f });
    drawCircle(car.x + car.width * 0.8f, car.lane * LANE_HEIGHT + LANE_HEIGHT * 0.2f,
        LANE_HEIGHT * 0.1f, { 0.1f, 0.1f, 0.1f });
}

void drawPlayer() {
    // Head
    drawCircle(playerX, playerY + PLAYER_SIZE * 0.3f, PLAYER_SIZE * 0.25f, SKIN_COLOR);

    // Body (shirt)
    drawRect(playerX - PLAYER_SIZE * 0.25f, playerY - PLAYER_SIZE * 0.15f,
        PLAYER_SIZE * 0.5f, PLAYER_SIZE * 0.5f, SHIRT_COLOR);

    // Legs (pants)
    drawRect(playerX - PLAYER_SIZE * 0.2f, playerY - PLAYER_SIZE * 0.5f,
        PLAYER_SIZE * 0.15f, PLAYER_SIZE * 0.35f, PANTS_COLOR);
    drawRect(playerX + PLAYER_SIZE * 0.05f, playerY - PLAYER_SIZE * 0.5f,
        PLAYER_SIZE * 0.15f, PLAYER_SIZE * 0.35f, PANTS_COLOR);

    // Arms
    drawRect(playerX - PLAYER_SIZE * 0.4f, playerY,
        PLAYER_SIZE * 0.15f, PLAYER_SIZE * 0.25f, SKIN_COLOR);
    drawRect(playerX + PLAYER_SIZE * 0.25f, playerY,
        PLAYER_SIZE * 0.15f, PLAYER_SIZE * 0.25f, SKIN_COLOR);

    // Eyes
    drawCircle(playerX - PLAYER_SIZE * 0.1f, playerY + PLAYER_SIZE * 0.35f,
        PLAYER_SIZE * 0.03f, { 0.0f, 0.0f, 0.0f });
    drawCircle(playerX + PLAYER_SIZE * 0.1f, playerY + PLAYER_SIZE * 0.35f,
        PLAYER_SIZE * 0.03f, { 0.0f, 0.0f, 0.0f });

    // Mouth (smile)
    glColor3f(0.5f, 0.2f, 0.2f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 180; i += 10) {
        glVertex2f(playerX + (PLAYER_SIZE * 0.15f * cos(i * 3.14159f / 180)),
            playerY + PLAYER_SIZE * 0.25f + (PLAYER_SIZE * 0.08f * sin(i * 3.14159f / 180)));
    }
    glEnd();
}

void drawRoad() {
    for (int i = 0; i < LANE_COUNT; i++) {
        if (i % 2 == 1) { // Road lanes
            drawRect(0, i * LANE_HEIGHT, WINDOW_WIDTH, LANE_HEIGHT, ROAD_COLOR);

            // Lane markings
            glColor3f(1.0f, 1.0f, 1.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            for (int x = 0; x < WINDOW_WIDTH; x += 40) {
                glVertex2f(x, i * LANE_HEIGHT + LANE_HEIGHT / 2);
                glVertex2f(x + 20, i * LANE_HEIGHT + LANE_HEIGHT / 2);
            }
            glEnd();
        }
        else { // Grass lanes
            drawRect(0, i * LANE_HEIGHT, WINDOW_WIDTH, LANE_HEIGHT, GRASS_COLOR);
        }
    }
}

void spawnObstacle() {
    Obstacle car;
    car.lane = (rand() % (LANE_COUNT / 2)) * 2 + 1;
    car.movesRight = (rand() % 2 == 0);

    if (car.movesRight) {
        car.x = -150;
        car.speed = 1.0f + currentLevel * 0.5f;
    }
    else {
        car.x = WINDOW_WIDTH + 150;
        car.speed = -(1.0f + currentLevel * 0.5f);
    }

    car.width = 120 + rand() % 60;
    car.height = LANE_HEIGHT * 0.7f;
    car.active = true;
    car.bodyColor = CAR_BODY_COLORS[rand() % 5];

    obstacles.push_back(car);
}

void updateObstacles(float deltaTime) {
    for (Obstacle& car : obstacles) {
        if (car.active) {
            car.x += car.speed * deltaTime * 60;

            if ((car.movesRight && car.x > WINDOW_WIDTH + 200) ||
                (!car.movesRight && car.x < -200 - car.width)) {
                car.active = false;
            }
        }
    }

    obstacles.erase(
        remove_if(obstacles.begin(), obstacles.end(),
            [](const Obstacle& car) { return !car.active; }),
        obstacles.end());

    obstacleSpawnTimer += deltaTime;
    if (obstacleSpawnTimer >= OBSTACLE_SPAWN_INTERVAL / (1 + currentLevel * 0.2)) {
        spawnObstacle();
        obstacleSpawnTimer = 0;
    }
}

bool checkCollision() {
    for (const Obstacle& car : obstacles) {
        if (car.active &&
            playerX + PLAYER_SIZE / 2 > car.x &&
            playerX - PLAYER_SIZE / 2 < car.x + car.width &&
            playerY + PLAYER_SIZE / 2 > car.lane * LANE_HEIGHT &&
            playerY - PLAYER_SIZE / 2 < (car.lane + 1) * LANE_HEIGHT) {
            return true;
        }
    }
    return false;
}

bool checkWin() {
    return playerY >= WINDOW_HEIGHT - PLAYER_SIZE;
}

void resetLevel() {
    playerX = WINDOW_WIDTH / 2;
    playerY = LANE_HEIGHT / 2;
    obstacles.clear();
    obstacleSpawnTimer = 0;
}

void drawText(const string& text, float x, float y, float scale, bool center,
    Color color, Color outlineColor) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    if (center) {
        float textWidth = 0;
        for (char c : text) {
            textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
        }
        glTranslatef(-textWidth * scale / 2, 0, 0);
    }
    glScalef(scale, scale, 1.0f);

    // Draw outline
    glColor3f(outlineColor.r, outlineColor.g, outlineColor.b);
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx != 0 || dy != 0) {
                glRasterPos2f(dx * 0.7f, dy * 0.7f);
                for (char c : text) {
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
                }
            }
        }
    }

    // Draw main text
    glColor3f(color.r, color.g, color.b);
    glRasterPos2f(0, 0);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glPopMatrix();
}

void drawMultiLineText(const vector<string>& lines, float startY, float lineHeight,
    float scale, Color color, Color outlineColor) {
    for (size_t i = 0; i < lines.size(); i++) {
        drawText(lines[i], WINDOW_WIDTH / 2, startY + i * lineHeight, scale, true, color, outlineColor);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Declare variables that might be used in any case block
    vector<string> menuLines;
    vector<string> levelCompleteLines;
    vector<string> gameOverLines;
    char levelText[30];
    char scoreText[30];
    char levelScoreText[50];
    char finalScoreText[50];

    switch (currentState) {
    case START_SCREEN:
        drawRoad();

        // Title with shadow effect
        drawText("CROSSY ROAD", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 90, 1.1f, true,
            { 1.0f, 0.8f, 0.2f }, { 0.2f, 0.2f, 0.2f });

        // Menu options - better aligned and spaced
        menuLines = {
            "Use arrow keys to move",
            "Press SPACE to start",
            "Press Q to quit"
        };
        drawMultiLineText(menuLines, WINDOW_HEIGHT / 2 - 40, 40.0f, 1.0f,
            { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
        break;

    case PLAYING:
        drawRoad();
        for (const auto& car : obstacles) {
            drawCar(car);
        }
        drawPlayer();

        // Score display - aligned to top-left corner
        sprintf(levelText, "Level: %d", currentLevel);
        drawText(levelText, 20, WINDOW_HEIGHT - 30, 1.0f, false,
            { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });

        sprintf(scoreText, "Score: %d", score);
        drawText(scoreText, 20, WINDOW_HEIGHT - 60, 1.0f, false,
            { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
        break;

    case LEVEL_COMPLETE:
        drawRoad();

        // Level complete message
        drawText("LEVEL COMPLETE!", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 90, 1.0f, true,
            { 0.2f, 1.0f, 0.2f }, { 0.0f, 0.2f, 0.0f });

        sprintf(levelScoreText, "Score: %d", score);
        drawText(levelScoreText, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 40, 1.2f, true,
            { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });

        // Better aligned options
        levelCompleteLines = {

        "Press Q to quit",
        "Press L for next level",
        };
        drawMultiLineText(levelCompleteLines, WINDOW_HEIGHT / 2 - 30, 40.0f, 1.1f,
            { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
        break;

    case GAME_OVER:
        drawRoad();

        // Game over message
        drawText("GAME OVER", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 90, 1.1f, true,
            { 1.0f, 0.2f, 0.2f }, { 0.3f, 0.0f, 0.0f });


        sprintf(finalScoreText, "Final Score: %d", score);
        drawText(finalScoreText, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 30, 1.1f, true,
            { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });

        // Better aligned options
        gameOverLines = {
            "Press SPACE to restart",
            "Press Q to quit"
        };
        drawMultiLineText(gameOverLines, WINDOW_HEIGHT / 2 - 40, 40.0f, 1.0f,
            { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
        break;
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
    case 'Q':
        exit(0);
        break;

    case 'l':
    case 'L':
        if (currentState == LEVEL_COMPLETE) {
            currentLevel++;
            resetLevel();
            currentState = PLAYING;
        }
        break;

    case ' ':
        if (currentState == START_SCREEN || currentState == GAME_OVER) {
            currentLevel = 1;
            score = 0;
            resetLevel();
            currentState = PLAYING;
        }
        break;
    }
}

void specialKeys(int key, int x, int y) {
    if (currentState != PLAYING) return;

    switch (key) {
    case GLUT_KEY_UP:
        playerY += LANE_HEIGHT;
        if (playerY > WINDOW_HEIGHT - PLAYER_SIZE / 2) {
            playerY = WINDOW_HEIGHT - PLAYER_SIZE / 2;
        }
        break;

    case GLUT_KEY_DOWN:
        playerY -= LANE_HEIGHT;
        if (playerY < PLAYER_SIZE / 2) {
            playerY = PLAYER_SIZE / 2;
        }
        break;

    case GLUT_KEY_LEFT:
        playerX -= LANE_HEIGHT;
        if (playerX < PLAYER_SIZE / 2) {
            playerX = PLAYER_SIZE / 2;
        }
        break;

    case GLUT_KEY_RIGHT:
        playerX += LANE_HEIGHT;
        if (playerX > WINDOW_WIDTH - PLAYER_SIZE / 2) {
            playerX = WINDOW_WIDTH - PLAYER_SIZE / 2;
        }
        break;
    }
}

void update(int value) {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (currentState == PLAYING) {
        updateObstacles(deltaTime);

        if (checkCollision()) {
            currentState = GAME_OVER;
        }
        else if (checkWin()) {
            score += currentLevel * 100;
            currentState = LEVEL_COMPLETE;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Crossy Road with Large Person Character");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
