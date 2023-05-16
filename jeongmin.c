#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

void print_logo();
void print_scoreboard();
void print_minesweeper_board();
void play_minesweeper();
void end_minesweeper(int);
void give_up();
void time_out();
void placement();
void init_game();
void clear_read_buffer();
void record_game(int);
int is_game_end();
void* print_time();

char Attacker[50];
char Defenser[50];

int main() {
    int option = 0;

    system("clear");

    while (1) {
        printf("\nPlay a minesweeper with your friend!\n");

        puts("1) Play game");
        puts("2) Scoreboard");
        puts("3) Rule");
        puts("4) Exit");
        scanf("%d", &option);
        clear_read_buffer();

        switch (option) {
        case 1:
            signal(SIGINT, give_up); // Ctrl+C 입력 시 게임 중단
            signal(SIGALRM, time_out); // 입력 안할 시 게임 종료
            
            init_game();
            play_minesweeper();
            break;

        case 2:
            system("clear");
            print_scoreboard();
            break;

        case 3:
            system("clear");
            puts("How to play the minesweeper?");
            break;
        case 4:
            exit(0);
        default:
            break;
        }
    }
}

void init_game() {

}

void play_minesweeper() {
    print_minesweeper_board();

    while (1) {
        print_minesweeper_board();

        if (is_game_end()) {
            end_minesweeper();
            break;
        }
    }
}

void print_scoreboard() {
    FILE* scoreboard_fp;
    char Attacker[4];
    char Defenser[4];
    char winner[4];
    int index;
    time_t t;

    index = 0;
    scoreboard_fp = fopen("./bin/scoreboard", "r");

    system("clear");

    printf(" Index | Attacker | Defenser | Winner |       Time\n");
    while (fscanf(scoreboard_fp, "%s %s %s %ld", Attacker, Defenser, winner, &t) != EOF) {
        printf("--------------------------------------------------------------\n");
        printf(" %d     |   %s   |   %s   |   %s  | %s", index++, Attacker, Defenser, winner, ctime(&t));
    }
    printf("--------------------------------------------------------------\n");
}

void end_minesweeper(int winner) {
    int option;

    // 게임이 끝났으므로 시그널 무시
    signal(SIGINT, SIG_IGN);
    signal(SIGALRM, SIG_IGN);

    if (winner == Attacker) {
        printf("Attacker is winner!\n");
    }
    else if (winner == Defenser) {
        printf("Defenser is winner!\n");
    }

    puts("Do you want to record the result of this game at the scoreboard?");
    puts("1) yes");
    puts("2) no");
    printf(": ");
    scanf("%d", &option);

    if (option == 2) {
        puts("Good Game Well Played!");
        exit(0);
    }

    puts("-------------------------------------");
    puts("| Input your name within 3 letters. |");
    puts("-------------------------------------");

    printf("Player 1 (Attacker): ");
    scanf("%s", Attacker);

    printf("Player 2 (Defenser): ");
    scanf("%s", Defenser);

    record_game(winner);

    puts("Good Game Well Played!");

    exit(0);
}

void record_game(int winner) {
    FILE* scoreboard_fp;
    int thepipe[2];
    char buf[BUFSIZ];

    if (pipe(thepipe) == -1) {
        perror("pipe");
    }

    // fork와 pipe를 이용한 병렬 처리
    switch (fork()) {
    case -1:
        perror("fork");
        exit(1);
        // child process가 결과를 기록함

    case 0:
        close(thepipe[1]);
        read(thepipe[0], buf, BUFSIZ);

        scoreboard_fp = fopen("./bin/scoreboard", "a+");

        if (scoreboard_fp == NULL) {
            perror("scoreboard_fp fopen");
            exit(1);
        }

        fprintf(scoreboard_fp, "%s", buf);
        fflush(scoreboard_fp);

        exit(0);
        // parent process가 child process에게 결과 전송
    default:
        close(thepipe[0]);

        if (winner == Attacker)
            sprintf(buf, "%s %s %s %ld\n", Attacker, Defenser, Attacker, time(NULL));

        else if (winner == Defenser)
            sprintf(buf, "%s %s %s %ld\n", Attacker, Defenser, Defenser, time(NULL));

        if (write(thepipe[1], buf, strlen(buf)) != strlen(buf)) {
            perror("write");
            exit(1);
        }

        wait(NULL);

        puts("Record completed.");

        break;
    }
}