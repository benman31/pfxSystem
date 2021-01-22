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
    wolf::Model* m_pModel = nullptr;
    wolf::Material* m_pMat = nullptr;
    wolf::Material* m_pSkinningMat = nullptr;
    wolf::Texture* m_pTex = nullptr;
    Grid3D* m_pGrid = nullptr;
    OrbitCamera* m_pOrbitCam = nullptr;
    float m_rot = 0.0f;
    float m_timer = 0.0f;
    glm::vec3 m_lightPos;
    Sphere* m_pLightDbg = nullptr;
};
