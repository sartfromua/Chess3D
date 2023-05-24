#ifndef TEX_H_INCLUDED
#define TEX_H_INCLUDED

typedef void (*TTexProc) ( unsigned char *data, int width, int height, int cnt);

void Tex_LoadTexture(char *filename, int *textureID, int wrap, int filter, TTexProc proc);

#endif // TEX_H_INCLUDED
