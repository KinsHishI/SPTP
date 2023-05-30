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
