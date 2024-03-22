#include "AddVelocityAffector.h"

AddVelocityAffector::AddVelocityAffector(const glm::vec3& p_vVel)
:
m_bRandomVelocity(false),
m_vVelocity(p_vVel),
m_vVelocityMin(glm::vec3(0.0f, 0.0f, 0.0f)),
m_vVelocityMax(glm::vec3(0.0f, 0.0f, 0.0f))
{

}

AddVelocityAffector::AddVelocityAffector(const glm::vec3& p_vMin, const glm::vec3& p_vMax)
:
m_bRandomVelocity(true),
m_vVelocity(glm::vec3(0.0f, 0.0f, 0.0f)),
m_vVelocityMin(p_vMin),
m_vVelocityMax(p_vMax)
{

}

AddVelocityAffector::~AddVelocityAffector()
{

}

void AddVelocityAffector::Apply(float dt, Particle* p)
{
    if (m_bRandomVelocity)
    {
        p->position += (wolf::randVec3(m_vVelocityMin, m_vVelocityMax)) * dt;
    }
    else
    {
        p->position += m_vVelocity * dt;
    }
}