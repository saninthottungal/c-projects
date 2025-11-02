#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tic_tac_toe.h"

void drawBoard(char board[3][3])
{

    printf("\nTic-Tac-Toe\n\n");

    for (int i = 0; i < 3; i++)
    {
        if (i != 0)
        {
            printf("\n-----------\n");
        }

        for (int j = 0; j < 3; j++)
        {
            printf(" %c ", board[i][j]);
            if (j < 2)
            {
                printf("|");
            }
        }
    }

    printf("\n\n");
}

void takeTurn(Player turn, int *x, int *y)
{
    char player = turn == PX ? 'X' : '0';
    int n1, n2;

    printf("turn of %c, play! (1-3)(1-3): ", player);
    scanf("%d %d", &n1, &n2);
    *x = n1 - 1;
    *y = n2 - 1;
}

void updateTakeAndTurn(char board[3][3], Player *turn, int x, int y)
{
    char player = *turn == PX ? 'X' : '0';
    int isValid = board[x][y] == ' ' && x < 3 && y < 3;
    if (!isValid)
    {
        printf("!!! Invalid Move !!!\n");
        sleep(2);
    }
    else
    {
        board[x][y] = player;
        *turn = *turn == PX ? P0 : PX;
    }
}
int calculateWinner(char board[3][3])
{
    int gameRunning = 1;
    Winner winner = NONE;

    for (int i = 0; i < 3; i++)
    {
        if ((board[i][0] == board[i][1] && board[i][1] == board[i][2]) && board[i][0] != ' ')
        {
            winner = (board[i][0] == 'X') ? PXW : P0W;
            gameRunning = 0;
            break;
        }

        if ((board[0][i] == board[1][i] && board[1][i] == board[2][i]) && board[0][i] != ' ')
        {
            winner = (board[0][i] == 'X') ? PXW : P0W;
            gameRunning = 0;
            break;
        }
    }

    if (winner == NONE)
    {
        if ((board[0][0] == board[1][1] && board[1][1] == board[2][2]) && board[0][0] != ' ')
        {
            winner = (board[0][0] == 'X') ? PXW : P0W;
            gameRunning = 0;
        }
        else if ((board[0][2] == board[1][1] && board[1][1] == board[2][0]) && board[0][2] != ' ')
        {
            winner = (board[0][2] == 'X') ? PXW : P0W;
            gameRunning = 0;
        }
    }

    if (winner == NONE)
    {
        int full = 1;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (board[i][j] == ' ')
                {
                    full = 0;
                    break;
                }
            }
        }

        if (full)
        {
            winner = DRAW;
            gameRunning = 0;
        }
    }

    if (winner != NONE)
    {
        if (winner == DRAW)
        {
            printf("\n\n!!! It's a Draw !!!\n\n");
        }
        else
        {
            printf("\n\n!!! The Winner is: %s !!!\n\n", winner == PXW ? "X" : "0");
        }

        sleep(3);
    }

    return gameRunning;
}
