#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include <limits>

#include "RTScene.h"
#include "Intersection.h"

#ifndef __RAY_H__
#define __RAY_H__

struct Ray {
    glm::vec3 p0;  // basepoint (position)
    glm::vec3 dir; // direction
};

namespace RayTracer {
    void Raytrace(Camera cam, RTScene scene, Image &image);
    Ray RayThruPixel(Camera cam, int i, int j, int width, int height);
    Intersection Intersect(Ray ray, Triangle triangle);
    Intersection Intersect(Ray ray, RTScene scene);
    glm::vec3 FindColor(Intersection hit, int recursion_depth);
};

void RayTracer::Raytrace(Camera cam, RTScene scene, Image &image) {
//    std::cout << "max float: " << std::numeric_limits<float>::max() << std::endl;
//
//    int w = image.width; int h = image.height;
//
//    std::cout << "image width: " << w << ", image height: " << h << std::endl;
//     for (int j=0; j<h; j++){
//         for (int i=0; i<w; i++){
//             std::cout << "Calling RayThruPixel..." << std::endl;
//             Ray ray = RayThruPixel( cam, i, j, w, h );
//             std::cout << "Calling Intersect..." << std::endl;
//             Intersection hit = Intersect( ray, scene );
//             std::cout << "Calling FindColor..." << std::endl;
//             image.pixels[j*w + i] = FindColor( hit, 6 );
//
//             image.pixels[j*w + i] = glm::vec3(0.2f, 0.375f, 0.35f);
//         }
//     }
//    std::cout << "Raytrace finished..." << std::endl;
}

Ray RayTracer::RayThruPixel(Camera cam, int i, int j, int width, int height) {
    float alpha = 2*(i+0.5f)/width-1;
    float beta = 1-2*(j+0.5f)/height;
    
    glm::vec3 w = glm::normalize(cam.eye-cam.target);
    glm::vec3 u = glm::normalize(glm::cross(cam.up, w));
    glm::vec3 v = glm::normalize(glm::cross(w, u));
    
    Ray ray;
    ray.p0 = cam.eye;
    ray.dir = glm::normalize(alpha*cam.aspect*glm::tan(cam.fovy/2)*u+beta*glm::tan(cam.fovy/2)*v-w);
    return ray;
}

Intersection RayTracer::Intersect(Ray ray, Triangle triangle) {
    //lambda 1,2,3 and t
    glm::vec4 res = glm::inverse(glm::mat4(
                                           glm::vec4(triangle.P[0], 1),
                                           glm::vec4(triangle.P[1], 1),
                                           glm::vec4(triangle.P[2], 1),
                                           glm::vec4(-ray.dir, 0)
                                           )) * glm::vec4(ray.p0, 1);

    Intersection intersect;
    intersect.P = res[0]*triangle.P[0] + res[1]*triangle.P[1] + res[2]*triangle.P[2];
    intersect.N = glm::normalize(res[0]*triangle.N[0] + res[1]*triangle.N[1] + res[2]*triangle.N[2]);
    intersect.V = glm::normalize(-ray.dir);
    intersect.triangle = &triangle;
    intersect.dist = res[3];
    
    //TODO: check no intersection hit?
    if (res[0] < 0 || res[1] < 0 || res[2] < 0 || res[3] < 0) {
        intersect.dist = std::numeric_limits<float>::max();
    }

    return intersect;
}

Intersection RayTracer::Intersect(Ray ray, RTScene scene) {
    std::cout << "In Intersect..." << std::endl;

    float mindist = std::numeric_limits<float>::max();
//    float mindist = 1000000.0f;

    Intersection hit;
    hit.dist = 1000.0f;
    for (Triangle object : scene.triangle_soup) { // Find closest intersection; test all objects
//        std::cout << "Calling Intersect helper..." << std::endl;
        Intersection hit_temp = Intersect(ray, object);
        if (hit_temp.dist < mindist) { // closer than previous hit
            mindist = hit_temp.dist;
            hit = hit_temp;
        }
    }
    
    std::cout << "Returning a hit..." << std::endl;
    std::cout << "Intersect dist: " << hit.dist << std::endl;

    return hit;
}

//TODO: simple color = lambert's cosine law?, or ambient+lambertian-diffuse+blinn-phong (hw3)
glm::vec3 RayTracer::FindColor(Intersection hit, int recursion_depth) {
    //TODO: check if intersection exists
    if (hit.dist >= std::numeric_limits<float>::max()) {
        return glm::vec3(0.0f, 0.0f, 0.0f); //black
    }
    
    return glm::vec3(0.2f, 0.375f, 0.35f);

}

#endif
