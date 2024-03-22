#pragma once

#include "../wolf/wolf.h"
#include "Affector.h"

class ScaleAffector : public Affector
{
    public:
    ScaleAffector(const float p_fStart, const float p_fEnd);
    ~ScaleAffector();

    virtual void Apply(float dt, Particle* p);

    private:

    float m_fStart;
    float m_fEnd;
};