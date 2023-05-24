#include "moves.h"
#include <stdio.h>
#include <math.h>


int isPlateEmpty(int x, int y, Figure *figures) {
    for (int i=0; i<32; i++){
        if (figures[i].isActive && figures[i].x == x && figures[i].y == y) return 0;
    }
    return 1;
}


int min(int a, int b) {
    if (a < b) return a;
    else return b;
}

int max(int a, int b) {
    if (a > b) return a;
    else return b;
}

int sign(int a) {
    if (a < 0) return -1;
    else return 1;
}


int getTeamOnPlate(int x, int y, Figure *figures){
    for (int i=0; i<32; i++){
        if (figures[i].isActive && figures[i].x == x && figures[i].y == y) return figures[i].team;
    }
    return -1;
}


int isEnemyOnPlate(int x, int y, int ally_team, Figure *figures) {
    if (isPlateEmpty(x ,y, figures)) return 0;
    int team_onPlate;
    for (int i=0; i<32; i++){
        if (figures[i].isActive && figures[i].x == x && figures[i].y == y) {team_onPlate = figures[i].team; break;}
    }
    return 0 ? team_onPlate == ally_team : 1;
}


int getFigureOnPlate(int x, int y, Figure *figures) {
    for (int i=0; i<32; i++){
        if (figures[i].isActive && figures[i].x == x && figures[i].y == y) return i;
    }
    return -1;
}


int gameOverCheck(Figure *figures) {
    // 16, 15 - короли
    if (figures[16].isActive && figures[15].isActive) return -1;
    else if (!figures[16].isActive) return fabs(figures[16].team - 1);
    else if (!figures[15].isActive) return fabs(figures[15].team - 1);
}


int figureCanBeBeaten(Figure figure, Figure* figures) {
    for (int fig=0+16*figure.team; fig < 16+16*figure.team; fig++) {
        if (canFigureGo(figure.x, figure.y, figures[fig], figures)) return 1;
    }
    return 0;
}


int pathCanBeBlocked(Figure figure, int toX, int toY, Figure* figures) {
    for (int ally_fig=0+16*(1-figure.team); ally_fig<16+16*(1-figure.team); ally_fig++) {
        if (figures[ally_fig].name = 6) continue;
        if (canFigureGo(toX, toY, figures[ally_fig], figures)) return 1;
    }
    if (figure.name == 2 || figure.name == 4 || figure.name == 5) {
        int dx = sign(toX - figure.x);
        int dy = sign(toY - figure.y);
        int x = figure.x;
        int y = figure.y;
        while (x + dx <= toX && y + dy <= toY) {
            x += dx;
            y += dy;
            for (int ally_fig=0+16*(1-figure.team); ally_fig<16+16*(1-figure.team); ally_fig++) {
                if (figures[ally_fig].name == 6) continue;
                if (canFigureGo(x, y, figures[ally_fig], figures)) return 1;
            }
        }
    }

    return 0;
}


int checkmateKing(Figure king, Figure* figures){
    if (king.name != 6) {printf("Error with KING!"); return 0;}
    int king_in_danger = figureCanBeBeaten(king, figures);

    if (king_in_danger){
        printf("king in danger!");
        int pos_plates[] = {0,1, 1,1, 1,0, 1,-1, 0,-1, -1,-1, -1,0, -1, 1};
        for (int i=0; i<16; i+=2) {
            int plate_in_danger = 0;
            int x = king.x+pos_plates[i];
            int y = king.y+pos_plates[i+1];
            if (x<1 || y<1 || x>8 || y>8) continue;
            if (!isPlateEmpty(x, y, figures)) continue;
            for (int fig=0+16*king.team; fig<16+16*king.team; fig++) {
                // Если фигура может пойти на клетку возле короля и ее нельзя побить
                if (canFigureGo(x, y, figures[fig], figures) && !figureCanBeBeaten(figures[fig], figures)){
                    plate_in_danger = 1;
                    // Закрытие клетки или пути к этой клетке для каждой вражеской фигуры, что может туда пойти
                    if (pathCanBeBlocked(figures[fig], king.x, king.y, figures)) plate_in_danger = 0;
                }
            }
            if (!plate_in_danger) {printf("\n(%d, %d) plate not in danger\n", x, y); return 0;}
        }
    } else return 0;
    return 1;
}


int kingCheckCheck(Figure* figures) {
    // Черный король
    if (figures[16].name != 6) {printf("Error with KING!"); return 0;}
    if (figureCanBeBeaten(figures[16], figures)) {
        system("cls");
        printf("Check to black team!");
        return 1;
    }

    // Белый король
    if (figures[15].name != 6) {printf("Error with KING!"); return 0;}
    if (figureCanBeBeaten(figures[15], figures)) {
        system("cls");
        printf("Check to white team!");
        return 1;
    }
    return 0;
}


int checkmateCheck(Figure *figures) {
    // Черный король
    if (checkmateKing(figures[16], figures)) {
        system("cls");
        printf("Checkmate to black team!");
        return 1;
    }

    // Белый король
    if (checkmateKing(figures[15], figures)) {
        system("cls");
        printf("Checkmate to white team!");
        return 1;
    }
    return 0;
}


// 1 if figure can get to the plate (may be enemy or ally on that plate)
int canFigureGo(int toX, int toY, Figure figure, Figure *figures) {
    int plFg = getFigureOnPlate(toX, toY, figures);

    switch(figure.name){
    // Pawn
    case 1:
        if (plFg == -1) {
            if (figure.team == 1 && figure.x == toX && figure.y+1 == toY) return 1;
            if (figure.team == 0 && figure.x == toX && figure.y-1 == toY) return 1;
        } else if (figure.team == 1 && figures[plFg].team == 0 && abs(figure.x - toX) == 1 && figure.y+1 == toY) return 1;
        else if (figure.team == 0 && figures[plFg].team == 1 && abs(figure.x - toX) == 1 && figure.y-1 == toY) return 1;
        return 0;
        break;
    // Rook
    case 2:
        if (toX == figure.x){
            for (int y=min(toY, figure.y)+1; y < max(toY, figure.y); y++){
                if (!isPlateEmpty(toX, y, figures)) return 0;
            }
            return 1;
        } else if (toY == figure.y){
            for (int x=min(toX, figure.x)+1; x < max(toX, figure.x); x++){
                if (!isPlateEmpty(x, toY, figures)) return 0;
            }
            return 1;
        } else return 0;
    // Horse
    case 3:
        if ((fabs(figure.x - toX) == 2 && fabs(figure.y - toY) == 1) || (fabs(figure.x - toX) == 1 && fabs(figure.y - toY) == 2)) return 1;
        else return 0;
    // Bishop
    case 4:
        if (fabs(figure.x - toX) == fabs(figure.y - toY)) {
            int dx = sign(toX-figure.x);
            int dy = sign(toY-figure.y);
            int x = figure.x;
            int y = figure.y;
            while (x + dx != toX){
                x += dx;
                y += dy;
                if (!isPlateEmpty(x, y, figures)) return 0;
            }
            return 1;
        } else return 0;
    // Queen
    case 5:
        if (toX == figure.x){
            for (int y=min(toY, figure.y)+1; y < max(toY, figure.y); y++){
                if (!isPlateEmpty(toX, y, figures)) return 0;
            }
            return 1;
        } else if (toY == figure.y){
            for (int x=min(toX, figure.x)+1; x < max(toX, figure.x); x++){
                if (!isPlateEmpty(x, toY, figures)) return 0;
            }
            return 1;
        }
        if (fabs(figure.x - toX) == fabs(figure.y - toY)) {
            int dx = sign(toX-figure.x);
            int dy = sign(toY-figure.y);
            int x = figure.x;
            int y = figure.y;
            while (x + dx != toX){
                x += dx;
                y += dy;
                if (!isPlateEmpty(x, y, figures)) return 0;
            }
            return 1;
        } else return 0;
    // King
    case 6:
        if (fabs(toX - figure.x) <= 1 && fabs(toY - figure.y) <= 1 && (toX != figure.x || toY != figure.y) && plFg == -1) return 1;
        // Рокировка
        if (figureCanBeBeaten(figure, figures)) return 0;
        else if ((figure.x == 5 && figure.y == 1 && figure.team == 1) || (figure.x == 5 && figure.y == 8 && figure.team == 0))
                if (figures[plFg].name == 2 && plFg > 0 && (((figure.team == 1 && (toX == 1 || toX == 8 || toX == 2) && toY == 1)) ||
                                               ((figure.team == 0 && (toX == 1 || toX == 8 || toX == 7) && toY == 8)))) {
                    printf("CASTLING APPROVED");
                    int dx = sign(toX - figure.x);
                    int x = figure.x;
                    while (x + dx != toX){
                        x += dx;
                        if (!isPlateEmpty(x, figure.y, figures)) return 0;
                    }

                    return 2;
        }

            return 0;
    }
    return 0;
}


int moveFigureTo(int fromX, int fromY, int toX, int toY, Figure *figures){
    if (fromX < 1 || fromX > 8 || fromY < 1 || fromY > 8 ||
        toX < 1 || toX > 8 || toY < 1 || toY > 8) return 0;
    if (isPlateEmpty(fromX, fromY, figures)) return 0;

    Figure* activeFigure = NULL;
    for (int i=0; i<32; i++){
        if (figures[i].isActive && figures[i].x == fromX && figures[i].y == fromY) {activeFigure = &figures[i]; break;}
    }
    if (activeFigure == NULL) return 0;

    int plateEmpty = isPlateEmpty(toX, toY, figures);

    if (canFigureGo(toX, toY, *activeFigure, figures))
            if (plateEmpty) {activeFigure->x = toX; activeFigure->y = toY; printf("Step was taken!");}
            else {
                /* Рокировка */
                //printf("Figure can go!");
                int onPlt = getFigureOnPlate(toX, toY, figures);
                if (activeFigure->name == 6) {
                    printf("Castling!");
                    int dx = sign(toX - activeFigure->x);
                    figures[getFigureOnPlate(toX, toY, figures)].x = activeFigure->x+dx;
                    activeFigure->x += dx*2;
                }

                /* Побить фигуру */
                printf("activeFigure team: %d, onPlt team: %d", activeFigure->team, figures[onPlt].team);
                if (activeFigure->team == figures[onPlt].team) return 0;
                figures[onPlt].isActive = 0;
                activeFigure->x = toX;
                activeFigure->y = toY;
                printf("Step was taken!");
            }
    else return 0;

    return 1;
}
