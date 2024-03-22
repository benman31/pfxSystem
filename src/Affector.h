#pragma once

#include "../wolf/wolf.h"
#include "Particle.h"

class Affector
{
    public:

    Affector(){};
    virtual ~Affector(){};

    virtual void Apply(float dt, Particle* p) = 0;

};


//Scale

//Fade


//Optional: ApplyGravity