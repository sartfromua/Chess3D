#include <gl/gl.h>


#include "stb_image.h"

#include "tex.h"


void Tex_LoadTexture(char *filename, int *textureID, int wrap, int filter, TTexProc proc) {
    int width, height, cnt;
    unsigned char *data = stbi_load(filename, &width, &height, &cnt, 0);
    if (!data)
        printf("File \"%s\" not found!", filename);

    if (proc)
        proc(data, width, height, cnt);

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height,
                                    0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}
