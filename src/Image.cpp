/**
Image class
*/
#include "Image.h"

Image::Image(const int width, const int height) {
    this->width = width;
    this->height = height;
}

void Image::init() {
    glGenFramebuffers(1,&fbo);
    glGenTextures(1,&tbo);
    
    //initialize size of pixels
    pixels = std::vector<glm::vec3>(width*height);
}

void Image::draw() {
    glBindTexture(GL_TEXTURE_2D, tbo);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height, // load texture
                 0,GL_RGB,GL_FLOAT, &pixels[0][0]);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, tbo, 0); // attach texture and the read frame
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);     // if not already so
    glBlitFramebuffer(0,0,width,height,0,0,width,height,GL_COLOR_BUFFER_BIT,
                      GL_NEAREST); // copy framebuffer from read to write
}
