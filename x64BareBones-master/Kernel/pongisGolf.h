#ifndef PONGISGOLF_H
#define PONGISGOLF_H
#include <stdint.h>

typedef struct Player;
typedef struct Ball;
typedef struct Hole;


void drawPlayer(struct Player *p);
void drawBall(struct Ball *b);
void drawHole(struct Hole *h);
void clear();
void drawScores();
void resetBall();
int isInHole(struct Ball *b, struct Hole *h);
void movePlayer(struct Player *p, char key);
void moveBall();
char getCharFromKeyboard();
void writeString(const char *str);
void selectPlayers();
void updateKeyStates();
void processPlayersInput();

#endif 