#include "FadeAffector.h"

FadeAffector::FadeAffector(const bool p_bFadeOut)
:
m_bFadeOut(p_bFadeOut)
{
}

FadeAffector::~FadeAffector()
{

}

void FadeAffector::Apply(float dt, Particle* p)
{
    float fStart, fEnd;
    if (m_bFadeOut)
    {
        fStart = 1.0f;
        fEnd = 0.0f;
    }
    else 
    {
        fStart = 0.0f;
        fEnd = 1.0f;
    }
    
    if (p->age < p->lifeTime)
    {
        float t = p->age / p->lifeTime;
        
        p->affectorFade = wolf::lerp(fStart, fEnd, t);

    }
    else
    {
        p->affectorFade = fEnd;
    }
    
}