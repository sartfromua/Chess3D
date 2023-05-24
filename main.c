#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "tex.h"
#include "moves.h"

/*
Figures:
1 - pawn
2 - rook
3 - horse
4 - bishop
5 - queen
6 - king
*/

struct SCamera {
    float x, y, z;
    float xAngle, zAngle;
} camera;

struct Pos {
    double x, y, z;
} camera_pos = {0, 0, 3};
struct Pos cursor_pos;

HWND hwnd;

unsigned int chosen_pos_texture = {0, 0};
float vert_plate[] = {1,1,0, 1,-1,0, -1,-1,0, -1,1,0};
float vert_figure[] = {0.5,0.5,0, -0.5,-0.5,0, -0.5,-0.5,2.5, 0.5,0.5,2.5};
float tex_vert[] = {0,1, 1,1, 1,0, 0,0};
GLuint texInd[] = {1, 2, 3, 4};
int texIndCount = sizeof(texInd) / sizeof(GLuint);
int lbutton_down = 0;
unsigned int textureId;
unsigned int tex_indexes_digits[8];
unsigned int tex_indexes_letters[8];
int move_turn = 1;

POINT base_cursor = {0, 0};
POINT chosen_coord;
Figure figures[32];


void DrawFigure(Figure figure) {

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, figure.texture);
        glColor4f(0.95,0.95,0.95, 0.5);

        glTranslatef(figure.x*2-10, figure.y*2-10, 0);
        glVertexPointer(3, GL_FLOAT, 0, &vert_figure);
        glTexCoordPointer(2, GL_FLOAT, 0, &tex_vert);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glRotatef(90, 0,0,1);
        glVertexPointer(3, GL_FLOAT, 0, &vert_figure);
        glTexCoordPointer(2, GL_FLOAT, 0, &tex_vert);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glPopMatrix();
    glDisableClientState(GL_VERTEX_ARRAY);

    glBindTexture(GL_TEXTURE_2D, 0);
}


void LoadIndexTextures(){
    Tex_LoadTexture("textures/1.png", &tex_indexes_digits[0], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/2.png", &tex_indexes_digits[1], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/3.png", &tex_indexes_digits[2], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/4.png", &tex_indexes_digits[3], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/5.png", &tex_indexes_digits[4], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/6.png", &tex_indexes_digits[5], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/7.png", &tex_indexes_digits[6], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/8.png", &tex_indexes_digits[7], GL_REPEAT, GL_NEAREST, NULL);

    Tex_LoadTexture("textures/A.png", &tex_indexes_letters[0], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/B.png", &tex_indexes_letters[1], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/C.png", &tex_indexes_letters[2], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/D.png", &tex_indexes_letters[3], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/E.png", &tex_indexes_letters[4], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/F.png", &tex_indexes_letters[5], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/G.png", &tex_indexes_letters[6], GL_REPEAT, GL_NEAREST, NULL);
    Tex_LoadTexture("textures/H.png", &tex_indexes_letters[7], GL_REPEAT, GL_NEAREST, NULL);

    Tex_LoadTexture("textures/circled_square.png", &chosen_pos_texture, GL_REPEAT, GL_NEAREST, NULL);
}


void DrawIndexes(){
    float vert_index[] = {0,1,0, 1,1,0, 1,-1,0, 0,-1,0};


    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glColor3f(1, 1, 1);
        // Цифры
        glEnable(GL_TEXTURE_2D);
        for (int i=0; i<8; i++) {
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, tex_indexes_digits[i]);

            glTranslatef(0*2-10, (i+1)*2-10, 0);
            glRotatef(180, 0,0,1);
            glRotatef(180, 0,1,0);
            glVertexPointer(3, GL_FLOAT, 0, &vert_index);
            glTexCoordPointer(2, GL_FLOAT, 0, &tex_vert);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }

        for (int i=0; i<8; i++) {
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, tex_indexes_digits[i]);
            glRotatef(180, 0,0,1);

            glTranslatef(0*2-10+2, (i+1)*2-10+2, 0);
            glRotatef(180, 0,0,1);
            glRotatef(180, 0,1,0);
            glVertexPointer(3, GL_FLOAT, 0, &vert_index);
            glTexCoordPointer(2, GL_FLOAT, 0, &tex_vert);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }

        float vert_index_numbers[] = {-1,1,0, 1,1,0, 1,0,0, -1,0,0};
        // Буквы
        for (int i=0; i<8; i++) {
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, tex_indexes_letters[i]);

            glTranslatef((i+1)*2-10, (0)*2-10+1, 0);
            glRotatef(180, 0,0,1);
            glRotatef(180, 0,1,0);
            glVertexPointer(3, GL_FLOAT, 0, &vert_index_numbers);
            glTexCoordPointer(2, GL_FLOAT, 0, &tex_vert);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }

        for (int i=0; i<8; i++) {
            glPushMatrix();
            glBindTexture(GL_TEXTURE_2D, tex_indexes_letters[i]);
            glRotatef(180, 0,0,1);


            glTranslatef((i+1)*2-10+2, (0)*2-10+3, 0);
            glRotatef(180, 0,0,1);
            glRotatef(180, 0,1,0);
            glVertexPointer(3, GL_FLOAT, 0, &vert_index_numbers);
            glTexCoordPointer(2, GL_FLOAT, 0, &tex_vert);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();
        }



    glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void setStartFigures() {
    for (int i = 0; i<8; i++) {
        figures[i].name = 1;
        figures[i].x = i+1;
        figures[i].y = 2;
        figures[i].isActive = 1;
        figures[i].team = 1;
        Tex_LoadTexture("textures/pawn_white.png", &figures[i].texture, GL_REPEAT, GL_NEAREST, NULL);

        figures[31-i].name = 1;
        figures[31-i].x = i+1;
        figures[31-i].y = 7;
        figures[31-i].isActive = 1;
        figures[31-i].team = 0;
        Tex_LoadTexture("textures/pawn_black.png", &figures[31-i].texture, GL_REPEAT, GL_NEAREST, NULL);
    }

    for (int i=8; i < 10; i++) {
        figures[i].name = 2;
        figures[i].x = i ==9 ? 1 : 8;
        figures[i].y = 1;
        figures[i].isActive = 1;
        figures[i].team = 1;
        Tex_LoadTexture("textures/rook_white.png", &figures[i].texture, GL_REPEAT, GL_NEAREST, NULL);

        figures[31-i].name = 2;
        figures[31-i].x = i == 9 ? 1 : 8;
        figures[31-i].y = 8;
        figures[31-i].isActive = 1;
        figures[31-i].team = 0;
        Tex_LoadTexture("textures/rook_black.png", &figures[31-i].texture, GL_REPEAT, GL_NEAREST, NULL);
    }

    for (int i=10; i < 12; i++) {
        figures[i].name = 3;
        figures[i].x = i == 10 ? 2 : 7;
        figures[i].y = 1;
        figures[i].isActive = 1;
        figures[i].team = 1;
        Tex_LoadTexture("textures/horse_white.png", &figures[i].texture, GL_REPEAT, GL_NEAREST, NULL);

        figures[31-i].name = 3;
        figures[31-i].x = i == 10 ? 2 : 7;
        figures[31-i].y = 8;
        figures[31-i].isActive = 1;
        figures[31-i].team = 0;
        Tex_LoadTexture("textures/horse_black.png", &figures[31-i].texture, GL_REPEAT, GL_NEAREST, NULL);
    }

    for (int i=12; i < 14; i++) {
        figures[i].name = 4;
        figures[i].x = i == 12 ? 3 : 6;
        figures[i].y = 1;
        figures[i].isActive = 1;
        figures[i].team = 1;
        Tex_LoadTexture("textures/bishop_white.png", &figures[i].texture, GL_REPEAT, GL_NEAREST, NULL);

        figures[31-i].name = 4;
        figures[31-i].x = i == 12 ? 3 : 6;
        figures[31-i].y = 8;
        figures[31-i].isActive = 1;
        figures[31-i].team = 0;
        Tex_LoadTexture("textures/bishop_black.png", &figures[31-i].texture, GL_REPEAT, GL_NEAREST, NULL);
    }

    int i = 14;
    figures[i].name = 5;
    figures[i].x = 4;
    figures[i].y = 1;
    figures[i].isActive = 1;
    figures[i].team = 1;
    Tex_LoadTexture("textures/queen_white.png", &figures[i].texture, GL_REPEAT, GL_NEAREST, NULL);

    figures[31-i].name = 5;
    figures[31-i].x = 4;
    figures[31-i].y = 8;
    figures[31-i].isActive = 1;
    figures[31-i].team = 0;
    Tex_LoadTexture("textures/queen_black.png", &figures[31-i].texture, GL_REPEAT, GL_NEAREST, NULL);

    i++;
    figures[i].name = 6;
    figures[i].x = 5;
    figures[i].y = 1;
    figures[i].isActive = 1;
    figures[i].team = 1;
    Tex_LoadTexture("textures/king_white.png", &figures[i].texture, GL_REPEAT, GL_NEAREST, NULL);

    figures[31-i].name = 6;
    figures[31-i].x = 5;
    figures[31-i].y = 8;
    figures[31-i].isActive = 1;
    figures[31-i].team = 0;
    Tex_LoadTexture("textures/king_black.png", &figures[31-i].texture, GL_REPEAT, GL_NEAREST, NULL);
}


void Game_Init() {
    // Заполняем массив фигур (комманды, позиции, названия, текстуры)
    setStartFigures();
    // Заполняем массив текстур индексов доски  и выбраной клетки
    LoadIndexTextures();

    // Настройка начального положения камеры
    glLoadIdentity();
    glFrustum(-1,1, -1,1, 2,100);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.3);
}


void ShowDesk() {

    glEnableClientState(GL_VERTEX_ARRAY);
        // Клетки
        glVertexPointer(3, GL_FLOAT, 0, &vert_plate);
        for (int i=-4; i < 4; i++) {
            for (int j=-4; j < 4; j++) {
                glPushMatrix();
                    if ((i+j)%2 == 0) glColor3f(0,0,0);
                    else glColor3f(1,1,1);
                    glTranslatef(i*2, j*2, 0);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glPopMatrix();
            }
        }

        // Подсвечивание выбранной клетки
        if (chosen_coord.x && chosen_coord.y &&
            (chosen_coord.x >= 1 && chosen_coord.x <= 8 && chosen_coord.y >= 1 && chosen_coord.y <= 8)) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, chosen_pos_texture);
            glColor4f(0.95,0.95,0.95, 0.5);

            glPushMatrix();
                int x1 = chosen_coord.x;
                int y1 = chosen_coord.y;
                glTranslatef((int) (x1-5)*2, (float) (y1-5)*2, 0.001);

                glVertexPointer(3, GL_FLOAT, 0, &vert_plate);
                glTexCoordPointer(2, GL_FLOAT, 0, &tex_vert);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glPopMatrix();

            glDisableClientState(GL_VERTEX_ARRAY);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Разметка 1-8, A-H
        DrawIndexes();

        // Фигуры
        for (int i=0; i<32; i++) {
            if (figures[i].isActive) DrawFigure(figures[i]);
        }
    glDisableClientState(GL_VERTEX_ARRAY);
}


void ClientToOpenGlCoords(int x, int y, double *ox, double *oy, double *oz) {
    GLint vp[4];
    GLdouble mMatrix[16], pMatrix[16];
    float z;

    glPushMatrix();
    MoveCamera();

        glGetDoublev(GL_MODELVIEW_MATRIX, mMatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, pMatrix);

        glGetIntegerv(GL_VIEWPORT, vp);
        y = vp[3] - y;

        glReadPixels(x, y, 1,1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
        gluUnProject(x, y, z, mMatrix, pMatrix, vp, ox, oy, oz);

    glPopMatrix();
}


void CameraRotation(float xAngle, float zAngle) {
    camera.zAngle += zAngle;
    if (camera.zAngle < 0) camera.zAngle += 360;
    if (camera.zAngle > 360) camera.zAngle -= 360;
    camera.xAngle += xAngle;
    if (camera.xAngle < 0) camera.xAngle = 0;
    if (camera.xAngle > 180) camera.xAngle = 180;
}


void RotateCamera() {
    POINT cursor;
    GetCursorPos(&cursor);

    CameraRotation((base_cursor.y - cursor.y) / 4, (base_cursor.x - cursor.x) / 4);

    SetCursorPos(base_cursor.x, base_cursor.y);
}


void MoveCamera() {
    if (lbutton_down == 1) RotateCamera();

    float angle = -camera.zAngle / 180 * M_PI;
    float speed = 0;
    float speed_boost = 0.1;
    if (GetKeyState('W') < 0) speed = speed_boost;
    else if (GetKeyState('S') < 0) speed = -speed_boost;
    else if (GetKeyState('A') < 0) {speed = speed_boost; angle -= M_PI/2;}
    else if (GetKeyState('D') < 0) {speed = speed_boost; angle += M_PI/2;}
    if (GetKeyState(VK_SPACE) < 0) camera_pos.z += speed_boost;
    if (GetKeyState(VK_SHIFT) < 0) camera_pos.z -= speed_boost;
    if (GetKeyState('S') < 0) speed = -speed_boost;
    if (speed != 0) {
        camera_pos.x += sin(angle) * speed;
        camera_pos.y += cos(angle) * speed;
    }

    // TransformedVector = TranslationMatrix * RotationMatrix * ScaleMatrix * OriginalVector;
    // TranslationMatrix^-1 * RotationMatrix^-1 * TransformedVector = OriginalVector;   ScaleMatrix = 1;
    glRotatef(-camera.xAngle, 1,0,0);
    glRotatef(-camera.zAngle, 0,0,1);
    glTranslatef(-camera_pos.x, -camera_pos.y, -camera_pos.z);
}


void WndResize (int x, int y) {
    glViewport(0, 0, x,y);
    float k = x / (float) y;
    float sz = 0.1;
    glLoadIdentity();
    glFrustum(-k*sz, k*sz, -sz,sz, sz*2, 100);
}


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    Game_Init();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.6f, 0.8f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            glPushMatrix();

                MoveCamera();
                ShowDesk();


            glPopMatrix();

            SwapBuffers(hDC);

            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_SIZE:
            WndResize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_LBUTTONDOWN:
            system("cls");
            POINT fromPos = {chosen_coord.x, chosen_coord.y};
            ClientToOpenGlCoords(LOWORD(lParam), HIWORD(lParam), &cursor_pos.x, &cursor_pos.y, &cursor_pos.z);
            chosen_coord.x = (cursor_pos.x+11)/2;
            chosen_coord.y = (cursor_pos.y+11)/2;
            //printf("\n%d, %d", chosen_coord.x, chosen_coord.y);

            if (fromPos.x && fromPos.y) {
                if (move_turn == getTeamOnPlate(fromPos.x, fromPos.y, figures)){
                    if (!moveFigureTo(fromPos.x, fromPos.y, chosen_coord.x, chosen_coord.y, figures)) {
                        printf("\nCan't go there!");
                    } else {
                        printf("\nFrom: %d %d \nTo: %d %d", fromPos.x, fromPos.y, chosen_coord.x, chosen_coord.y);
                        move_turn = fabs(move_turn - 1);
                    }
                    kingCheckCheck(figures);
                    checkmateCheck(figures);
                }
                chosen_coord.x = 0;
                chosen_coord.y = 0;
            }

            //for (int i=0; i<32; i+=2) {
            //    printf("\n(%d %d)  (%d %d)", figures[i].x, figures[i].y, figures[i+1].x, figures[i+1].y);
            //}
            // Движение камерой
            GetCursorPos(&base_cursor);
            ShowCursor(FALSE);
            lbutton_down = 1;
        break;

        case WM_LBUTTONUP:

            // Движение камерой
            lbutton_down = 0;
            ShowCursor(TRUE);
        break;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {

                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

