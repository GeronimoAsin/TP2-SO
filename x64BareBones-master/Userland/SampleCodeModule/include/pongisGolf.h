#ifndef PONGISGOLF_H
#define PONGISGOLF_H
#define MAX_OBSTACLES 10
#include <stdint.h>

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

typedef struct ObstacleRect {
    int x, y, width, height, color;
} ObstacleRect;

typedef struct ObstacleCircle {
    int x, y, radius, color;
} ObstacleCircle;

typedef struct Level{
    int hole_x, hole_y;
    int numRects;
    ObstacleRect rects[MAX_OBSTACLES];
    int numCircles;
    ObstacleCircle circles[MAX_OBSTACLES];
} Level;

int getWidth();
int getHeight();
void drawCircle(int x, int y, int radius, uint32_t color);
void drawRectangle(int x, int y, int width, int height, uint32_t color);
void clearScreen(uint32_t color);
void setCursor(int x, int y);
void hideCursor();
void gameObjects();
void drawPlayer(Player *p);
void drawScoreArea();
void drawScores();
void clearPlayer(int x, int y);
void drawBall(Ball *b);
void clearBall(int x, int y);
void drawHole(Hole *h);
void clearHole(int x, int y);
void clear();
void resetBall();
void resetPlayer(Player *p, int start_x, int start_y);
void GameObjects();
int isInHole(Ball *b, Hole *h);
static int my_abs(int x);
int playerHitsBall(Player *p, Ball *b);
int playersCollide(Player *p1, Player *p2);
void movePlayerOptimized(Player *p, char key, int *prev_x, int *prev_y);
void moveBallOptimized();
char getCharFromKeyboard();
void writeString(const char *str);
void selectPlayers();
void drawObstacles();
void redrawObstaclesInArea(int x, int y, int width, int height);
int ballHitsRect(Ball *b, ObstacleRect *r);
int ballHitsCircle(Ball *b, ObstacleCircle *c);
int playerHitsRect(Player *p, ObstacleRect *r);
int playerHitsCircle(Player *p, ObstacleCircle *c);
void initLevels();
int nextLevel();
void showWinner();
void pongisGolfMain();

#endif 