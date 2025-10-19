#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAZE_WIDTH 28
#define MAZE_HEIGHT 31
#define CELL_SIZE 18

// Game states
enum GameState { MENU, PLAYING, PAUSED, GAME_OVER, WIN };
GameState currentState = MENU;

// Pacman structure
struct Pacman {
    float x, y;
    float speed;
    int direction; // 0=right, 1=up, 2=left, 3=down
    int nextDirection;
    float mouthAngle;
    int mouthOpening;
    int lives;
    int score;
} pacman;

// Ghost structure
struct Ghost {
    float x, y;
    float speed;
    int direction;
    int color; // 0=red, 1=pink, 2=cyan, 3=orange
    float stateTimer;
} ghosts[4];

// Game variables
int maze[MAZE_HEIGHT][MAZE_WIDTH];
int totalDots = 0;
int dotsEaten = 0;
float gameTime = 0;
float startTime = 0;
int highScore = 0;

// Simple maze layout (1=wall, 0=path with dot, 2=empty path, 3=ghost house)
void initMaze() {
    int mazeLayout[MAZE_HEIGHT][MAZE_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,0,1,1,1,1,1,2,1,1,2,1,1,1,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,1,1,1,2,1,1,2,1,1,1,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,2,2,2,2,2,2,2,2,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,3,3,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,3,3,3,3,3,3,1,2,1,1,0,1,1,1,1,1,1},
        {2,2,2,2,2,2,0,2,2,2,1,3,3,3,3,3,3,1,2,2,2,0,2,2,2,2,2,2},
        {1,1,1,1,1,1,0,1,1,2,1,3,3,3,3,3,3,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,2,2,2,2,2,2,2,2,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,1,1,1,1,1,0,1,1,2,1,1,1,1,1,1,1,1,2,1,1,0,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
        {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
        {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
        {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
        {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    
    totalDots = 0;
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            maze[i][j] = mazeLayout[i][j];
            if (maze[i][j] == 0) totalDots++;
        }
    }
}

void initPacman() {
    pacman.x = 14;
    pacman.y = 23;
    pacman.speed = 0.1f;
    pacman.direction = 0;
    pacman.nextDirection = 0;
    pacman.mouthAngle = 0;
    pacman.mouthOpening = 1;
    pacman.lives = 3;
    pacman.score = 0;
}

void initGhosts() {
    // Red ghost (Blinky)
    ghosts[0].x = 13; ghosts[0].y = 11; ghosts[0].color = 0; ghosts[0].speed = 0.08f;
    // Pink ghost (Pinky)
    ghosts[1].x = 14; ghosts[1].y = 14; ghosts[1].color = 1; ghosts[1].speed = 0.08f;
    // Cyan ghost (Inky)
    ghosts[2].x = 13; ghosts[2].y = 14; ghosts[2].color = 2; ghosts[2].speed = 0.08f;
    // Orange ghost (Clyde)
    ghosts[3].x = 15; ghosts[3].y = 14; ghosts[3].color = 3; ghosts[3].speed = 0.08f;
    
    for (int i = 0; i < 4; i++) {
        ghosts[i].direction = rand() % 4;
        ghosts[i].stateTimer = 0;
    }
}

void resetGame() {
    initMaze();
    initPacman();
    initGhosts();
    dotsEaten = 0;
    gameTime = 0;
    startTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

bool isWall(int x, int y) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT)
        return true;
    return maze[y][x] == 1;
}

bool canMove(float x, float y, int dir) {
    float nextX = x, nextY = y;
    float offset = 0.4f;
    
    switch(dir) {
        case 0: nextX += offset; break; // right
        case 1: nextY -= offset; break; // up
        case 2: nextX -= offset; break; // left
        case 3: nextY += offset; break; // down
    }
    
    int gridX = (int)(nextX + 0.5f);
    int gridY = (int)(nextY + 0.5f);
    
    return !isWall(gridX, gridY);
}

void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
    }
    glEnd();
}

void drawPacman() {
    glPushMatrix();
    glTranslatef(pacman.x * CELL_SIZE + CELL_SIZE/2, 
                 pacman.y * CELL_SIZE + CELL_SIZE/2, 0);
    glRotatef(-pacman.direction * 90, 0, 0, 1);
    
    glColor3f(1.0f, 1.0f, 0.0f);
    
    float startAngle = pacman.mouthAngle * M_PI / 180.0f;
    float endAngle = (360 - pacman.mouthAngle) * M_PI / 180.0f;
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (float angle = startAngle; angle <= endAngle; angle += 0.1f) {
        glVertex2f(cos(angle) * 8, sin(angle) * 8);
    }
    glEnd();
    
    glPopMatrix();
}

void drawGhost(Ghost &ghost) {
    float x = ghost.x * CELL_SIZE + CELL_SIZE/2;
    float y = ghost.y * CELL_SIZE + CELL_SIZE/2;
    
    // Set ghost color
    switch(ghost.color) {
        case 0: glColor3f(1.0f, 0.0f, 0.0f); break; // Red
        case 1: glColor3f(1.0f, 0.75f, 0.8f); break; // Pink
        case 2: glColor3f(0.0f, 1.0f, 1.0f); break; // Cyan
        case 3: glColor3f(1.0f, 0.65f, 0.0f); break; // Orange
    }
    
    // Body (semi-circle + rectangle)
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 180; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + cos(angle) * 8, y + sin(angle) * 8);
    }
    glEnd();
    
    // Bottom wavy part
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    glVertex2f(x - 8, y);
    glVertex2f(x - 6, y + 4);
    glVertex2f(x - 3, y);
    glVertex2f(x, y + 4);
    glVertex2f(x + 3, y);
    glVertex2f(x + 6, y + 4);
    glVertex2f(x + 8, y);
    glEnd();
    
    // Eyes
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(x - 3, y - 2, 2, 16);
    drawCircle(x + 3, y - 2, 2, 16);
    
    glColor3f(0.0f, 0.0f, 1.0f);
    drawCircle(x - 3, y - 2, 1, 16);
    drawCircle(x + 3, y - 2, 1, 16);
}

void drawMaze() {
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            float x = j * CELL_SIZE;
            float y = i * CELL_SIZE;
            
            if (maze[i][j] == 1) {
                // Wall
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_QUADS);
                glVertex2f(x, y);
                glVertex2f(x + CELL_SIZE, y);
                glVertex2f(x + CELL_SIZE, y + CELL_SIZE);
                glVertex2f(x, y + CELL_SIZE);
                glEnd();
            } else if (maze[i][j] == 0) {
                // Dot
                glColor3f(1.0f, 1.0f, 1.0f);
                drawCircle(x + CELL_SIZE/2, y + CELL_SIZE/2, 2, 8);
            }
        }
    }
}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawHUD() {
    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[100];
    
    sprintf(buffer, "Score: %d", pacman.score);
    drawText(520, 550, buffer);
    
    sprintf(buffer, "Lives: %d", pacman.lives);
    drawText(520, 520, buffer);
    
    sprintf(buffer, "Time: %.1f", gameTime);
    drawText(520, 490, buffer);
    
    sprintf(buffer, "Dots: %d/%d", dotsEaten, totalDots);
    drawText(520, 460, buffer);
}

void drawMenu() {
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(300, 400, "PAC-MAN");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(280, 320, "Press SPACE to Start");
    drawText(280, 280, "Press H for High Score");
    drawText(280, 240, "Press ESC to Exit");
    drawText(250, 180, "Controls: Arrow Keys");
    drawText(250, 140, "Press P to Pause");
}

void drawPauseScreen() {
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(320, 300, "PAUSED");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(260, 250, "Press P to Resume");
    drawText(260, 210, "Press ESC to Menu");
}

void drawGameOver() {
    glColor3f(1.0f, 0.0f, 0.0f);
    drawText(300, 350, "GAME OVER");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[100];
    sprintf(buffer, "Final Score: %d", pacman.score);
    drawText(280, 300, buffer);
    
    sprintf(buffer, "Time: %.1f seconds", gameTime);
    drawText(280, 260, buffer);
    
    drawText(250, 200, "Press SPACE to Play Again");
    drawText(260, 160, "Press ESC for Menu");
}

void drawWinScreen() {
    glColor3f(0.0f, 1.0f, 0.0f);
    drawText(300, 350, "YOU WIN!");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[100];
    sprintf(buffer, "Final Score: %d", pacman.score);
    drawText(280, 300, buffer);
    
    sprintf(buffer, "Time: %.1f seconds", gameTime);
    drawText(280, 260, buffer);
    
    if (pacman.score > highScore) {
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(270, 220, "NEW HIGH SCORE!");
    }
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(250, 180, "Press SPACE to Play Again");
    drawText(260, 140, "Press ESC for Menu");
}

void updatePacman() {
    // Try to change direction
    if (canMove(pacman.x, pacman.y, pacman.nextDirection)) {
        pacman.direction = pacman.nextDirection;
    }
    
    // Move in current direction
    if (canMove(pacman.x, pacman.y, pacman.direction)) {
        switch(pacman.direction) {
            case 0: pacman.x += pacman.speed; break;
            case 1: pacman.y -= pacman.speed; break;
            case 2: pacman.x -= pacman.speed; break;
            case 3: pacman.y += pacman.speed; break;
        }
    }
    
    // Wrap around
    if (pacman.x < 0) pacman.x = MAZE_WIDTH - 1;
    if (pacman.x >= MAZE_WIDTH) pacman.x = 0;
    
    // Animate mouth
    pacman.mouthAngle += pacman.mouthOpening * 3;
    if (pacman.mouthAngle >= 45) pacman.mouthOpening = -1;
    if (pacman.mouthAngle <= 0) pacman.mouthOpening = 1;
    
    // Eat dots
    int gridX = (int)(pacman.x + 0.5f);
    int gridY = (int)(pacman.y + 0.5f);
    
    if (maze[gridY][gridX] == 0) {
        maze[gridY][gridX] = 2;
        dotsEaten++;
        pacman.score += 10;
    }
    
    // Check win condition
    if (dotsEaten >= totalDots) {
        currentState = WIN;
        if (pacman.score > highScore) {
            highScore = pacman.score;
        }
    }
}

void updateGhosts() {
    for (int i = 0; i < 4; i++) {
        Ghost &ghost = ghosts[i];
        
        // Simple AI: try to move towards Pacman with some randomness
        if (rand() % 100 < 70) { // 70% chase, 30% random
            float dx = pacman.x - ghost.x;
            float dy = pacman.y - ghost.y;
            
            if (fabs(dx) > fabs(dy)) {
                ghost.direction = (dx > 0) ? 0 : 2;
            } else {
                ghost.direction = (dy > 0) ? 3 : 1;
            }
        } else {
            ghost.direction = rand() % 4;
        }
        
        // Try to move
        if (canMove(ghost.x, ghost.y, ghost.direction)) {
            switch(ghost.direction) {
                case 0: ghost.x += ghost.speed; break;
                case 1: ghost.y -= ghost.speed; break;
                case 2: ghost.x -= ghost.speed; break;
                case 3: ghost.y += ghost.speed; break;
            }
        } else {
            ghost.direction = rand() % 4;
        }
        
        // Check collision with Pacman
        float dist = sqrt(pow(ghost.x - pacman.x, 2) + pow(ghost.y - pacman.y, 2));
        if (dist < 0.5f) {
            pacman.lives--;
            if (pacman.lives <= 0) {
                currentState = GAME_OVER;
            } else {
                // Reset positions
                initPacman();
                initGhosts();
            }
        }
        
        // Increase speed over time
        ghost.speed = 0.08f + gameTime * 0.001f;
        if (ghost.speed > 0.15f) ghost.speed = 0.15f;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (currentState == MENU) {
        drawMenu();
    } else if (currentState == PAUSED) {
        drawMaze();
        drawPacman();
        for (int i = 0; i < 4; i++) drawGhost(ghosts[i]);
        drawHUD();
        drawPauseScreen();
    } else if (currentState == PLAYING) {
        drawMaze();
        drawPacman();
        for (int i = 0; i < 4; i++) drawGhost(ghosts[i]);
        drawHUD();
    } else if (currentState == GAME_OVER) {
        drawGameOver();
    } else if (currentState == WIN) {
        drawWinScreen();
    }
    
    glutSwapBuffers();
}

void update(int value) {
    if (currentState == PLAYING) {
        gameTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - startTime;
        updatePacman();
        updateGhosts();
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27: // ESC
            if (currentState == PLAYING || currentState == PAUSED) {
                currentState = MENU;
            } else if (currentState == GAME_OVER || currentState == WIN) {
                currentState = MENU;
            } else {
                exit(0);
            }
            break;
        case ' ':
            if (currentState == MENU || currentState == GAME_OVER || currentState == WIN) {
                resetGame();
                currentState = PLAYING;
            }
            break;
        case 'p':
        case 'P':
            if (currentState == PLAYING) {
                currentState = PAUSED;
            } else if (currentState == PAUSED) {
                currentState = PLAYING;
                startTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - gameTime;
            }
            break;
        case 'h':
        case 'H':
            if (currentState == MENU) {
                char buffer[100];
                sprintf(buffer, "High Score: %d", highScore);
                printf("%s\n", buffer);
            }
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (currentState == PLAYING) {
        switch(key) {
            case GLUT_KEY_RIGHT: pacman.nextDirection = 0; break;
            case GLUT_KEY_UP: pacman.nextDirection = 1; break;
            case GLUT_KEY_LEFT: pacman.nextDirection = 2; break;
            case GLUT_KEY_DOWN: pacman.nextDirection = 3; break;
        }
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    
    srand(time(NULL));
    resetGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("PAC-MAN - CSE 426 Project");
    
    init();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, update, 0);
    
    glutMainLoop();
    return 0;
}
