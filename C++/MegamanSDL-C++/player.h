#ifndef PLAYER_H
#define PLAYER_H

const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
const int BOUNCER_SIZE = 32;
const int MAX_BULLETS = 512;
const int MAX_PLAYERS = 2;
enum MYKEYS
{
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, SPACE
};

typedef struct bullet_t
{
    char b_pos;
    float b_x,b_y;
} Bullet;

typedef struct player
{
    char p_pos;
    int bullets;
    float p_x, p_y;
    long p_hp;
    Bullet bullet_a[MAX_BULLETS];
} Player;

typedef struct boss
{
    char e_pos;
    float e_x, e_y;
    long e_hp;
    Bullet bullet_a[MAX_BULLETS];
} Bauce;

int initializeGame();

int startGame();

int endGame();
#endif
