#pragma once

#include "../wolf/wolf.h"
#include "Affector.h"

class FadeAffector : public Affector
{
    public:
    FadeAffector(const bool p_bFadeOut);
    ~FadeAffector();

    virtual void Apply(float dt, Particle* p);

    private:

    bool m_bFadeOut;

};