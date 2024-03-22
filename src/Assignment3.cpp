#include "Assignment3.h"

const std::vector<glm::vec3> LIGHT_COLOR = {
    glm::vec3(0.8f,0.9f,0.8f),
    glm::vec3(0.9f,0.9f,0.8f),
    glm::vec3(0.8f,0.9f,0.9f),
};

const std::vector<glm::vec3> LIGHT_POS = {
    glm::vec3(0.0f,30.0f,0.0f),
    glm::vec3(-20.0f,30.0f,20.0f),
    glm::vec3(-20.0f,30.0f,-20.0f),
};

class SingleMaterialProvider : public wolf::Model::MaterialProvider
{
public:
    SingleMaterialProvider(const std::string& matName) : m_matName(matName) { }

    wolf::Material* getMaterial(const std::string& nodeName, int subMeshIndex, const std::string& name) const override
    {
        // Regardless of what mesh index or mat name the model wants, we just
        // use the mat we were seeded with. Note that we create a new one each
        // time so the DestroyMaterial calls line up. This could be improved,
        // but they do share shaders.
        return wolf::MaterialManager::CreateMaterial(m_matName);
    }

private:
    std::string m_matName;
};

Assignment3::~Assignment3()
{
	printf("Destroying Particle Assignment Sample\n");
    wolf::MaterialManager::DestroyMaterial(m_pMat);
    wolf::TextureManager::DestroyTexture(m_pTex);
    delete m_pOrbitCam;

    for(int i = 0; i < LIGHT_POS.size(); ++i)
        delete m_pLightDbg[i];
}

void SetLightsOnMaterial(wolf::Material* pMat)
{
    const int MAX_PROP_NAME_LEN = 64;
    char strBuff[MAX_PROP_NAME_LEN];

    for(int i = 0; i < LIGHT_POS.size(); ++i)
    {
        snprintf(strBuff, MAX_PROP_NAME_LEN, "u_light%dPosRange", i);
        pMat->SetUniform(strBuff, glm::vec4(LIGHT_POS[i], 100.0f));
        snprintf(strBuff, MAX_PROP_NAME_LEN, "u_light%dColor", i);
        pMat->SetUniform(strBuff, LIGHT_COLOR[i]);
        snprintf(strBuff, MAX_PROP_NAME_LEN, "u_light%dAttenuation", i);
        pMat->SetUniform(strBuff, glm::vec3(0.0f,0.1f,0.0f));
    }

    pMat->SetUniform("u_ambientLight", glm::vec3(0.1f,0.1f,0.15f));
}

void Assignment3::init()
{

    if(!m_pFloor)
    {
        m_pTex = wolf::TextureManager::CreateTexture("data/diffuseGray.png");
        m_pTex->SetWrapMode(wolf::Texture::WM_Repeat);

        const std::string MATNAME = "Floor";
        m_pMat = wolf::MaterialManager::CreateMaterial(MATNAME);
        m_pMat->SetProgram("data/standard.vsh", "data/standard.fsh");
        m_pMat->SetUniform("u_specularColor", glm::vec3(1.0f,1.0f,1.0f));
        m_pMat->SetUniform("u_shininess", 100.0f);
        m_pMat->SetUniform("u_diffuseTex", 0);

        SetLightsOnMaterial(m_pMat);

        SingleMaterialProvider matProviderFloor(MATNAME);
        m_pFloor = new wolf::Model("data/plane.fbx", matProviderFloor);
        glm::vec3 min = m_pFloor->getAABBMin();
        glm::vec3 max = m_pFloor->getAABBMax();
        glm::vec3 center = m_pFloor->getAABBCenter();

        m_pOrbitCam = new OrbitCamera(m_pApp);
        m_pOrbitCam->focusOn(min,max);

        for(int i = 0; i < LIGHT_POS.size(); ++i)
        {
            m_pLightDbg[i] = new Sphere(0.25f);
            m_pLightDbg[i]->SetColor(LIGHT_COLOR[i]);
            m_pLightDbg[i]->SetPosition(LIGHT_POS[i]);
        }

        m_pFireEffect = new Effect("data/smokey-fire.effect");
        m_pFireEffect->Init();
        m_pConfettiEffect = new Effect("data/confetti-pop.effect");
        m_pConfettiEffect->Init();
    }

    printf("Successfully initialized Particle Assignment Sample\n");
}

void Assignment3::update(float dt) 
{

    if(m_pApp->isKeyJustDown('2'))
        m_showLightSpheres = !m_showLightSpheres;
    
    if(m_pApp->isKeyJustDown(' '))
        m_bSwapEffects = !m_bSwapEffects;

    m_pOrbitCam->update(dt);
    
    if (m_bSwapEffects)
    {
        m_pConfettiEffect->Update(dt);
    }
    else
    {
        m_pFireEffect->Update(dt);
    }
}

void Assignment3::render(int width, int height)
{
	glClearColor(0.1f, 0.1f, 0.12f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 mProj = m_pOrbitCam->getProjMatrix(width, height);
	glm::mat4 mView = m_pOrbitCam->getViewMatrix();
	glm::mat4 mWorld = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    m_pMat->SetUniform("u_viewPos", m_pOrbitCam->getViewPosition());

    m_pTex->Bind();
    m_pFloor->Render(mWorld, mView, mProj);

    if(m_showLightSpheres)
    {
        for (int i = 0; i < LIGHT_POS.size(); ++i)
        {
            m_pLightDbg[i]->Render(mWorld,mView,mProj);
        }
    }
    if (m_bSwapEffects)
    {
        m_pConfettiEffect->Render(mView, mProj);
    }
    else 
    {
        m_pFireEffect->Render(mView, mProj);
    }
    
}