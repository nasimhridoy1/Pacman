

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
#define M_PI 3.14159265358979323846 // Define PI if not available via <math.h>

// Game states enumeration
enum GameState { MENU, PLAYING, PAUSED, GAME_OVER, WIN };
GameState currentState = MENU;


// Pacman structure definition
struct Pacman {
    float x, y;          // Position in maze coordinates (e.g., 14.5, 23.0)
    float speed;         // Movement speed multiplier
    int direction;       // Current direction: 0=right, 1=up, 2=left, 3=down
    int nextDirection;   // Direction requested by the user, for smooth turning
    float mouthAngle;    // Current angle for mouth animation (0 to 45 degrees)
    int mouthOpening;    // 1 for opening, -1 for closing
    int lives;           // Remaining lives
    int score;           // Current score
} pacman;

// Ghost structure definition
struct Ghost {
    float x, y;          // Position in maze coordinates
    float speed;         // Movement speed multiplier
    int direction;       // Current direction
    int color;           // 0=red (Blinky), 1=pink (Pinky), 2=cyan (Inky), 3=orange (Clyde)
    float stateTimer;    // Timer for scatter/chase/frightened states (not fully implemented in AI, but kept)
} ghosts[4];



// Maze grid array: 1=wall, 0=path with dot, 2=empty path, 3=ghost house
int maze[MAZE_HEIGHT][MAZE_WIDTH];
int totalDots = 0;   // Total dots in the maze at the start
int dotsEaten = 0;   // Number of dots currently eaten
float gameTime = 0;  // Total time played in the current game (seconds)
float startTime = 0; // Time when the game started or unpaused
int highScore = 0;   // High score tracking


// Initializes the maze layout from a hardcoded array
void initMaze() {
    // The Pac-Man classic maze structure
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
        {2,2,2,2,2,2,0,2,2,2,1,3,3,3,3,3,3,1,2,2,2,0,2,2,2,2,2,2}, // Warp tunnel row (value 2)
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
            // Count total dots available for the win condition
            if (maze[i][j] == 0) totalDots++;
        }
    }
}

// Sets initial properties for Pacman
void initPacman() {
    pacman.x = 13.5f; // Center Pacman initially (13 + 0.5)
    pacman.y = 23.5f; // Center Pacman initially (23 + 0.5)
    pacman.speed = 0.1f;
    pacman.direction = 0;       // Start facing right
    pacman.nextDirection = 0;
    pacman.mouthAngle = 0;
    pacman.mouthOpening = 1;
    pacman.lives = 3;
    pacman.score = 0;
}

// Sets initial properties and positions for the four ghosts
void initGhosts() {
    // Red ghost (Blinky) - starting outside the house
    ghosts[0].x = 13.5f; ghosts[0].y = 11.5f; ghosts[0].color = 0; ghosts[0].speed = 0.08f;
    // Pink ghost (Pinky) - inside the house
    ghosts[1].x = 14.5f; ghosts[1].y = 14.5f; ghosts[1].color = 1; ghosts[1].speed = 0.08f;
    // Cyan ghost (Inky) - inside the house
    ghosts[2].x = 13.5f; ghosts[2].y = 14.5f; ghosts[2].color = 2; ghosts[2].speed = 0.08f;
    // Orange ghost (Clyde) - inside the house
    ghosts[3].x = 15.5f; ghosts[3].y = 14.5f; ghosts[3].color = 3; ghosts[3].speed = 0.08f;

    for (int i = 0; i < 4; i++) {
        ghosts[i].direction = rand() % 4; // Start with a random direction
        ghosts[i].stateTimer = 0;
    }
}

// Resets all game variables and states for a new game
void resetGame() {
    initMaze();
    initPacman();
    initGhosts();
    dotsEaten = 0;
    gameTime = 0;
    // Record the current GLUT time to calculate elapsed game time
    startTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}


// Checks if a given grid coordinate (x, y) contains a wall (1)
bool isWall(int x, int y) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT)
        return true; // Treat out-of-bounds as walls
    return maze[y][x] == 1;
}

// Checks if a movement in a given direction is possible without hitting a wall
bool canMove(float x, float y, int dir) {
    float nextX = x, nextY = y;
    float offset = 0.4f; // A small offset to check slightly ahead of the center

    // Calculate the potential future position based on direction
    switch(dir) {
        case 0: nextX += offset; break; // right
        case 1: nextY -= offset; break; // up (Y-axis is reversed in OpenGL ortho)
        case 2: nextX -= offset; break; // left
        case 3: nextY += offset; break; // down
    }

    // Convert floating point position to integer grid coordinates
    int gridX = (int)(nextX + 0.5f);
    int gridY = (int)(nextY + 0.5f);

    // A warp area (value 2) is considered a path, and ghost house (3) is path for ghosts
    return maze[gridY][gridX] != 1;
}


// Draws a filled circle using GL_TRIANGLE_FAN
void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy); // Center of the circle
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
    }
    glEnd();
}

// Draws Pacman with animated mouth, rotated based on direction
void drawPacman() {
    // Save current transformation matrix
    glPushMatrix();

    // Translate to Pacman's current grid position
    glTranslatef(pacman.x * CELL_SIZE + CELL_SIZE/2,
                 pacman.y * CELL_SIZE + CELL_SIZE/2, 0);

    // Rotate Pacman based on its current direction (1=up, 2=left, 3=down, 0=right (no rotation))
    glRotatef(-pacman.direction * 90, 0, 0, 1);

    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for Pacman

    // Calculate mouth opening angles in radians
    float startAngle = pacman.mouthAngle * M_PI / 180.0f;
    float endAngle = (360 - pacman.mouthAngle) * M_PI / 180.0f;

    // Draw Pacman as a partial circle (pie slice) using GL_TRIANGLE_FAN
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0); // Center point
    // Loop through the angle range to draw the body, skipping the mouth
    for (float angle = startAngle; angle <= endAngle; angle += 0.1f) {
        glVertex2f(cos(angle) * 8, sin(angle) * 8); // 8 is the radius
    }
    glEnd();

    // Restore previous transformation matrix
    glPopMatrix();
}

// Draws a single ghost figure
void drawGhost(Ghost &ghost) {
    float x = ghost.x * CELL_SIZE + CELL_SIZE/2;
    float y = ghost.y * CELL_SIZE + CELL_SIZE/2;

    // Set ghost color based on its type
    switch(ghost.color) {
        case 0: glColor3f(1.0f, 0.0f, 0.0f); break;    // Red (Blinky)
        case 1: glColor3f(1.0f, 0.75f, 0.8f); break;  // Pink (Pinky)
        case 2: glColor3f(0.0f, 1.0f, 1.0f); break;   // Cyan (Inky)
        case 3: glColor3f(1.0f, 0.65f, 0.0f); break;  // Orange (Clyde)
    }

    // --- Body (Top Semi-circle) ---
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center point
    for (int i = 0; i <= 180; i += 10) {
        float angle = i * M_PI / 180.0f;
        // Drawing points from 0 to 180 degrees (top half)
        glVertex2f(x + cos(angle) * 8, y + sin(angle) * 8);
    }
    glEnd();

    // --- Bottom Wavy Part (Basic Wavy Skirt Shape) ---
    // Note: This draws a simplified skirt shape relative to the center (x, y)
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center for reference

    // Draw a zigzag line at the bottom
    glVertex2f(x - 8, y);
    glVertex2f(x - 6, y + 4);
    glVertex2f(x - 3, y);
    glVertex2f(x, y + 4);
    glVertex2f(x + 3, y);
    glVertex2f(x + 6, y + 4);
    glVertex2f(x + 8, y);
    glEnd();

    // --- Eyes (White part) ---
    glColor3f(1.0f, 1.0f, 1.0f); // White
    drawCircle(x - 3, y - 2, 2, 16); // Left eye
    drawCircle(x + 3, y - 2, 2, 16); // Right eye

    // --- Pupils (Blue part) ---
    glColor3f(0.0f, 0.0f, 1.0f); // Blue
    drawCircle(x - 3, y - 2, 1, 16); // Left pupil
    drawCircle(x + 3, y - 2, 1, 16); // Right pupil
}

// Draws the maze walls and dots
void drawMaze() {
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            float x = j * CELL_SIZE;
            float y = i * CELL_SIZE;

            if (maze[i][j] == 1) {
                // Wall: Draw a solid blue square
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_QUADS);
                glVertex2f(x, y);
                glVertex2f(x + CELL_SIZE, y);
                glVertex2f(x + CELL_SIZE, y + CELL_SIZE);
                glVertex2f(x, y + CELL_SIZE);
                glEnd();
            } else if (maze[i][j] == 0) {
                // Dot: Draw a small white circle
                glColor3f(1.0f, 1.0f, 1.0f);
                drawCircle(x + CELL_SIZE/2, y + CELL_SIZE/2, 2, 8);
            }
            // Note: maze[i][j] == 2 (empty path) and 3 (ghost house) are drawn as black background
        }
    }
}

// Utility function to draw text on the screen
void drawText(float x, float y, const char* text) {
    // Set the starting position for the text raster
    glRasterPos2f(x, y);
    // Loop through the string and draw each character
    for (const char* c = text; *c != '\0'; c++) {
        // Use a standard GLUT bitmap font
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Draws the Heads-Up Display (Score, Lives, Time)
void drawHUD() {
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    char buffer[100];

    // Display Score
    sprintf(buffer, "Score: %d", pacman.score);
    drawText(520, 550, buffer);

    // Display Lives
    sprintf(buffer, "Lives: %d", pacman.lives);
    drawText(520, 520, buffer);

    // Display Game Time
    sprintf(buffer, "Time: %.1f", gameTime);
    drawText(520, 490, buffer);

    // Display Dots Eaten vs Total
    sprintf(buffer, "Dots: %d/%d", dotsEaten, totalDots);
    drawText(520, 460, buffer);
}

// Draws the game main menu screen
void drawMenu() {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow Pac-Man Title
    drawText(300, 400, "PAC-MAN");

    glColor3f(1.0f, 1.0f, 1.0f); // White instructions
    drawText(280, 320, "Press SPACE to Start");
    drawText(280, 280, "Press H for High Score");
    drawText(280, 240, "Press ESC to Exit");
    drawText(250, 180, "Controls: Arrow Keys");
    drawText(250, 140, "Press P to Pause");
}

// Draws the pause screen overlay
void drawPauseScreen() {
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(320, 300, "PAUSED");

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(260, 250, "Press P to Resume");
    drawText(260, 210, "Press ESC to Menu");
}

// Draws the game over screen
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

// Draws the win screen
void drawWinScreen() {
    glColor3f(0.0f, 1.0f, 0.0f);
    drawText(300, 350, "YOU WIN!");

    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[100];
    sprintf(buffer, "Final Score: %d", pacman.score);
    drawText(280, 300, buffer);

    sprintf(buffer, "Time: %.1f seconds", gameTime);
    drawText(280, 260, buffer);

    // Highlight if a new high score was achieved
    if (pacman.score > highScore) {
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(270, 220, "NEW HIGH SCORE!");
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(250, 180, "Press SPACE to Play Again");
    drawText(260, 140, "Press ESC for Menu");
}


// Updates Pacman's position, animation, and handles dot eating/wrapping
void updatePacman() {
    // 1. Direction Change Logic: Check if the queued direction is valid for a turn
    if (canMove(pacman.x, pacman.y, pacman.nextDirection)) {
        pacman.direction = pacman.nextDirection;
    }

    // 2. Movement Logic: Move only if the current direction is valid
    if (canMove(pacman.x, pacman.y, pacman.direction)) {
        switch(pacman.direction) {
            case 0: pacman.x += pacman.speed; break; // Right
            case 1: pacman.y -= pacman.speed; break; // Up (decreasing Y)
            case 2: pacman.x -= pacman.speed; break; // Left
            case 3: pacman.y += pacman.speed; break; // Down (increasing Y)
        }
    }

    // 3. Wrap Around Logic (for the tunnel)
    if (pacman.x < 0) pacman.x = MAZE_WIDTH - 1;
    if (pacman.x >= MAZE_WIDTH) pacman.x = 0;

    // 4. Mouth Animation Logic
    pacman.mouthAngle += pacman.mouthOpening * 3;
    if (pacman.mouthAngle >= 45) pacman.mouthOpening = -1; // Start closing
    if (pacman.mouthAngle <= 0) pacman.mouthOpening = 1;  // Start opening

    // 5. Dot Eating Logic: Check the grid cell Pacman is currently centered in
    int gridX = (int)(pacman.x + 0.5f);
    int gridY = (int)(pacman.y + 0.5f);

    if (maze[gridY][gridX] == 0) {
        maze[gridY][gridX] = 2; // Mark cell as empty path
        dotsEaten++;
        pacman.score += 10;
    }

    // 6. Check Win Condition
    if (dotsEaten >= totalDots) {
        currentState = WIN;
        // Update high score if current score is higher
        if (pacman.score > highScore) {
            highScore = pacman.score;
        }
    }
}

// Updates ghost positions and handles simple AI, speed increase, and collisions
void updateGhosts() {
    for (int i = 0; i < 4; i++) {
        Ghost &ghost = ghosts[i];

        // --- Simple Ghost AI (70% chase, 30% random) ---
        if (rand() % 100 < 70) {
            // Chase logic: Determine preferred direction (horizontal or vertical)
            float dx = pacman.x - ghost.x;
            float dy = pacman.y - ghost.y;

            if (fabs(dx) > fabs(dy)) {
                // Prioritize horizontal movement
                ghost.direction = (dx > 0) ? 0 : 2; // Right or Left
            } else {
                // Prioritize vertical movement
                ghost.direction = (dy > 0) ? 3 : 1; // Down or Up
            }
        } else {
            // Random movement (30% of the time)
            ghost.direction = rand() % 4;
        }

        // --- Ghost Movement ---
        // Only move if the chosen direction is valid (not a wall)
        if (canMove(ghost.x, ghost.y, ghost.direction)) {
            switch(ghost.direction) {
                case 0: ghost.x += ghost.speed; break;
                case 1: ghost.y -= ghost.speed; break;
                case 2: ghost.x -= ghost.speed; break;
                case 3: ghost.y += ghost.speed; break;
            }
        } else {
            // If hitting a wall, choose a new random direction
            ghost.direction = rand() % 4;
        }

        // --- Collision Check (Ghost vs. Pacman) ---
        // Calculate distance between centers
        float dist = sqrt(pow(ghost.x - pacman.x, 2) + pow(ghost.y - pacman.y, 2));
        if (dist < 0.5f) { // Collision radius check
            pacman.lives--;
            if (pacman.lives <= 0) {
                currentState = GAME_OVER;
            } else {
                // Reset positions if Pacman loses a life
                initPacman();
                initGhosts();
            }
        }

        // --- Speed Increase over Time ---
        // Increase speed slightly based on elapsed game time
        ghost.speed = 0.08f + gameTime * 0.001f;
        if (ghost.speed > 0.15f) ghost.speed = 0.15f; // Cap max speed
    }
}

// =================================================================
// GLUT CALLBACKS
// =================================================================

// GLUT Display Callback: Called when the window needs to be redrawn
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with the background color (black)

    // Draw elements based on the current game state
    if (currentState == MENU) {
        drawMenu();
    } else if (currentState == PAUSED) {
        // Draw game elements and then overlay the pause screen
        drawMaze();
        drawPacman();
        for (int i = 0; i < 4; i++) drawGhost(ghosts[i]);
        drawHUD();
        drawPauseScreen();
    } else if (currentState == PLAYING) {
        // Draw the main game scene
        drawMaze();
        drawPacman();
        for (int i = 0; i < 4; i++) drawGhost(ghosts[i]);
        drawHUD();
    } else if (currentState == GAME_OVER) {
        drawGameOver();
    } else if (currentState == WIN) {
        drawWinScreen();
    }

    glutSwapBuffers(); // Swap the front and back buffers to display the scene
}


void update(int value) {
    if (currentState == PLAYING) {
        // Calculate elapsed game time
        gameTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - startTime;

        // Update game components
        updatePacman();
        updateGhosts();
    }

    glutPostRedisplay(); // Request a redraw
    glutTimerFunc(16, update, 0); // Schedule the next call after 16 milliseconds (1000/60)
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27: // ESC key
            if (currentState == PLAYING || currentState == PAUSED || currentState == GAME_OVER || currentState == WIN) {
                currentState = MENU; // Go back to menu from game/end screens
            } else {
                exit(0);
            }
            break;
        case ' ': // SPACE key
            if (currentState == MENU || currentState == GAME_OVER || currentState == WIN) {
                resetGame();
                currentState = PLAYING; // Start or restart the game
            }
            break;
        case 'p':
        case 'P': // P key (Pause/Resume)
            if (currentState == PLAYING) {
                currentState = PAUSED;
            } else if (currentState == PAUSED) {
                currentState = PLAYING;
                // Re-sync startTime to account for the paused duration
                startTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - gameTime;
            }
            break;
        case 'h':
        case 'H': // H key (High Score - Console Output)
            if (currentState == MENU) {
                char buffer[100];
                sprintf(buffer, "High Score: %d", highScore);
                printf("%s\n", buffer);
            }
            break;
    }
    glutPostRedisplay();
}

// GLUT Special Keys Callback: Handles arrow keys
void specialKeys(int key, int x, int y) {
    if (currentState == PLAYING) {
        // Queue the next direction change
        switch(key) {
            case GLUT_KEY_RIGHT: pacman.nextDirection = 0; break;
            case GLUT_KEY_UP: pacman.nextDirection = 1; break;
            case GLUT_KEY_LEFT: pacman.nextDirection = 2; break;
            case GLUT_KEY_DOWN: pacman.nextDirection = 3; break;
        }
    }
}

// OpenGL initialization and setup
void init() {
    // Set the background color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Set up the projection matrix (2D orthographic view)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define the 2D viewing volume (0, 0) is top-left, (WINDOW_WIDTH, WINDOW_HEIGHT) is bottom-right
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    // Switch back to the modelview matrix for drawing
    glMatrixMode(GL_MODELVIEW);

    // Seed the random number generator
    srand(time(NULL));

    // Initialize game state (Pacman, Ghosts, Maze)
    resetGame();
}

// Main function: Entry point of the GLUT application
int main(int argc, char** argv) {
    glutInit(&argc, argv); // Initialize GLUT

    // Set display mode: Double buffering (for smooth animation) and RGB color
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("PAC-MAN - CSE 426 Project");

    init(); // Custom initialization function

    // Register callback functions
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys); // Register handler for arrow keys
    glutTimerFunc(0, update, 0); // Start the game loop timer

    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}
