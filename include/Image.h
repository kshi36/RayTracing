/**
Image class
*/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <vector>
#include <string>

#ifndef __IMAGE_H__
#define __IMAGE_H__

class Image {
public:
    int width, height;
    std::vector<glm::vec3> pixels;      // RBG colors
    
    // constructor
    Image(const int width, const int height);
    
    void init(void);
    void draw(void);
    
private:
    unsigned int fbo;    // frame buffer obj
    unsigned int tbo;    // texture buffer obj
};

#endif
