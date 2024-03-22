#pragma once

#include "../wolf/wolf.h"

// Color struct for storage in particle rather than a vec4 so we dont have to cast from float to GLUbyte for each vert during Render
struct Color
{
    GLubyte r,g,b,a;
};

struct Particle
{
    Particle* prev = 0;
    Particle* next = 0;

    Color color = {0, 0, 0, 0};
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    
    float age = 0.0f;

    // Initial size (multiplies a 1 x 1 quad)
    float size = 1.0f;
    
    // Additional scaling added by affector, multiplied at render time
    float scale = 1.0f;

    float lifeTime = 0.0f;
    
    // Initial fade
    float fade = 1.0f;

    // Extra fade multiplier applied by affector
    float affectorFade = 1.0f;
};