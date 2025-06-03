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


void drawPlayer(struct Player *p);
void drawBall(struct Ball *b);
void drawHole(struct Hole *h);
void clear();
void drawScores();
void resetBall();
void resetPlayer(Player *p, int start_x, int start_y);
int isInHole(struct Ball *b, struct Hole *h);
void movePlayer(struct Player *p, char key);
void moveBall();
char getCharFromKeyboard();
void writeString(const char *str);
void selectPlayers();
void updateKeyStates();
void processPlayersInput();
void drawObstacles();
void initLevels();
int nextLevel();
void showWinner(); 
int ballHitsRect(Ball *b, ObstacleRect *r);
int ballHitsCircle(Ball *b, ObstacleCircle *c);
int playerHitsRect(Player *p, ObstacleRect *r);
int playerHitsCircle(Player *p, ObstacleCircle *c);

#endif 