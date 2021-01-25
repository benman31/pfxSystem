#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include "../samplefw/Grid3D.h"
#include "../samplefw/OrbitCamera.h"
#include "../samplefw/Sphere.h"

class SampleSkinning: public Sample
{
public:
    SampleSkinning(wolf::App* pApp) : Sample(pApp,"Skinning") {}
    ~SampleSkinning();

    void init() override;
    void update(float dt) override;
    void render(int width, int height) override;

private:
    wolf::Model* m_pFloor = nullptr;
    wolf::SkinnedModel* m_pModel = nullptr;
    wolf::Material* m_pMat = nullptr;
    wolf::Material* m_pSkinningMat = nullptr;
    wolf::Texture* m_pTex = nullptr;
    OrbitCamera* m_pOrbitCam = nullptr;
    Sphere* m_pLightDbg[4] = { nullptr, nullptr, nullptr, nullptr };
    int m_currAnim = 0;
    float m_timer = 0.0f;
    bool m_showLightSpheres = false;
};
