#ifndef TWOKILLONE_SDLPP_IMAGE_H
#define TWOKILLONE_SDLPP_IMAGE_H

#include <sstream>
#include <SDL2/SDL_image.h>
#include "sdlpp.hpp"

class InitImage {
public:
    InitImage(int flags) {
        if ((IMG_Init(flags) & flags) != flags) {
            std::ostringstream sout;
            sout << "InitImage::InitImage(): " << IMG_GetError();
            throw sdl::Error(sout.str());
        }
    }

    ~InitImage() {
        IMG_Quit();
    }

    InitImage(const InitImage &) = delete;
    InitImage(InitImage &&) = delete;
    InitImage &operator=(const InitImage &) = delete;
    InitImage &operator=(InitImage &&) = delete;
};

#endif //TWOKILLONE_SDLPP_IMAGE_H
