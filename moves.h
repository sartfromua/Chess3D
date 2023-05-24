#ifndef MOVES_H_INCLUDED
#define MOVES_H_INCLUDED

/*
@param int x, y - coords on the board
@param int name:
    1 - pawn
    2 - rook
    3 - horse
    4 - bishop
    5 - queen
    6 - king
@param unsigned int texture - texture data
@param int isActive:
    0 - inactive
    1 - active
@param int team:
    0 - black
    1 - white
*/
typedef struct Figure {
    int x, y;
    int name;
    unsigned int texture;
    int isActive;
    int team;
} Figure;

int moveFigureTo(int fromX, int fromY, int toX, int toY, Figure *figures);
int getTeamOnPlate(int x, int y, Figure *figures);
int gameOverCheck(Figure *figures);
int checkmateCheck(Figure *figures);
int kingCheckCheck(Figure* figures);

#endif // MOVES_H_INCLUDED
