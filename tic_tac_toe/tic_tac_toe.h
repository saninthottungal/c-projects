typedef enum
{
    PX,
    P0,
} Player;

typedef enum
{
    PXW,
    P0W,
    DRAW,
    NONE,
} Winner;

void drawBoard(char board[3][3]);
void takeTurn(Player turn, int *x, int *y);
void updateTakeAndTurn(char board[3][3], Player *turn, int x, int y);
int calculateWinner(char board[3][3]);