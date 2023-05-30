// TEAM 12  -   지뢰 찾기 프로젝트  (2018110115 최지원, 2022116110 정민규, 2022115008 이정민)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX 101

struct position 
{
    int cnt;
    int x[MAX];
    int y[MAX];
}; 


void installMines(int x, int y, char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n, int bomb,struct position *bombPosition);
void flag(int x, int y, char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n);
void graphic(char table[MAX][MAX],int m, int n);
void open(int x, int y,char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n);
void findzero(int x, int y,char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n);



 
void installMines(int x, int y, char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n, int bomb,struct position *bombPosition) {
    
    
    int mineNumber = 0;
    int bombmap[MAX][MAX];
    srand(time(NULL));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            bombmap[i][j] = (i - 1) * n + j - 1;
        }
    }

    // 랜덤으로 폭탄을 배치
    for (int i = 1; i <= bomb; i++) {
        int random = rand() % (m * n - i + 1) + i - 1;
        int temp = bombmap[1 + random / n][random % n + 1];
        bombmap[1 + random / n][random % n + 1] = bombmap[1 + i / n][i % n + 1];
        bombmap[1 + i / n][i % n + 1] = temp;
        int raw = bombmap[1 + i / n][i % n + 1] / n;
        int line = bombmap[1 + i / n][i % n + 1] % n;
        gameBoard[line][raw] = 'B';
        bombPosition->x[i] = line;
        bombPosition->y[i] = raw;
    }
   
    // 게임판 세팅
    for (int raw = 0; raw < m; raw++) {
        for (int line = 0; line < n; line++) {
            if (gameBoard[raw][line] != 'B') {
                int num = 0;
                if (raw > 0 && gameBoard[raw - 1][line] == 'B') num++; // up
                if (raw < m - 1 && gameBoard[raw + 1][line] == 'B') num++; // down
                if (line > 0 && gameBoard[raw][line - 1] == 'B') num++; // left
                if (line < n - 1 && gameBoard[raw][line + 1] == 'B') num++; // right
                if (raw > 0 && line > 0 && gameBoard[raw - 1][line - 1] == 'B') num++; // up-left
                if (raw > 0 && line < n - 1 && gameBoard[raw - 1][line + 1] == 'B') num++; // up-right
                if (raw < m - 1 && line > 0 && gameBoard[raw + 1][line - 1] == 'B') num++; // down-left
                if (raw < m - 1 && line < n - 1 && gameBoard[raw + 1][line + 1] == 'B') num++; // down-right
                gameBoard[raw][line] = num + '0';
            }
        }
    }
}

// Flag 설정
void flag(int x, int y, char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n) {
    if (game[y - 1][x - 1] == '*') {
        game[y - 1][x - 1] = 'F';
    }
    else if (game[y - 1][x - 1] == 'F') {
        game[y - 1][x - 1] = '*';
    }
}

// 게임 화면 실행
void graphic(char table[MAX][MAX],int m, int n) {
    
    
    printf("        ");
    for (int i = 1; i <= n; i++) {
        printf("|%3d| ", i);
    }
    printf("\n");
    printf("\n");

    for (int i = 0; i < m; i++) {
        printf("|%3d| ", i + 1);
        for (int j = 0; j < n; j++) {
            printf("%5c ", table[i][j]);
        }
        printf("\n\n");
    }
}

// 게임 시작 후 빈칸 열기
void open(int x, int y,char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n) {
    if (gameBoard[y - 1][x - 1] == 'B') {
        game[y - 1][x - 1] = '*';
    }
    else if (game[y - 1][x - 1] == '*') {
        game[y - 1][x - 1] = gameBoard[y - 1][x - 1];
        if (gameBoard[y - 1][x - 1] == '0') {
            findzero(x - 1, y - 1, gameBoard, game, m, n);
        }
    }
}

// 0 인 부분
void findzero(int x, int y,char gameBoard[MAX][MAX], char game[MAX][MAX], int m, int n) {
    game[y][x] = gameBoard[y][x];
    if (gameBoard[y][x] != '0') {
        return;
    }
    if (y - 1 >= 0 && game[y - 1][x] == '*') {
        open(x, y - 1, gameBoard, game, m, n);
        findzero(x, y - 1, gameBoard, game, m, n);
    }
    if (y + 1 < m && game[y + 1][x] == '*') {
        open(x, y + 1, gameBoard, game, m, n);
        findzero(x, y + 1, gameBoard, game, m, n);
    }
    if (x - 1 >= 0 && game[y][x - 1] == '*') {
        open(x - 1, y, gameBoard, game, m, n);
        findzero(x - 1, y, gameBoard, game, m, n);
    }
    if (x + 1 < n && game[y][x + 1] == '*') {
        open(x + 1, y, gameBoard, game, m, n);
        findzero(x + 1, y, gameBoard, game, m, n);
    }
}