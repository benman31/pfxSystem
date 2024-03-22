#include "GravityAffector.h"

GravityAffector::GravityAffector()
{
}

GravityAffector::~GravityAffector()
{
}

void GravityAffector::Apply(float dt, Particle* p)
{
    p->velocity.y += (GRAVITY * dt); 
}