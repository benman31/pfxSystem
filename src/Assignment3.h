#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include "../samplefw/Grid3D.h"
#include "../samplefw/OrbitCamera.h"
#include "../samplefw/Sphere.h"

#include "Effect.h"

class Assignment3: public Sample
{
public:
    Assignment3(wolf::App* pApp) : Sample(pApp,"ParticleEffect") {}
    ~Assignment3();

    void init() override;
    void update(float dt) override;
    void render(int width, int height) override;

private:
    wolf::Model* m_pFloor = nullptr;
    wolf::Material* m_pMat = nullptr;
    wolf::Texture* m_pTex = nullptr;
    OrbitCamera* m_pOrbitCam = nullptr;
    Sphere* m_pLightDbg[4] = { nullptr, nullptr, nullptr, nullptr };
    bool m_showLightSpheres = false;

    
    Effect* m_pFireEffect = nullptr;
    Effect* m_pConfettiEffect = nullptr;
    bool m_bSwapEffects = false;
};
