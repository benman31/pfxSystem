#pragma once

#include "../wolf/wolf.h"
#include "Affector.h"

class GravityAffector : public Affector
{
    public:
    GravityAffector();
    ~GravityAffector();

    virtual void Apply(float dt, Particle* p);

    private:
    
    const float GRAVITY = -9.8f;

};