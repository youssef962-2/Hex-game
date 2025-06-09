#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 11
#define MAX_NAME 20

typedef struct {
    char name[MAX_NAME];
    char symbol;
    int id;
    int startEdge; // 0: top, 1: bottom, 2: left, 3: right
    int endEdge;
    const char *color;  // Added color field
} Player;

void initializeBoard(char board[SIZE][SIZE]);
void displayBoard(char board[SIZE][SIZE], Player players[4]);
int isValidMove(char board[SIZE][SIZE], int row, int col);
void placeMark(char *cell, char symbol);
int isBoardFull(char board[SIZE][SIZE]);
void switchPlayerTurn(int *currentPlayer);
int isWinningMove(char board[SIZE][SIZE], Player player);
int dfs(char board[SIZE][SIZE], int visited[SIZE][SIZE], int row, int col, Player player);
int charToRowIndex(char c);

int main() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    char board[SIZE][SIZE];
    Player players[4] = {
        {"Player1", 'A', 1, 0, 1, "\x1b[41m"},  // Red
        {"Player2", 'B', 2, 1, 0, "\x1b[46m"},  // cyan
        {"Player3", 'C', 3, 2, 3, "\x1b[44m"},  // Blue
        {"Player4", 'D', 4, 3, 2, "\x1b[45m"}   // miginta
    };

    initializeBoard(board);

    // Reserve unique starting points
    placeMark(&board[0][5], 'A');   // A6 for Player1 (Top)
    placeMark(&board[10][5], 'B');  // K6 for Player2 (Bottom)
    placeMark(&board[5][0], 'C');   // F1 for Player3 (Left)
    placeMark(&board[5][10], 'D');  // F11 for Player4 (Right)

    int currentPlayer = 0;
     int movesPlayed = 4;

    while (1) {
        displayBoard(board, players);
        char rowChar;
        int col;
        int rowIndex;

        while (1) {
            printf("%s%s (%c)\x1b[0m, enter your move (e.g., B 5): ", 
                players[currentPlayer].color, 
                players[currentPlayer].name, 
                players[currentPlayer].symbol
            );

            int result = scanf(" %c %d", &rowChar, &col);
            while (getchar() != '\n'); // Clear buffer

            rowChar = toupper(rowChar);
            rowIndex = charToRowIndex(rowChar);

            if (rowIndex >= 0 && rowIndex < SIZE) {
                if (col >= 1 && col <= SIZE) {
                    if (isValidMove(board, rowIndex, col - 1)) {
                        break;
                    } else {
                        printf("That cell is already occupied. Try again.\n");
                        continue;
                    }
                } else {
                    printf("Column out of range (1-11). Try again.\n");
                    continue;
                }
            } else {
                printf("Invalid input. Row must be A–K and column must be 1–11. Try again.\n");
                continue;
            }
        }

        placeMark(&board[rowIndex][col - 1], players[currentPlayer].symbol);
         movesPlayed ++;

        if (isWinningMove(board, players[currentPlayer])) {
            displayBoard(board, players);
            printf("🎉 %s%s wins!\x1b[0m\n", players[currentPlayer].color, players[currentPlayer].name);
            saveGameResult(players[currentPlayer], movesPlayed);
            break;
        }

        if (isBoardFull(board)) {
            displayBoard(board, players);
            printf("The board is full. It's a draw!\n");
            break;
        }

        switchPlayerTurn(&currentPlayer);
    }

    return 0;
}

void initializeBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = '.';
}

void displayBoard(char board[SIZE][SIZE], Player players[4]) {
    printf("\n     ");
    for (int i = 1; i <= SIZE; i++)
        printf("%2d ", i);
    printf("\n");

    for (int i = 0; i < SIZE; i++) {
        printf("  %c ", 'A' + i);
        for (int k = 0; k < i; k++) printf("  ");
        for (int j = 0; j < SIZE; j++) {
            int printed = 0;
            for (int p = 0; p < 4; p++) {
                if (board[i][j] == players[p].symbol) {
                    printf("%s %c \x1b[0m", players[p].color, board[i][j]);
                    printed = 1;
                    break;
                }
            }
            if (!printed)
                printf(" %c ", board[i][j]);
        }
        printf("\n");
    }
}

int isValidMove(char board[SIZE][SIZE], int row, int col) {
    if ((row == 0 && col == 5) ||     // A6
        (row == 10 && col == 5) ||    // K6
        (row == 5 && col == 0) ||     // F1
        (row == 5 && col == 10))      // F11
        return 0;

    return board[row][col] == '.';
}

void placeMark(char *cell, char symbol) {
    *cell = symbol;
}

int isBoardFull(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == '.')
                return 0;
    return 1;
}

void switchPlayerTurn(int *currentPlayer) {
    *currentPlayer = (*currentPlayer + 1) % 4;
}

int isWinningMove(char board[SIZE][SIZE], Player player) {
    int visited[SIZE][SIZE] = {0};
    if (player.startEdge == 0) {
        for (int j = 0; j < SIZE; j++)
            if (board[0][j] == player.symbol && dfs(board, visited, 0, j, player)) return 1;
    } else if (player.startEdge == 1) {
        for (int j = 0; j < SIZE; j++)
            if (board[SIZE - 1][j] == player.symbol && dfs(board, visited, SIZE - 1, j, player)) return 1;
    } else if (player.startEdge == 2) {
        for (int i = 0; i < SIZE; i++)
            if (board[i][0] == player.symbol && dfs(board, visited, i, 0, player)) return 1;
    } else if (player.startEdge == 3) {
        for (int i = 0; i < SIZE; i++)
            if (board[i][SIZE - 1] == player.symbol && dfs(board, visited, i, SIZE - 1, player)) return 1;
    }
    return 0;
}

int dfs(char board[SIZE][SIZE], int visited[SIZE][SIZE], int row, int col, Player player) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) return 0;
    if (visited[row][col] || board[row][col] != player.symbol) return 0;

    visited[row][col] = 1;

    if ((player.endEdge == 0 && row == 0) ||
        (player.endEdge == 1 && row == SIZE - 1) ||
        (player.endEdge == 2 && col == 0) ||
        (player.endEdge == 3 && col == SIZE - 1)) return 1;

    int dr[] = {-1, -1, 0, 0, 1, 1};
    int dc[] = {0, 1, -1, 1, -1, 0};

    for (int i = 0; i < 6; i++) {
        int newRow = row + dr[i];
        int newCol = col + dc[i];
        if (dfs(board, visited, newRow, newCol, player)) return 1;
    }

    return 0;
}

int charToRowIndex(char c) {
    return c - 'A';
}
void saveGameResult(Player player, int movesPlayed) {
    FILE *file = fopen("C:\\Users\\HARDWARE\\Desktop\\game_results.txt", "a");
    if (file == NULL) {
        printf("Error: Could not open file to save result.\n");
        return;
    }
    fprintf(file, "Winner: %c\n", player.symbol);
    fprintf(file, "Moves Played: %d\n", movesPlayed);
    fprintf(file, "-----------------------\n");
    fclose(file);
}
