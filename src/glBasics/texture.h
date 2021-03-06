#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture
{
public:
    Texture(unsigned int tex = 0, unsigned int texLocation = GL_TEXTURE0)
        : mTex(tex)
        , mTexLocation(texLocation)
        , mShared(false)
    {
    }

    ~Texture()
    {
        if (mTex != 0)
        {
            glDeleteTextures(1, &mTex);
            mTex = 0;
        }
    }

    virtual void use(unsigned int overrideLocation = GL_TEXTURE) = 0;

    unsigned int mTex;
    unsigned int mTexLocation;
    bool mShared;
};

// TEXTURE_H
#endif