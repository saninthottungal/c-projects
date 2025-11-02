#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tic_tac_toe.h"

int main()
{
    Player turn = PX;
    char board[3][3] = {
        {' ', ' ', ' '},
        {' ', ' ', ' '},
        {' ', ' ', ' '},
    };

    while (1)
    {
        int x = 0, y = 0;
        drawBoard(board);

        takeTurn(turn, &x, &y);
        updateTakeAndTurn(board, &turn, x, y);
        int gameRunning = calculateWinner(board);
        if (!gameRunning)
        {
            turn = PX;
            board[0][0] = ' ';
            board[0][1] = ' ';
            board[0][2] = ' ';
            board[1][0] = ' ';
            board[1][1] = ' ';
            board[1][2] = ' ';
            board[2][0] = ' ';
            board[2][1] = ' ';
            board[2][2] = ' ';
        }

        system("clear");
    }

    return 0;
}
