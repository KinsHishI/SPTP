// TEAM 12  -   지뢰 찾기 프로젝트  (2018110115 최지원, 2022116110 정민규, 2022115008 이정민)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#include "mine.h"
#include <stdbool.h>


#define BUF_SIZE 100
#define MAX_CLNT 2
#define MAX_SIZE 100

//cmd type
#define GAME_START 1
#define GAME_SETTING 2
#define GAME_PLAY 3
#define WIN 4   // 유저가 모든 지뢰를 찾았을 때 승리했음을 알리는 cmd
#define LOSE 5  // 유저가 지뢰를 밟았을 때 패배했음을 알리는 cmd
#define WIN2 6  // 상대방이 지뢰를 밟았을 때 승리했음을 알리는 cmd
#define GAME_PLAY2 7
#define LOSE2 8 // 상대방이 모든 지뢰를 다 찾았을 때 패배했음을 알리는 cmd

typedef struct {
    char name[50];
    double score;
} User;

// Game Setting을 위한 PACKET
typedef struct {
    int cmd;
    int n;
    int m;
    int bomb;
} PACKET3;

// GAME PLAY를 위한 PACKET (유저가 여는 번호판 전달)
typedef struct {
    int cmd;
    int x,y;
    char c[10];

} PACKET2;

// GAME DATA 전달을 위한 PACKET (유저가 보드를 열어 업데이트 되는 보드판이 이동한다!!)
typedef struct {
    int cmd;
    // Here, We need to add data for A_info ,B_info
    int clnt_sock;
    char game[MAX][MAX];
} PACKET;

int compare(const void* a, const void* b) {
    return ((User*)a)->score - ((User*)b)->score;
}

void * handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);
void update(char dest[MAX][MAX], char src[MAX][MAX],int m, int n);
void filesort(void);

// 쓰레드가 공유하는 메모리 (전역 변수)
int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
PACKET clnt_info[MAX_CLNT];
pthread_mutex_t mutx; // 임계영역(critical section)에 접근할 때 lock을 위한 선언
int set_cnt=0;
int win2 = 0;
int lose2 = 0;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    if(argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while(1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_info[clnt_cnt].clnt_sock = clnt_sock;
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));

    }

    close(serv_sock);
    return 0;

}

void *handle_clnt(void * arg)
{
    int clnt_sock = *((int*)arg);
    struct timespec begin, end;
    double execution_time;

    int m, n, bomb;
    char gameBoard[MAX][MAX];
    char game[MAX][MAX];
    struct position bombPosition;

    PACKET recv_packet;
	PACKET send_packet;
	memset(&recv_packet,0,sizeof(recv_packet));
	memset(&send_packet,0,sizeof(send_packet));
    
    int rx_len = 0; int str_len;
    int me, you;
    char name[50];


    while(1) if (clnt_cnt == 2) break;

    send_packet.cmd = GAME_START;
    write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
    memset(&send_packet,0,sizeof(send_packet));

    str_len = read(clnt_sock, name, sizeof(name));

    printf("%s\n\n", name);

    PACKET3 setting_packet;
    memset(&setting_packet,0, sizeof(setting_packet));

    rx_len = read(clnt_sock, (void*) &setting_packet, sizeof(PACKET3));

    if(setting_packet.cmd == GAME_SETTING) {
        n = setting_packet.n;
        m = setting_packet.m;
        bomb = setting_packet.bomb;

        printf("%d %d %d\n",m,n,bomb);
        bombPosition.cnt = bomb;
        
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                gameBoard[i][j] = '0';
                game[i][j] = '*';
                
            }
        }

        installMines(0, 0, gameBoard, game,m,n,bomb,&bombPosition);

    }

    pthread_mutex_lock(&mutx);
    set_cnt++;

    for(int k = 0; k<MAX_CLNT; k++) {
        if(clnt_info[k].clnt_sock == clnt_sock ) {
            me = k;
            for (int i = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    clnt_info[k].game[i][j] = game[i][j];
                }
            }
        }
        else {
            you = k;
        }
    }

    pthread_mutex_unlock(&mutx);

    send_packet.cmd = GAME_PLAY;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            send_packet.game[i][j] = game[i][j];
        }
    }

    
    while(1) if (set_cnt == 2) break;

    write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
    memset(&send_packet, 0, sizeof(send_packet));


    PACKET2 play_packet;
    memset(&play_packet,0,sizeof(play_packet));
    
    clock_gettime(CLOCK_MONOTONIC, &begin);

    while(1)
    {
        rx_len = read(clnt_sock, (void*) &play_packet, sizeof(PACKET2));

        if(lose2) { // LOSE 2
            send_packet.cmd = LOSE2;

            write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
            memset(&send_packet,0,sizeof(send_packet));
            break;

        }

         if(win2) { // WIN 2
            send_packet.cmd = WIN2;

            write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
            memset(&send_packet,0,sizeof(send_packet));
            break;

        }

        // 게임 진행 중일 때, User가 전달한 번호판을 열고 그 게임판을 유저에게 전송한다
        if(play_packet.cmd == GAME_PLAY)  
        {
            int x,y, check;
            x= play_packet.x, y = play_packet.y;
            if (play_packet.c[0] == 'o') {
                open(x, y, gameBoard, game , m, n);
                
                // LOSE
                if (gameBoard[y - 1][x - 1] == 'B') {
                    pthread_mutex_lock(&mutx);
                    send_packet.cmd = LOSE;
                    win2 = 1;
                    write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
                    memset(&send_packet,0,sizeof(send_packet));
                    pthread_mutex_unlock(&mutx);

                    break;
                }
            }
            else if (play_packet.c[0] == 'f') {
                flag(x,y,gameBoard, game, m, n);

            }

            // Win Check
            check = 1;

            for (int i = 0; i < m; i++) {
                for (int j = 0; j < n; j++)
                {
                    if (gameBoard[i][j] == 'B')
                    {
                        if (game[i][j] != 'F')
                        check = 0;
                    }
                }
            }
            
            pthread_mutex_lock(&mutx);
            update(clnt_info[me].game, game, m, n);
            
            
            if (check == 1) { // GAME Win
                
                clock_gettime(CLOCK_MONOTONIC, &end);
                execution_time = (double) (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 10000000000.0 ;
                FILE *file = fopen("record.txt", "a+");
                
                if (file != NULL) {
                    fprintf(file, "%d %s %.6f\n", clnt_sock, name ,execution_time);
                    fclose(file);
                }
                else {
                    printf("file can't open");
                    exit(1);
                }

                send_packet.cmd = WIN;
                lose2 = 1;
                write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
                memset(&send_packet,0,sizeof(send_packet));
                pthread_mutex_unlock(&mutx);

                filesort();

                if((file = fopen("output.txt", "r")) != NULL) {
                    char buf[BUF_SIZE];
                    int len;
                    while(1) {
                        len = fread(buf, sizeof(char), BUF_SIZE,file);

                        if(len >= BUF_SIZE) {
                            write(clnt_sock, buf, BUF_SIZE);
                        }
                        else {
                            write(clnt_sock, buf, BUF_SIZE);
                            fclose(file);

                            break;
                        }
                    }

                }
                else {
                    printf("file not OPEN");
                    exit(1);
                }

                break;

            }

            else {  // GAME NOT END JUST PLAY
                send_packet.cmd = GAME_PLAY;
                update(send_packet.game, game, m, n);
                write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
                memset(&send_packet,0,sizeof(send_packet));

                send_packet.cmd = GAME_PLAY2;
                update(send_packet.game, clnt_info[you].game, m, n);
                write(clnt_sock, (void*) &send_packet, sizeof(PACKET));
                memset(&send_packet,0,sizeof(send_packet));
            }

            pthread_mutex_unlock(&mutx);
        }
        
        memset(&play_packet,0,sizeof(play_packet));
    }


    pthread_mutex_lock(&mutx);
    for(int i=0; i<clnt_cnt; i++)   // remove disconnected client
    {
        if(clnt_sock == clnt_socks[i])
        {
            while(i++ < clnt_cnt-1)
                clnt_socks[i] = clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

// dest에 src를 Update!!
void update(char dest[MAX][MAX], char src[MAX][MAX],int m, int n)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            dest[i][j] = src[i][j];
        }
    }

}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

//파일을 시간(score)에 대한 오름차순으로 정렬
void filesort() {
    User data[MAX_SIZE];
    int num_elements = 0;

    // 파일에서 데이터를 읽어옴
    FILE* file = fopen("record.txt", "r");
    if (file == NULL) {
        printf("file can Not OPEN");
        return ;
    }
    int temp;
    while (fscanf(file, "%d %s %lf", &temp ,data[num_elements].name, &data[num_elements].score) != EOF) {
        num_elements++;
    }
    fclose(file);

    // 데이터를 정렬
    qsort(data, num_elements, sizeof(User), compare);

    // 정렬된 데이터를 파일에 저장
    file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("file can Not OPEN.");
        return ;
    }

    for (int i = 0; i < num_elements; i++) {
        fprintf(file, "%d %s %f\n",i+1, data[i].name, data[i].score);
    }
    fclose(file);

    printf("Store Data\n");

    return ;
}
