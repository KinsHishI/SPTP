#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "mine.h"
#include <stdbool.h>

#define BUF_SIZE 100
#define NAME_SIZE 20 // Dont use? 

//cmd type
#define GAME_START 1
#define GAME_SETTING 2
#define GAME_PLAY 3
#define WIN 4
#define LOSE 5
#define WIN2 6
#define GAME_PLAY2 7
#define LOSE2 8 // Other Cath all bomb..

typedef struct {
    int cmd;
    int n;
    int m;
    int bomb;
} PACKET3;

typedef struct {
    int cmd;
    int x, y;
    char c[10];

} PACKET2;

typedef struct {
    int cmd;
    // Here, We need to add data for A_info ,B_info
    int clnt_sock;
    char game[MAX][MAX];

} PACKET;

pthread_mutex_t mutx;
int m, n, bomb;
// char gameBoard[MAX][MAX];
// char game[MAX][MAX];
// struct position bombPosition; // 사용자간의 통신때 이용되는 변수
int gameend = 0;


void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

//char name[NAME_SIZE] = "[DEFAULT]";
//char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;

    PACKET recv_packet;
    PACKET3 send_packet;
    memset(&recv_packet, 0, sizeof(recv_packet));
    memset(&send_packet, 0, sizeof(send_packet));
    int rx_len;
    char name[50];


    if (argc != 4) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    //sprintf(name, "[%s]", argv[3]);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");


    /*
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    */

    while (1)
    {
        rx_len = read(sock, (void*)&recv_packet, sizeof(PACKET));

        if (recv_packet.cmd == GAME_START)
        {
            strcpy(name, argv[3]);
            write(sock, name, strlen(name));

            send_packet.cmd = GAME_SETTING;
            FILE* logo_fp;
            char logo[BUFSIZ] = { 0, };
            logo_fp = fopen("logo.txt", "r");
            fread(logo, sizeof(char), BUFSIZ, logo_fp);
            printf("\033[1;35m%s\033[0m\n", logo);

            fclose(logo_fp);
            printf("Welcome to Minesweeper\n");
            printf("How many rows? (max 100) : ");
            scanf("%d", &m);
            printf("How many columns? (max 100) : ");
            scanf("%d", &n);
            printf("How many bombs? : ");
            scanf("%d", &bomb);

            // bombPosition.cnt = bomb;

            // for (int i = 0; i < m; i++) {
            //     for (int j = 0; j < n; j++) {
            //         gameBoard[i][j] = '0';
            //         game[i][j] = '*';
            //     }
            // }

            // installMines(0, 0, gameBoard, game,m,n,bomb,&bombPosition);

            // graphic(game, m, n);

            send_packet.m = m;
            send_packet.n = n;
            send_packet.bomb = bomb;

            write(sock, (void*)&send_packet, sizeof(PACKET3));
            memset(&send_packet, 0, sizeof(send_packet));

            printf("Waitting for Setting Other\n");
        }
        else if (recv_packet.cmd == GAME_PLAY)
        {
            // output GAME TABLE
            graphic(recv_packet.game, m, n);
            break;
        }
        memset(&recv_packet, 0, sizeof(recv_packet));

    }

    // Here We Divide INPUT,, OUTPUT

    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);


    close(sock);
    return 0;
}




void* send_msg(void* arg)       // send thread main
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    PACKET2 packet;
    int x, y;
    char cmd[10];

    int str_len;
    char buf[BUF_SIZE];

    while (1)
    {
        if (gameend) break;
        // // GAME END signal handling
        // if(gmaeend == WIN) {

        //     break;
        // }
        // else if(gameend == LOSE) {
        //     break;
        // }
        // else if (gameend == WIN2) {
        //     break;
        // }
        // ----------------------------------------


        printf("Enter x, y : ");
        scanf("%d %d", &x, &y);
        if (x < 1 || x > n || y < 1 || y > m) {
            printf("Out of range\n");
            continue;
        }

        printf("Enter command : ");
        scanf("%s", cmd);

        if (gameend) break;

        if (cmd[0] != 'o' && cmd[0] != 'f') {
            printf("Invalid command\n");
            continue;
        }

        packet.cmd = GAME_PLAY;
        packet.x = x;
        packet.y = y;
        strncpy(packet.c, cmd, 1);

        write(sock, (void*)&packet, sizeof(PACKET2));
        memset(&packet, 0, sizeof(packet));

        sleep(1);
    }
    return NULL;
}



void* recv_msg(void* arg)      // read thread main
{
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int rx_len;
    PACKET recv_packet;

    int scorereq = 0;

    while (1)
    {
        rx_len = read(sock, (void*)&recv_packet, sizeof(PACKET));

        if (recv_packet.cmd == GAME_PLAY)
        {
            printf("My GAME INTERFACE\n");
            graphic(recv_packet.game, m, n);
        }
        else if (recv_packet.cmd == GAME_PLAY2)
        {
            printf("Other GAME INTERFACE\n");
            graphic(recv_packet.game, m, n);

        }
        else if (recv_packet.cmd == WIN)
        {
            pthread_mutex_lock(&mutx);
            gameend = 1;
            pthread_mutex_unlock(&mutx);
            printf("I catch ALL BOMB FLAG!!! I HAVE WIN YEYEYEYEYE\n");

            scorereq = 1;
            break;
        }
        else if (recv_packet.cmd == LOSE)
        {
            pthread_mutex_lock(&mutx);
            gameend = 1;
            pthread_mutex_unlock(&mutx);

            printf("I GOT BOMB... I LOSE...\n");
            break;
        }
        else if (recv_packet.cmd == WIN2)
        {
            pthread_mutex_lock(&mutx);
            gameend = 1;
            pthread_mutex_unlock(&mutx);

            printf("Other got bomb!!!! I WIN\n");
            break;
        }
        else if (recv_packet.cmd == LOSE2)
        {
            pthread_mutex_lock(&mutx);
            gameend = 1;
            pthread_mutex_unlock(&mutx);

            printf("Other catch ALL BOMB!!!! I LOSE...\n");
            break;
        }


    }

    if (scorereq) {
        char buf[BUF_SIZE];
        int len;
        while (1) {
            len = read(sock, buf, BUF_SIZE);
            if (len < BUF_SIZE) {
                break;
            }
            printf("%s", buf);
        }
    }
    return NULL;
}


void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}