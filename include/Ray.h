#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include <math.h>
#include <glm/gtx/string_cast.hpp>

#include "RTScene.h"
#include "Intersection.h"

#ifndef __RAY_H__
#define __RAY_H__

const float MY_INFINITY = 9999999999999999999.9f;

struct Ray {
    glm::vec3 p0;  // basepoint (position)
    glm::vec3 dir; // direction
};

namespace RayTracer {
    void Raytrace(Camera *cam, RTScene &scene, Image &image);
    Ray RayThruPixel(Camera *cam, int i, int j, int width, int height);
    Intersection Intersect(Ray &ray, Triangle &triangle);
    Intersection Intersect(Ray &ray, RTScene &scene);
    glm::vec3 FindColor(Intersection &hit, RTScene &scene, int recursion_depth);
};

//bool eq_float(float x, float y, float epsilon=0.01f) {
//    if (fabs(x-y) < epsilon) return true;
//    return false;
//}

void RayTracer::Raytrace(Camera *cam, RTScene &scene, Image &image) {
    int w = image.width; int h = image.height;

//    std::cout << "image width: " << w << ", image height: " << h << std::endl;
     for (int j=0; j<h; j++){
         for (int i=0; i<w; i++){
             Ray ray = RayThruPixel( cam, i, j, w, h );
             Intersection hit = Intersect( ray, scene );
             image.pixels[(h-j-1)*w + i] = FindColor( hit, scene, 2 );
//             std::cout << "pixel[" << i << "][" << j << "] finished" << std::endl;
         }
     }
    std::cout << "Raytrace finished..." << std::endl;
}

Ray RayTracer::RayThruPixel(Camera *cam, int i, int j, int width, int height) {
    float alpha = 2*((i+0.5f)/width)-1;
    float beta = 1-(2*((j+0.5f)/height));
    float angle = (cam->fovy) * M_PI/180.0f; // convert to radians

    glm::vec3 w = glm::normalize(cam->eye - cam->target);
    glm::vec3 u = glm::normalize(glm::cross(cam->up, w));
    glm::vec3 v = glm::normalize(glm::cross(w, u));
    
    Ray ray;
    //in world coordinate system!
    ray.p0 = cam->eye;
    ray.dir = glm::normalize((alpha*(cam->aspect)*glm::tan(angle/2)*u) + (beta*glm::tan(angle/2)*v) - w);
    
//    std::cout << "ray.p0: " << glm::to_string(ray.p0) << ", ray.dir: " << glm::to_string(ray.dir) << std::endl;
    return ray;
}

Intersection RayTracer::Intersect(Ray &ray, Triangle &triangle) {
    //lambda 1,2,3 and t
    glm::vec4 res = glm::inverse(glm::mat4(
                                           glm::vec4(triangle.P[0], 1.0f),
                                           glm::vec4(triangle.P[1], 1.0f),
                                           glm::vec4(triangle.P[2], 1.0f),
                                           glm::vec4(-ray.dir, 0.0f)
                                           )) * glm::vec4(ray.p0, 1.0f);
    
//    std::cout << "res: " << glm::to_string(res) << std::endl;

    Intersection intersect;
    intersect.P = res[0]*triangle.P[0] + res[1]*triangle.P[1] + res[2]*triangle.P[2];
    intersect.N = glm::normalize(res[0]*triangle.N[0] + res[1]*triangle.N[1] + res[2]*triangle.N[2]);
    intersect.V = -ray.dir;
    intersect.triangle = &triangle;
    //check no intersection hit?
    if (res[0] < 0 || res[1] < 0 || res[2] < 0 || res[3] < 0) {
        intersect.dist = MY_INFINITY;
//        std::cout << "NO INTERSECTION!" << std::endl;
//        std::cout << "no intersect dist: " << intersect.dist << std::endl;
    }
    else {
        intersect.dist = res[3];
//        std::cout << "YES INTERSECTION!" << std::endl;
//        std::cout << "intersect dist: " << intersect.dist << std::endl;
    }
    return intersect;
}

Intersection RayTracer::Intersect(Ray &ray, RTScene &scene) {
    float mindist = MY_INFINITY;

    Intersection hit;
    hit.dist = mindist;
    for (Triangle &object : scene.triangle_soup) { // Find closest intersection; test all objects
        Intersection hit_temp = Intersect(ray, object);
        
//        std::cout << "hit_temp dist: " << hit_temp.dist << std::endl;
        
        if (hit_temp.dist < mindist) { // closer than previous hit
            mindist = hit_temp.dist;
            hit = hit_temp;
        }
    }
    
//    std::cout << "hit dist: " << hit.dist << std::endl;
    
    return hit;
}

glm::vec3 RayTracer::FindColor(Intersection &hit, RTScene &scene, int recursion_depth) {
    //recursion base case
    if (recursion_depth <= 0) {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
    
    //intersection doesn't exist (infinity)
    if (fabs(hit.dist-MY_INFINITY) < 0.1f) {
//        std::cout << "background" << std::endl;
        return glm::vec3(0.1f, 0.2f, 0.3f); //background color
    }
    
    //add light on intersection hit
//    glm::vec3 fragColor = glm::vec3(hit.triangle->material->emision);
    glm::vec3 fragColor = glm::vec3(0.0f);
    
    for ( std::pair<std::string, Light*> light : scene.light ) {
        glm::vec3 l_vec = glm::normalize(glm::vec3(((light.second)->position)[0],
                                              ((light.second)->position)[1],
                                              ((light.second)->position)[2])
                                    - ((light.second)->position)[3]*hit.P);
        
        glm::vec3 half_vec = glm::normalize(hit.V + l_vec);
        
        //SHADOWS
        //generate secondary rays to all lights, add shadow when there is hit btwn light source and a scene object
        float visible = 1.0f;
        Ray shadowray;
        shadowray.p0 = hit.P + 0.1f * hit.N;        //jitter hit pos along unit normal of hit triangle
        shadowray.dir = glm::normalize(glm::vec3(((light.second)->position)[0],
                                           ((light.second)->position)[1],
                                           ((light.second)->position)[2]) - shadowray.p0);
        //obstructed by a scene object (towards light)
        Intersection shadowhit = Intersect(shadowray, scene);
        if (!(fabs(shadowhit.dist-MY_INFINITY) < 0.1f)) {
            visible = 0.0f;
        }
                
        fragColor += glm::vec3((light.second)->color *   //light source color
                      (hit.triangle->material->ambient +    //ambient
                       (hit.triangle->material->diffuse)*glm::max(glm::dot(hit.N, l_vec),0.0f)*visible));   //diffuse
        
        //TODO: RECURSIVE MIRROR REFLECTION
        //generate mirror-reflected ray
        Ray ray2;
        ray2.p0 = hit.P;
        ray2.dir = glm::normalize(2.0f*glm::dot(hit.N, hit.V)*hit.N - hit.V);   //mirror reflection direction

        Intersection hit2 = Intersect( ray2, scene );
        
        //hit2 hits a scene object
        if (!(fabs(hit2.dist-MY_INFINITY) < 0.1f)) {
            fragColor += (visible*(glm::vec3(hit.triangle->material->specular) * FindColor( hit2, scene, recursion_depth-1 )));
        }
//        else {
//            std::cout << "no intersection for reflection!" << std::endl;
//        }
    }
    return glm::vec3(fragColor);
//    return glm::vec3(0.0f, 0.0f, 0.0f); //black
}

#endif
