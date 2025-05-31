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

#define COLOR_PLAYER1 0xFF0000 // rojo
#define COLOR_PLAYER2 0x0000FF // azul
#define COLOR_BALL 0xFFFFFF    // blanco
#define COLOR_HOLE 0x000000    // negro
#define COLOR_BG 0x7FFFD4      // verde agua

extern uint64_t syscallDispatcher(uint64_t id, ...);

typedef struct Player{
    int x, y;
    uint32_t color;
    int score;
    char up, down, left, right;
} Player;

typedef struct Ball{
    int x, y;
    int dx, dy;
    int inMotion;
    uint32_t color;
} Ball;

typedef struct Hole{
    int x, y;
    int radius;
    uint32_t color;
} Hole;

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
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    ball.dx = 0;
    ball.dy = 0;
    ball.inMotion = 0;
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

    if (key == p->up && p->y > 0){
        new_y -= PLAYER_SPEED;
    }else if (key == p->down && p->y + PLAYER_SIZE < SCREEN_HEIGHT){
        new_y += PLAYER_SPEED;
    }else if (key == p->left && p->x > 0){
        new_x -= PLAYER_SPEED;
    }else if (key == p->right && p->x + PLAYER_SIZE < SCREEN_WIDTH){
        new_x += PLAYER_SPEED;
    }

    if (ball.inMotion) {
        int player_cx = new_x + PLAYER_SIZE / 2;
        int player_cy = new_y + PLAYER_SIZE / 2;
        int dx = ball.x - player_cx;
        int dy = ball.y - player_cy;
        int distance2 = dx * dx + dy * dy;
        int minDist = (PLAYER_SIZE / 2) + BALL_RADIUS;
        if (distance2 <= (minDist * minDist)) {
            // Si colisiona, no actualiza la posición
            return;
        }
    }

    p->x = new_x;
    p->y = new_y;

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
        //sys_beep(900, 150);
    }
}


void moveBall() {
    if (!ball.inMotion) return;
    ball.x += ball.dx;
    ball.y += ball.dy;

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

void pongisGolfMain() {
    selectPlayers(); 
    clear();
    drawScores();

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
            //sys_beep(300, 400);
            if (lastHitter != NULL) {
                lastHitter->score++;
            }
            resetBall();
            lastHitter = NULL; 
        }

        clear();
        drawHole(&hole);
        drawPlayer(&p1);
        if (numPlayers == 2) {
            drawPlayer(&p2);
        }
        drawBall(&ball);
        drawScores();

        //sys_sleep(15);
    }
}
