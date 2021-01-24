#include "sampleSkinning.h"

const glm::vec3 LIGHT_COLOR(1.0f,1.0f,1.0f);
const glm::vec3 LIGHT_POS(20.0f,20.0f,0.0f);

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
    delete m_pGrid;
    delete m_pOrbitCam;
    delete m_pLightDbg;
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
        m_pMat->SetProgram("data/spotLight.vsh", "data/spotLight.fsh");
        m_pMat->SetDepthTest(true);
        m_pMat->SetDepthWrite(true);

        m_pMat->SetUniform("u_lightPosRange", glm::vec4(LIGHT_POS, 100.0f));
        m_pMat->SetUniform("u_lightColor", LIGHT_COLOR);
        m_pMat->SetUniform("u_lightAttenuation", glm::vec3(0.0f,0.1f,0.0f));

        m_pMat->SetUniform("u_ambientLight", glm::vec3(0.2f,0.2f,0.2f));
        m_pMat->SetUniform("u_specularColor", glm::vec3(1.0f,1.0f,1.0f));
        m_pMat->SetUniform("u_shininess", 200.0f);
        m_pMat->SetUniform("u_diffuseTex", 0);

        const std::string SKINNING_MATNAME = "SampleSkinning_skinning";
        m_pSkinningMat = wolf::MaterialManager::CreateMaterial(SKINNING_MATNAME);
        m_pSkinningMat->SetProgram("data/skinning.vsh", "data/skinning.fsh");
        m_pSkinningMat->SetUniform("u_lightPosRange", glm::vec4(LIGHT_POS, 100.0f));
        m_pSkinningMat->SetUniform("u_lightColor", LIGHT_COLOR);
        m_pSkinningMat->SetUniform("u_lightAttenuation", glm::vec3(0.0f,0.01f,0.0f));
        m_pSkinningMat->SetUniform("u_ambientLight", glm::vec3(0.2f,0.2f,0.2f));
        m_pSkinningMat->SetUniform("u_specularColor", glm::vec3(1.0f,1.0f,1.0f));
        m_pSkinningMat->SetUniform("u_shininess", 200.0f);
        m_pSkinningMat->SetUniform("u_diffuseTex", 0);

        SingleMaterialProvider matProvider(SKINNING_MATNAME);
        m_pModel = new wolf::Model("data/myskeleton.fbx", matProvider);

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

        float gridSize = 2.5f * wolf::max(max.x,max.z);
        m_pGrid = new Grid3D(10, gridSize / 10.0f);
        m_pGrid->hideAxes();

        m_pLightDbg = new Sphere(0.25f);
        m_pLightDbg->SetColor(LIGHT_COLOR);
    }

    printf("Successfully initialized Skinning Sample\n");
}

void SampleSkinning::update(float dt) 
{
    m_pOrbitCam->update(dt);
    m_pGrid->update(dt);

    // m_timer += dt;

    float sinTheta = sin(m_timer);
    float cosTheta = cos(m_timer);
    float height = 20.0f + 20.0f * ((1.0f + sinTheta) / 2.0f);

    m_lightPos = glm::vec3(32.0f * cosTheta, height, 32.0f * sinTheta);
    m_pLightDbg->SetPosition(m_lightPos);

    m_pModel->Update(dt);
}

void SampleSkinning::render(int width, int height)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 mProj = m_pOrbitCam->getProjMatrix(width, height);
	glm::mat4 mView = m_pOrbitCam->getViewMatrix();
	glm::mat4 mWorld = glm::rotate(glm::mat4(1.0f), m_rot, glm::vec3(0.0f, 1.0f, 0.0f));

    // m_pGrid->render(mView, mProj);

    glm::vec3 spotDir = glm::normalize(glm::vec3(0.0f,0.0f,0.0f) - m_lightPos);
    m_pMat->SetUniform("u_viewPos", m_pOrbitCam->getViewPosition());
    m_pMat->SetUniform("u_lightPosRange", glm::vec4(m_lightPos,100.0f));
    m_pMat->SetUniform("u_lightSpot", glm::vec4(spotDir, 10.0f));

    m_pSkinningMat->SetUniform("u_viewPos", m_pOrbitCam->getViewPosition());
    m_pSkinningMat->SetUniform("u_lightPosRange", glm::vec4(m_lightPos,100.0f));
    m_pSkinningMat->SetUniform("u_lightSpot", glm::vec4(spotDir, 10.0f));

    m_pTex->Bind();
    m_pFloor->Render(mWorld, mView, mProj);
    m_pModel->Render(mWorld * glm::scale(glm::mat4(1.0f), glm::vec3(0.25f,0.25f,0.25f)), mView, mProj);

    m_pLightDbg->Render(mWorld,mView,mProj);
}

