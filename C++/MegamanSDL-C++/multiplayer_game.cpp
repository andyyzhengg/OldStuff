#undef UNICODE
#include "player.h"
#include "network.h"
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <limits>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using namespace std;

const int sizeof_player = sizeof(Player);
WSADATA wsaData;
int iResult;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;
SOCKET ConnectSocket = INVALID_SOCKET;

struct addrinfo *ptr = NULL,hints;
struct addrinfo *result = NULL;

int iSendResult;
char s[DEFAULT_BUFLEN];
char recvbuf[DEFAULT_BUFLEN];
char playervbuf[sizeof_player];
int recvbuflen = DEFAULT_BUFLEN;
char *sendthis;
int current_bullets;
int previous_bullets;

char ip_address[256];
bool server = false;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *p_r = NULL;
ALLEGRO_BITMAP *p_shoot_r = NULL;
ALLEGRO_BITMAP *p_l = NULL;
ALLEGRO_BITMAP *p_shoot_l = NULL;
ALLEGRO_BITMAP *b_image = NULL;
ALLEGRO_BITMAP *e_image= NULL;
ALLEGRO_FONT *font = NULL;

Player p[MAX_PLAYERS];
Bauce b;

int initializeGame()
{
    if(!al_init())
    {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }

    if(!al_init_image_addon())
    {
        fprintf(stderr,"failed to initialize image addon!\n");
        return -1;
    }
    if(!al_install_keyboard())
    {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return -1;
    }

    al_init_font_addon();
    if(!al_init_ttf_addon())
    {
        fprintf(stderr,"failed to initialize ttfont addon!\n");
        return -1;
    }

    timer = al_create_timer(1 / FPS);
    if(!timer)
    {
        fprintf(stderr, "failed to create timer!\n");
        return -1;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(timer);
        return -1;
    }

    al_set_window_title(display, "Megaman Alpha V.04");
    font = al_load_ttf_font("comic.ttf", 24, 0);

    p_r = al_load_bitmap("image.png");
    al_set_display_icon(display, p_r);

    p_shoot_r = al_load_bitmap("image1.png");
    p_l = al_load_bitmap("image0.png");
    p_shoot_l = al_load_bitmap("image2.png");
    b_image = al_load_bitmap("image3.png");
    e_image = al_load_bitmap("image4.png");

    if(!p_r && !p_l && !p_shoot_r && !p_shoot_l && !b_image && !e_image)
    {
        fprintf(stderr, "failed to create bitmaps!\n");
        al_destroy_bitmap(e_image);
        al_destroy_bitmap(b_image);
        al_destroy_bitmap(p_shoot_r);
        al_destroy_bitmap(p_r);
        al_destroy_bitmap(p_shoot_l);
        al_destroy_bitmap(p_l);
        return -1;
    }

    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_font(font);
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        al_destroy_bitmap(e_image);
        al_destroy_bitmap(b_image);
        al_destroy_bitmap(p_shoot_r);
        al_destroy_bitmap(p_r);
        al_destroy_bitmap(p_shoot_l);
        al_destroy_bitmap(p_l);
        al_destroy_timer(timer);
        return -1;
    }

    int i;
    for(i = 0; i < MAX_PLAYERS; i++)
    {
        p[i].p_x = SCREEN_W / 2.0 - al_get_bitmap_width(p_r) / 2.0;
        p[i].p_y = SCREEN_H / 2.0 - al_get_bitmap_height(p_r) / 2.0;
        p[i].p_hp = 200;
        p[i].p_pos = 1;
    }
    b.e_x = 0;
    b.e_y = 0;
    b.e_hp = 200;
    b.e_pos = 1;
    return 0;
}

int startGame()
{
    char hpval[] = "";
    char ehpval[] = "";

    //int bullets = 0;
    //int bullet_cd = 0;
    //int jump_counter=0, j_down=0; //to be implemented

    float b_e_dx = 4.0, b_e_dy = 4.0;
    //float gravity = 4.0; // to be implemented

    bool key[5] = { false, false, false, false, false };
    bool redraw = true;
    bool shoot = false;
    bool e_dead = false;
    bool s_dead = false;
    bool doexit = false;
    bool tele = false;
    //bool jump = false;

    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_clear_to_color(al_map_rgb(0,0,0));

    al_flip_display();

    al_start_timer(timer);

    while(!doexit)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(key[KEY_UP] && p[0].p_y >= 4.0)
            {
                //if(jump_counter < 5){
                p[0].p_y -= 4.0;
                // jump_counter++;
                //}
            }

            if(key[KEY_DOWN] && p[0].p_y <= SCREEN_H - al_get_bitmap_height(p_r) - 4.0)
            {
                p[0].p_y += 4.0;
            }

            if(key[KEY_LEFT] && p[0].p_x >= 4.0)
            {
                p[0].p_x -= 4.0;
            }

            if(key[KEY_RIGHT] && p[0].p_x <= SCREEN_W - al_get_bitmap_width(p_r) - 4.0)
            {
                p[0].p_x += 4.0;
            }
            /**if(jump_counter >= 5)
            {
                p[0].p_y += gravity;
                j_down++;
            }
            if(j_down >=5)
            {
                j_down = 0;
                jump_counter = 0;
            }**/
            int i;
            for(i = 0; i < p[0].bullets; i++)
            {
                if(p[0].bullet_a[i].b_x < SCREEN_W - 4 && p[0].bullet_a[i].b_x > 0)
                {
                    if(!e_dead  && p[0].bullet_a[i].b_x + 2>= b.e_x  && p[0].bullet_a[i].b_x + 2 <= b.e_x + al_get_bitmap_width(e_image) && p[0].bullet_a[i].b_y + 2 >= b.e_y && p[0].bullet_a[i].b_y +2 <= b.e_y + al_get_bitmap_height(e_image))
                    {
                        b.e_hp-=5;
                        p[0].bullet_a[i].b_x = -1;
                        if(b.e_hp % 50 == 0)
                        {
                            tele = true;
                        }
                    }
                    else
                    {
                        p[0].bullet_a[i].b_x += (4.5*p[0].bullet_a[i].b_pos);
                    }
                }
                else
                {
                    p[0].bullet_a[i].b_x = -1;
                }
                if(b.bullet_a[i].b_x < SCREEN_W-4 && b.bullet_a[i].b_x > 0)
                {
                    if(!s_dead && b.bullet_a[i].b_x + 2 >= p[0].p_x && b.bullet_a[i].b_x + 2 <= p[0].p_x + al_get_bitmap_width(p_r) && b.bullet_a[i].b_y + 2 >= p[0].p_y && b.bullet_a[i].b_y + 2 <= p[0].p_y + al_get_bitmap_height(p_r))
                    {
                        p[0].p_hp-=10;
                        b.bullet_a[i].b_x = -1;

                    }

                    else
                    {
                        b.bullet_a[i].b_x +=(4.5*b.bullet_a[i].b_pos);
                    }
                }
                else
                {
                    b.bullet_a[i].b_x = -1;
                }
            }
            for(i = 0; i < p[1].bullets; i++)
            {
                if(!e_dead  && p[1].bullet_a[i].b_x + 2>= b.e_x  && p[1].bullet_a[i].b_x + 2 <= b.e_x + al_get_bitmap_width(e_image) && p[1].bullet_a[i].b_y + 2 >= b.e_y && p[1].bullet_a[i].b_y +2 <= b.e_y + al_get_bitmap_height(e_image))
                {
                        b.e_hp-=5;
                        p[0].bullet_a[i].b_x = -1;

                }
            }
            if(!s_dead && !e_dead)
            {
                if(!((b.e_x > p[0].p_x + al_get_bitmap_width(p_r) - 1) ||
                        (b.e_y > p[0].p_y + al_get_bitmap_height(p_r) - 1) ||
                        (p[0].p_x > b.e_x + al_get_bitmap_width(e_image) -1)||
                        (p[0].p_y > b.e_y + al_get_bitmap_height(e_image)-1)))
                {
                    p[0].p_hp-=10;
                }

                if(b.e_x < 0 || b.e_x > SCREEN_W - al_get_bitmap_width(e_image))
                {
                    b_e_dx = -b_e_dx;
                    b.e_pos = -b.e_pos;
                }
                if(b.e_y < 0 || b.e_y > SCREEN_H - al_get_bitmap_height(e_image))
                {
                    b_e_dy = -b_e_dy;
                }
                b.e_x+=b_e_dx;
                b.e_y+=b_e_dy;
            }
            if(p[0].p_hp<=0)
            {
                s_dead = true;
            }
            if(b.e_hp <= 0)
            {
                e_dead = true;
            }

            if(tele)
            {
                p[0].p_x = b.e_x - 40;
                p[0].p_y = b.e_y;
                tele = false;
            }
            //sprintf(hpval, "HP: %d", p[0].p_hp);

            //sprintf(ehpval, "Enemy HP: %d", enep[0].p_hp);
            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN && !s_dead)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key[KEY_UP] = true;
                break;

            case ALLEGRO_KEY_DOWN:
                key[KEY_DOWN] = true;
                break;

            case ALLEGRO_KEY_LEFT:
                key[KEY_LEFT] = true;
                p[0].p_pos = -1;
                break;

            case ALLEGRO_KEY_RIGHT:
                key[KEY_RIGHT] = true;
                p[0].p_pos = 1;
                break;
            case ALLEGRO_KEY_SPACE:
                key[SPACE] = true;
                shoot = true;
                Bullet b1;
                if(p[0].bullets > 512)
                {
                    p[0].bullets = 0;
                }
                b1.b_x = p[0].p_x+(21*p[0].p_pos);
                b1.b_y = p[0].p_y+16;
                b1.b_pos = p[0].p_pos;
                p[0].bullet_a[p[0].bullets] = b1;
                b1.b_x = b.e_x+(21*b.e_pos);
                b1.b_y = b.e_y+16;
                b1.b_pos = b.e_pos;
                b.bullet_a[p[0].bullets] = b1;
                (p[0].bullets)++;
                break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key[KEY_UP] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                key[KEY_DOWN] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                key[KEY_LEFT] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                key[KEY_RIGHT] = false;
                break;
            case ALLEGRO_KEY_SPACE:
                key[SPACE] = false;
                shoot = false;
                break;
            case ALLEGRO_KEY_ESCAPE:
                doexit = true;
                break;
            }
        }

        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            if(shoot)
            {
                al_clear_to_color(al_map_rgb(0,0,0));
                if(!s_dead)
                {
                    if(p[0].p_pos == 1 )
                    {
                        al_draw_bitmap(p_shoot_r, p[0].p_x, p[0].p_y, 0);
                    }
                    else
                    {
                        al_draw_bitmap(p_shoot_l, p[0].p_x,p[0].p_y,0);
                    }
                }
                if(!e_dead)
                {
                    al_draw_bitmap(e_image, b.e_x, b.e_y, 0);
                }
                int i;
                for(i = 0; i < p[0].bullets; i++)
                {
                    if(p[0].bullet_a[i].b_x > 0.0)
                    {
                        al_draw_bitmap(b_image, p[0].bullet_a[i].b_x, p[0].bullet_a[i].b_y, 0);
                    }
                    if(b.bullet_a[i].b_x > 0.0)
                    {
                        al_draw_bitmap(b_image, b.bullet_a[i].b_x, b.bullet_a[i].b_y, 0);
                    }
                }
                sprintf(hpval, "HP: %d", p[0].p_hp);
                al_draw_text(font, al_map_rgb(224,255,255), 0,0,0, hpval);
                sprintf(ehpval, "Enemy HP: %d", b.e_hp);
                al_draw_text(font, al_map_rgb(200,200,200), SCREEN_W-1, 0,ALLEGRO_ALIGN_RIGHT, ehpval);
                if(e_dead || s_dead)
                {
                    al_draw_text(font, al_map_rgb(224,255,255), SCREEN_W / 2.0 , SCREEN_H / 2.0, ALLEGRO_ALIGN_CENTRE, " GAME OVER \n Press Escape to Exit.");
                }
                if(p[1].p_pos == 1 )
                {
                    al_draw_bitmap(p_r, p[1].p_x, p[1].p_y, 0);
                }
                else
                {
                    al_draw_bitmap(p_l, p[1].p_x,p[1].p_y,0);
                }
                for(i = 0; i < p[1].bullets; i++)
                {
                    if(p[1].bullet_a[i].b_x > 0.0)
                    {
                        al_draw_bitmap(b_image, p[1].bullet_a[i].b_x, p[1].bullet_a[i].b_y, 0);
                    }
                }
                al_flip_display();
            }
            else
            {
                al_clear_to_color(al_map_rgb(0,0,0));
                if(!s_dead)
                {
                    if(p[0].p_pos == 1 )
                    {
                        al_draw_bitmap(p_r, p[0].p_x, p[0].p_y, 0);
                    }
                    else
                    {
                        al_draw_bitmap(p_l, p[0].p_x,p[0].p_y,0);
                    }
                }
                if(!e_dead)
                {
                    al_draw_bitmap(e_image, b.e_x, b.e_y, 0);
                }
                int i;
                for(i = 0; i < p[0].bullets; i++)
                {
                    if(p[0].bullet_a[i].b_x > 0.0)
                    {
                        al_draw_bitmap(b_image, p[0].bullet_a[i].b_x, p[0].bullet_a[i].b_y, 0);
                    }
                    if(b.bullet_a[i].b_x > 0.0)
                    {
                        al_draw_bitmap(b_image, b.bullet_a[i].b_x, b.bullet_a[i].b_y, 0);
                    }
                }
                sprintf(hpval, "HP: %d", p[0].p_hp);
                al_draw_text(font, al_map_rgb(224,255,255), 0,0,0, hpval);
                sprintf(ehpval, "Enemy HP: %d", b.e_hp);
                al_draw_text(font, al_map_rgb(200,200,200), SCREEN_W-1, 0,ALLEGRO_ALIGN_RIGHT, ehpval);
                if(e_dead || s_dead)
                {
                    al_draw_text(font, al_map_rgb(224,255,255), SCREEN_W / 2.0 , SCREEN_H / 2.0, ALLEGRO_ALIGN_CENTRE, " GAME OVER \n Press Escape to Exit.");
                }
                if(p[1].p_pos == 1 )
                {
                    al_draw_bitmap(p_r, p[1].p_x, p[1].p_y, 0);
                }
                else
                {
                    al_draw_bitmap(p_l, p[1].p_x,p[1].p_y,0);
                }
                for(i = 0; i < p[1].bullets; i++)
                {
                    if(p[1].bullet_a[i].b_x > 0.0)
                    {
                        al_draw_bitmap(b_image, p[1].bullet_a[i].b_x, p[1].bullet_a[i].b_y, 0);
                    }
                }
                al_flip_display();
            }
            if(server)
            {
                sprintf(s, "Current ally's HP: %d", p[0].p_hp);
                server_send(s);
                cout << p[1].bullet_a[0].b_x << "their hp" << endl << e_dead;
            }
            else
            {
                sprintf(s, "Current ally's HP: %d", p[0].p_hp);
                receive_data(s);
            }
        }
    }
    return 0;
}

int endGame()
{
    al_destroy_font(font);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_bitmap(e_image);
    al_destroy_bitmap(b_image);
    al_destroy_bitmap(p_shoot_r);
    al_destroy_bitmap(p_r);
    al_destroy_bitmap(p_shoot_l);
    al_destroy_bitmap(p_l);
    al_destroy_timer(timer);
    return 0;
}

/**
Server Code
**/
int setup_server()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 )
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);
    return 0;
}

int server_send(char *send_string)
{
    /**
    Send boss hp.
    // Receive until the peer shuts down the connection
    iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
    if (iResult > 0)
    {
        //printf("Bytes received: %d\n", iResult);
        //printf("%s\n", recvbuf);
        long newhp = atol(recvbuf);
        if(newhp < b.e_hp && newhp >= 0)
            b.e_hp = newhp;
    }
    else if (iResult == 0)
        printf("Connection closing...\n");
    else
    {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    char nottoolong[sizeof(long)];
    sprintf(nottoolong, "%ld", b.e_hp);
    iSendResult = send(ClientSocket, nottoolong, (int)sizeof(long), 0);
    if (iSendResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    **/
    //printf("Bytes sent: %d\n", iSendResult);
    //printf("Other side said: %s\n", recvbuf);
    //Player temp;
    /**
    Send player information.
    **/
    iResult = recv(ClientSocket, playervbuf, sizeof_player, 0);
    if (iResult > 0)
    {
        // printf("Bytes received: %d\n", iResult);
        // printf("%s\n", playervbuf);
        memcpy(&p[1], playervbuf, sizeof_player);
    }
    else if (iResult == 0)
        printf("Connection closing...\n");
    else
    {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    memcpy(playervbuf, &p[0],sizeof_player);
    iSendResult = send(ClientSocket,playervbuf, sizeof_player, 0);
    if (iSendResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    //printf("Bytes sent: %d\n", iSendResult);
    //printf("Other side said: %s\n", playervbuf);
}

int close_server()
{
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

/**
Client
**/
int setup_client(char *ipaddress)
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ipaddress, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 )
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ; ptr=ptr->ai_next)
    {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    return 0;
}

int receive_data(char *send_string)
{
    /**
    Send boss information.

    // Send an initial buffer
    char nottoolong[sizeof(long)];
    sprintf(nottoolong, "%ld", b.e_hp);

    iResult = send( ConnectSocket, nottoolong, (int)sizeof(long), 0 );
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    iResult = recv(ConnectSocket, recvbuf, 512, 0);
    if ( iResult > 0 )
    {
        printf("Bytes received: %d\n", iResult);
        long newhp = atol(recvbuf);
        if(newhp< b.e_hp && newhp >= 0)
            b.e_hp = newhp;
        printf("%s\n", recvbuf);
    }
    else if ( iResult == 0 )
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());
    **/

    /**
    Send player information.
    **/
    memcpy(playervbuf, &p[0],sizeof_player);
    iResult = send( ConnectSocket, playervbuf, sizeof_player, 0 );
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    iResult = recv(ConnectSocket, playervbuf, sizeof_player, 0);
    if ( iResult > 0 )
    {
        printf("Bytes received: %d\n", iResult);
        memcpy(&p[1], playervbuf,sizeof_player);
    }
    else if ( iResult == 0 )
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());


}

int close_client()
{
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

int main(int argc, char **argv)
{
    cout << "Are you hosting or joining a game? [1]: Host [2]: Join " << endl;
    int choice;
    do
    {
        cin >> choice;
        switch(choice)
        {
        case 1:
            server = true;
            break;
        case 2:
            server = false;
            break;
        default:
            cout << endl << "Please enter 1 or 2: ";
        }
    }
    while( choice != 1 && choice != 2);
    if(server)
    {
        cout << "Waiting for other players..."<<endl;
        if(setup_server())
        {
            cout << "Failed creating server... Program exiting... Press ENTER to continue...";
            cin.sync();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return -1;
        }
    }
    else
    {
        cout << "Enter server IP or name: ";
        cin >> ip_address;
        cout << endl << "Joining server..." << endl;
        if(setup_client(ip_address))
        {
            cout << "Failed joining server... Program exiting... Press ENTER to continue...";
            cin.sync();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return -1;
        }
    }
    initializeGame();
    startGame();
    endGame();
    if(server)
    {
        close_server();
    }
    else
    {
        close_client();
    }
}
