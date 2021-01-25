#include "sampleSkinning.h"

const std::vector<std::string> ANIM_CLIPS = {
    "idle",
    "walk",
    "attack",
    "attack2",
    "attack3",
    "spawn",
    "defeat",
    "victory",
    "summon",
    "death"
};

const std::vector<glm::vec3> LIGHT_COLOR = {
    glm::vec3(0.9f,0.8f,0.8f),
    glm::vec3(0.9f,0.9f,0.8f),
    glm::vec3(0.8f,0.9f,0.9f),
};

const std::vector<glm::vec3> LIGHT_POS = {
    glm::vec3(20.0f,30.0f,20.0f),
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

SampleSkinning::~SampleSkinning()
{
	printf("Destroying Skinning Sample\n");
    wolf::MaterialManager::DestroyMaterial(m_pMat);
    wolf::TextureManager::DestroyTexture(m_pTex);
    delete m_pModel;
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

void SampleSkinning::init()
{
	// Only init if not already done
    if(!m_pModel)
    {
        m_pTex = wolf::TextureManager::CreateTexture("data/diffuseGray.png");
        m_pTex->SetWrapMode(wolf::Texture::WM_Repeat);

        const std::string MATNAME = "SampleSkinning";
        m_pMat = wolf::MaterialManager::CreateMaterial(MATNAME);
        m_pMat->SetProgram("data/standard.vsh", "data/standard.fsh");
        m_pMat->SetUniform("u_specularColor", glm::vec3(1.0f,1.0f,1.0f));
        m_pMat->SetUniform("u_shininess", 200.0f);
        m_pMat->SetUniform("u_diffuseTex", 0);

        const std::string SKINNING_MATNAME = "SampleSkinning_skinning";
        m_pSkinningMat = wolf::MaterialManager::CreateMaterial(SKINNING_MATNAME);
        m_pSkinningMat->SetProgram("data/skinning.vsh", "data/skinning.fsh");
        m_pSkinningMat->SetUniform("u_specularColor", glm::vec3(1.0f,1.0f,1.0f));
        m_pSkinningMat->SetUniform("u_shininess", 20.0f);
        m_pSkinningMat->SetUniform("u_diffuseTex", 0);

        SetLightsOnMaterial(m_pMat);
        SetLightsOnMaterial(m_pSkinningMat);

        SingleMaterialProvider matProvider(SKINNING_MATNAME);
        m_pModel = new wolf::SkinnedModel("data/skeleton.json", matProvider);
        m_pModel->PlayClip(ANIM_CLIPS[0]);

        glm::vec3 min = m_pModel->getAABBMin();
        glm::vec3 max = m_pModel->getAABBMax();
        glm::vec3 center = m_pModel->getAABBCenter();

        SingleMaterialProvider matProviderFloor(MATNAME);
        m_pFloor = new wolf::Model("data/plane.fbx", matProviderFloor);
        min = m_pFloor->getAABBMin();
        max = m_pFloor->getAABBMax();
        center = m_pFloor->getAABBCenter();

        m_pOrbitCam = new OrbitCamera(m_pApp);
        m_pOrbitCam->focusOn(min,max);

        for(int i = 0; i < LIGHT_POS.size(); ++i)
        {
            m_pLightDbg[i] = new Sphere(0.25f);
            m_pLightDbg[i]->SetColor(LIGHT_COLOR[i]);
            m_pLightDbg[i]->SetPosition(LIGHT_POS[i]);
        }
    }

    printf("Successfully initialized Skinning Sample\n");
}

void SampleSkinning::update(float dt) 
{
    if(m_pApp->isKeyJustDown('1'))
    {
        m_currAnim = (m_currAnim + 1) % ANIM_CLIPS.size();
        m_pModel->PlayClip(ANIM_CLIPS[m_currAnim]);
    }

    if(m_pApp->isKeyJustDown('2'))
        m_showLightSpheres = !m_showLightSpheres;

    m_pModel->Update(dt);
    m_pOrbitCam->update(dt);
}

void SampleSkinning::render(int width, int height)
{
	glClearColor(0.1f, 0.1f, 0.12f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 mProj = m_pOrbitCam->getProjMatrix(width, height);
	glm::mat4 mView = m_pOrbitCam->getViewMatrix();
	glm::mat4 mWorld = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    m_pMat->SetUniform("u_viewPos", m_pOrbitCam->getViewPosition());
    m_pSkinningMat->SetUniform("u_viewPos", m_pOrbitCam->getViewPosition());

    m_pTex->Bind();
    m_pFloor->Render(mWorld, mView, mProj);
    m_pModel->Render(mWorld * glm::scale(glm::mat4(1.0f), glm::vec3(0.25f,0.25f,0.25f)), mView, mProj);

    if(m_showLightSpheres)
    {
        for (int i = 0; i < LIGHT_POS.size(); ++i)
        {
            m_pLightDbg[i]->Render(mWorld,mView,mProj);
        }
    }
}

