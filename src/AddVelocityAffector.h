#pragma once

#include "../wolf/wolf.h"
#include "Affector.h"

class AddVelocityAffector : public Affector
{
    public:
    AddVelocityAffector(const glm::vec3& p_vVel);
    AddVelocityAffector(const glm::vec3& p_vMin, const glm::vec3& p_vMax);
    ~AddVelocityAffector();

    virtual void Apply(float dt, Particle* p);

    private:

    bool m_bRandomVelocity = false;
    glm::vec3 m_vVelocityMin = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_vVelocityMax = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_vVelocity = glm::vec3(0.0f, 2.0f, 0.0f);

};