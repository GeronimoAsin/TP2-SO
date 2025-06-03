#include <stdint.h>
#include <stddef.h>
#include "videoDriver.h"
#include "pongisGolf.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define PLAYER_SIZE 30
#define PLAYER_SPEED 10
#define BALL_RADIUS 5
#define HOLE_RADIUS 10
#define MAX_LEVELS 5

#define COLOR_PLAYER1 0xFF0000 // rojo
#define COLOR_PLAYER2 0x0000FF // azul
#define COLOR_BALL 0xFFFFFF    // blanco
#define COLOR_HOLE 0x000000    // negro
#define COLOR_BG 0x7FFFD4      // verde agua

extern uint64_t syscallDispatcher(uint64_t id, ...);



Level levels[MAX_LEVELS];
int currentLevel = 0;

// Jugadores y pelota globales
Player p1 = {100, 300, COLOR_PLAYER1, 0, 'w', 's', 'a', 'd'};
Player p2 = {900, 300, COLOR_PLAYER2, 0, 'i', 'k', 'j', 'l'};
Ball ball = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0, 0, COLOR_BALL};
Hole hole = {SCREEN_WIDTH / 2, 100, HOLE_RADIUS, COLOR_HOLE};

int numPlayers = 2;

// Utilidades
void drawPlayer(Player *p) {
   // Cabeza
    drawCircle(p->x + PLAYER_SIZE / 2, p->y + PLAYER_SIZE / 2, PLAYER_SIZE / 2, p->color);

    // Ojos (negros)
    int eye_radius = PLAYER_SIZE / 8;
    int eye_offset_x = PLAYER_SIZE / 4;
    int eye_offset_y = PLAYER_SIZE / 4;
    drawCircle(p->x + eye_offset_x, p->y + eye_offset_y, eye_radius, 0x000000);
    drawCircle(p->x + PLAYER_SIZE - eye_offset_x, p->y + eye_offset_y, eye_radius, 0x000000);

    // Boca (negra)
    int mouth_width = PLAYER_SIZE / 2;
    int mouth_height = PLAYER_SIZE / 8;
    int mouth_x = p->x + PLAYER_SIZE / 4;
    int mouth_y = p->y + (PLAYER_SIZE * 3) / 4;
    drawRectangle(mouth_x, mouth_y, mouth_width, mouth_height, 0x000000);
}

void drawBall(Ball *b) {
    drawCircle(b->x, b->y, BALL_RADIUS, b->color);
}

void drawHole(Hole *h) {
    drawCircle(h->x, h->y, h->radius, h->color);
}

void clear() {
    clearScreen(COLOR_BG);
}

void drawScores() {
    setCursor(10, 10);
    printString("P1: ");
    printChar('0' + p1.score);
    setCursor(10, 40);
    printString("Rojo (WASD)");
    if(numPlayers == 2){
        setCursor(200, 10);
        printString("P2: ");
        printChar('0' + p2.score); 
        setCursor(200, 40);
        printString("Azul (IJKL)");
    }
}

void resetBall() {
    Level *lvl = &levels[currentLevel];
    int found = 0;
    int try_x = SCREEN_WIDTH / 2;
    int try_y = SCREEN_HEIGHT / 2;

    for (int offset = 0; offset < SCREEN_WIDTH && !found; offset += 20) {
        for (int dx = -offset; dx <= offset && !found; dx += 20) {
            for (int dy = -offset; dy <= offset && !found; dy += 20) {
                int x = SCREEN_WIDTH / 2 + dx;
                int y = SCREEN_HEIGHT / 2 + dy;
                // Limita a los bordes
                if (x < BALL_RADIUS) x = BALL_RADIUS;
                if (x > SCREEN_WIDTH - BALL_RADIUS) x = SCREEN_WIDTH - BALL_RADIUS;
                if (y < BALL_RADIUS) y = BALL_RADIUS;
                if (y > SCREEN_HEIGHT - BALL_RADIUS) y = SCREEN_HEIGHT - BALL_RADIUS;

                Ball temp = {x, y, 0, 0, 0, COLOR_BALL};
                found = 1;
                for (int i = 0; i < lvl->numRects; i++)
                    if (ballHitsRect(&temp, &lvl->rects[i])) found = 0;
                for (int i = 0; i < lvl->numCircles; i++)
                    if (ballHitsCircle(&temp, &lvl->circles[i])) found = 0;
                if (found) {
                    try_x = x;
                    try_y = y;
                }
            }
        }
    }

    ball.x = try_x;
    ball.y = try_y;
    ball.dx = 0;
    ball.dy = 0;
    ball.inMotion = 0;
}

void resetPlayer(Player *p, int start_x, int start_y) {
    Level *lvl = &levels[currentLevel];
    int found = 0, try_x, try_y;
    for (int tries = 0; tries < 100 && !found; tries++) {
        try_x = start_x;
        try_y = start_y;
        found = 1;
        for (int i = 0; i < lvl->numRects; i++)
            if (playerHitsRect(p, &lvl->rects[i])) found = 0;
        for (int i = 0; i < lvl->numCircles; i++)
            if (playerHitsCircle(p, &lvl->circles[i])) found = 0;
    }
    p->x = try_x;
    p->y = try_y;
}

int isInHole(Ball *b, Hole *h) {
    int dx = b->x - h->x;
    int dy = b->y - h->y;
    return (dx * dx + dy * dy <= h->radius * h->radius);
}

static int my_abs(int x) {
    return x < 0 ? -x : x;
}


// Devuelve 1 si el jugador y la pelota colisionan
int playerHitsBall(Player *p, Ball *b) {
    int player_cx = p->x + PLAYER_SIZE / 2;
    int player_cy = p->y + PLAYER_SIZE / 2;
    int dx = b->x - player_cx;
    int dy = b->y - player_cy;
    int distance2 = dx * dx + dy * dy;
    int minDist = (PLAYER_SIZE / 2) + BALL_RADIUS;
    return distance2 <= (minDist * minDist);
}

Player *lastHitter = NULL;

void movePlayer(Player *p, char key) {
    int new_x = p->x;
    int new_y = p->y;

    if (key == p->up && p->y > 0)
        new_y -= PLAYER_SPEED;
    else if (key == p->down && p->y + PLAYER_SIZE < SCREEN_HEIGHT)
        new_y += PLAYER_SPEED;
    else if (key == p->left && p->x > 0)
        new_x -= PLAYER_SPEED;
    else if (key == p->right && p->x + PLAYER_SIZE < SCREEN_WIDTH)
        new_x += PLAYER_SPEED;

    // Chequeo colisión con obstáculos
    Level *lvl = &levels[currentLevel];
    Player temp = *p;
    temp.x = new_x;
    temp.y = new_y;
    for (int i = 0; i < lvl->numRects; i++){
        if (playerHitsRect(&temp, &lvl->rects[i])) return;
    }
    for (int i = 0; i < lvl->numCircles; i++){
        if (playerHitsCircle(&temp, &lvl->circles[i])) return;
    }
    if (ball.inMotion) {
        int player_cx = new_x + PLAYER_SIZE / 2;
        int player_cy = new_y + PLAYER_SIZE / 2;
        int dx = ball.x - player_cx;
        int dy = ball.y - player_cy;
        int distance2 = dx * dx + dy * dy;
        int minDist = (PLAYER_SIZE / 2) + BALL_RADIUS;
        if (distance2 <= (minDist * minDist)) {
            return;
        }
    }

    p->x = new_x;
    p->y = new_y;

    // HIT siempre activo
    if (!ball.inMotion && playerHitsBall(p, &ball)) {
        int player_cx = p->x + PLAYER_SIZE / 2;
        int player_cy = p->y + PLAYER_SIZE / 2;
        int dx = ball.x - player_cx;
        int dy = ball.y - player_cy;

        int mag2 = dx*dx + dy*dy;
        if (mag2 == 0) {
            dx = 0;
            dy = -1;
        }
        int abs_dx = my_abs(dx);
        int abs_dy = my_abs(dy);
        int max_abs = abs_dx > abs_dy ? abs_dx : abs_dy;
        if (max_abs == 0){
            max_abs = 1;
        }
        int speed = 6;
        ball.dx = dx * speed / max_abs;
        ball.dy = dy * speed / max_abs;
        ball.inMotion = 1;
        lastHitter = p;
    }
}


void moveBall() {
    if (!ball.inMotion) return;
    int next_x = ball.x + ball.dx;
    int next_y = ball.y + ball.dy;
    Level *lvl = &levels[currentLevel];

    // Rectángulos
    for (int i = 0; i < lvl->numRects; i++) {
        Ball temp = {next_x, next_y, 0, 0, 0, COLOR_BALL};
        if (ballHitsRect(&temp, &lvl->rects[i])) {
            // Rebote simple
            if (ballHitsRect(&(Ball){ball.x, next_y, 0, 0, 0, COLOR_BALL}, &lvl->rects[i]))
                ball.dy = -ball.dy;
            if (ballHitsRect(&(Ball){next_x, ball.y, 0, 0, 0, COLOR_BALL}, &lvl->rects[i]))
                ball.dx = -ball.dx;
            return;
        }
    }
    // Círculos
    for (int i = 0; i < lvl->numCircles; i++) {
        Ball temp = {next_x, next_y, 0, 0, 0, COLOR_BALL};
        if (ballHitsCircle(&temp, &lvl->circles[i])) {
            ball.dx = -ball.dx;
            ball.dy = -ball.dy;
            return;
        }
    }

    // Movimiento normal si no hay colisión
    ball.x = next_x;
    ball.y = next_y;

    // Bordes de pantalla
    if (ball.x - BALL_RADIUS < 0) {
        ball.x = BALL_RADIUS;
        ball.dx = -ball.dx;
    }
    if (ball.x + BALL_RADIUS > SCREEN_WIDTH) {
        ball.x = SCREEN_WIDTH - BALL_RADIUS;
        ball.dx = -ball.dx;
    }
    if (ball.y - BALL_RADIUS < 0) {
        ball.y = BALL_RADIUS;
        ball.dy = -ball.dy;
    }
    if (ball.y + BALL_RADIUS > SCREEN_HEIGHT) {
        ball.y = SCREEN_HEIGHT - BALL_RADIUS;
        ball.dy = -ball.dy;
    }

    ball.dx *= 0.95;
    ball.dy *= 0.95;

    if (my_abs(ball.dx) < 1 && my_abs(ball.dy) < 1) {
        ball.inMotion = 0;
    }
}

char getCharFromKeyboard() {
    char c;
    syscallDispatcher(0, 0, &c, 1, 0);
    return c;
}

void writeString(const char *str) {
    uint64_t len = 0;
    while (str[len]) len++;
    syscallDispatcher(1, 1, str, len, 0); // sys_write(1, str, len)
}

void selectPlayers() {
    clearScreen(COLOR_BG);
    setCursor(100, 200);
    printString("Presione '1' para 1 jugador o '2' para 2 jugadores:");
    char c = 0;
    while (c != '1' && c != '2') {
        c = getCharFromKeyboard();
    }
    numPlayers = (c == '1') ? 1 : 2;
}

void drawObstacles() {
    Level *lvl = &levels[currentLevel];
    for (int i = 0; i < lvl->numRects; i++) {
        ObstacleRect *r = &lvl->rects[i];
        drawRectangle(r->x, r->y, r->width, r->height, r->color);
    }
    for (int i = 0; i < lvl->numCircles; i++) {
        ObstacleCircle *c = &lvl->circles[i];
        drawCircle(c->x, c->y, c->radius, c->color);
    }
}

int ballHitsRect(Ball *b, ObstacleRect *r) {
    int closestX = b->x < r->x ? r->x : (b->x > r->x + r->width ? r->x + r->width : b->x);
    int closestY = b->y < r->y ? r->y : (b->y > r->y + r->height ? r->y + r->height : b->y);
    int dx = b->x - closestX;
    int dy = b->y - closestY;
    return (dx * dx + dy * dy) <= (BALL_RADIUS * BALL_RADIUS);
}

int ballHitsCircle(Ball *b, ObstacleCircle *c) {
    int dx = b->x - c->x;
    int dy = b->y - c->y;
    int minDist = BALL_RADIUS + c->radius;
    return (dx * dx + dy * dy) <= (minDist * minDist);
}

int playerHitsRect(Player *p, ObstacleRect *r) {
    int px1 = p->x, px2 = p->x + PLAYER_SIZE;
    int py1 = p->y, py2 = p->y + PLAYER_SIZE;
    int rx1 = r->x, rx2 = r->x + r->width;
    int ry1 = r->y, ry2 = r->y + r->height;
    return !(px2 < rx1 || px1 > rx2 || py2 < ry1 || py1 > ry2);
}

int playerHitsCircle(Player *p, ObstacleCircle *c) {
    int cx = p->x + PLAYER_SIZE / 2;
    int cy = p->y + PLAYER_SIZE / 2;
    int dx = cx - c->x;
    int dy = cy - c->y;
    int minDist = PLAYER_SIZE / 2 + c->radius;
    return (dx * dx + dy * dy) <= (minDist * minDist);
}

void initLevels() {
    // Nivel 1
    levels[0].hole_x = SCREEN_WIDTH / 2;
    levels[0].hole_y = 100;
    levels[0].numRects = 0;
    levels[0].numCircles = 0;

    // Nivel 2
    levels[1].hole_x = SCREEN_WIDTH / 4;
    levels[1].hole_y = 150;
    levels[1].numRects = 1;
    levels[1].rects[0] = (ObstacleRect){SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 20, 100, 40, 0x964B00};
    levels[1].numCircles = 0;

    // Nivel 3
    levels[2].hole_x = SCREEN_WIDTH * 3 / 4;
    levels[2].hole_y = 200;
    levels[2].numRects = 0;
    levels[2].numCircles = 1;
    levels[2].circles[0] = (ObstacleCircle){SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 30, 0x228B22};

    // Nivel 4
    levels[3].hole_x = SCREEN_WIDTH / 2;
    levels[3].hole_y = SCREEN_HEIGHT - 100;
    levels[3].numRects = 2;
    levels[3].rects[0] = (ObstacleRect){200, 300, 80, 30, 0x964B00};
    levels[3].rects[1] = (ObstacleRect){700, 400, 60, 60, 0x964B00};
    levels[3].numCircles = 1;
    levels[3].circles[0] = (ObstacleCircle){SCREEN_WIDTH/2, 500, 40, 0x228B22};

    // Nivel 5
    levels[4].hole_x = 100;
    levels[4].hole_y = 100;
    levels[4].numRects = 2;
    levels[4].rects[0] = (ObstacleRect){400, 200, 120, 30, 0x964B00};
    levels[4].rects[1] = (ObstacleRect){600, 600, 80, 80, 0x964B00};
    levels[4].numCircles = 2;
    levels[4].circles[0] = (ObstacleCircle){300, 400, 30, 0x228B22};
    levels[4].circles[1] = (ObstacleCircle){800, 300, 50, 0x228B22};
}

int nextLevel() {
    currentLevel++;
    if (currentLevel >= MAX_LEVELS) {
        return 0; // No hay más niveles
    }
    hole.x = levels[currentLevel].hole_x;
    hole.y = levels[currentLevel].hole_y;
    resetBall();
    resetPlayer(&p1, 100, 300);
    if (numPlayers == 2){
        resetPlayer(&p2, 900, 300);
    } 
    resetBall();
    return 1;
}

void showWinner() {
    clearScreen(COLOR_BG);
    setCursor(400, 300);
    if (p1.score > p2.score) {
        printString("Winner: player 1");
    } else if (p2.score > p1.score) {
        printString("Winner: player 2");
    } else {
        printString("Empate!");
    }
    for (int i = 0; i < 500000000; i++);
}

void pongisGolfMain() {
    initLevels();
    currentLevel = 0;
    hole.x = levels[currentLevel].hole_x;
    hole.y = levels[currentLevel].hole_y;
    selectPlayers(); 
    clear();
    drawScores();
    setCursor(100, 250);
    printString("Presiona cualquier tecla para comenzar");

    while (1) {
        char key = getCharFromKeyboard();

        if (key) {
            movePlayer(&p1, key);
            if (numPlayers == 2) {
                movePlayer(&p2, key);
            }
        }

        moveBall();

        if (isInHole(&ball, &hole)) {
            if (lastHitter != NULL) {
                lastHitter->score++; // Solo cuenta niveles ganados
            }
            lastHitter = NULL; 
            if (!nextLevel()) {
                showWinner();
                clearScreen(0x000000);
                break; // Sale del juego
            }
        }

        clear();
        drawHole(&hole);
        drawObstacles();
        drawPlayer(&p1);
        if (numPlayers == 2) {
            drawPlayer(&p2);
        }
        drawBall(&ball);
        drawScores();

        //sys_sleep(15);
    }
}