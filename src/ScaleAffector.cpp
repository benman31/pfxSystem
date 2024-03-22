#include "ScaleAffector.h"

ScaleAffector::ScaleAffector(const float p_fStart, const float p_fEnd)
:
m_fStart(p_fStart),
m_fEnd(p_fEnd)
{

}

ScaleAffector::~ScaleAffector()
{

}

void ScaleAffector::Apply(float dt, Particle* p)
{
    if (p->age < p->lifeTime)
    {
        float t = p->age / p->lifeTime;
        p->scale = wolf::lerp(m_fStart, m_fEnd, t);
    }
    else
    {
        p->scale = m_fEnd;
    }
    
}