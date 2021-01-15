#include "sampleShadowMaps.h"

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

SampleShadowMaps::~SampleShadowMaps()
{
	printf("Destroying Shadow Maps Sample\n");
    wolf::MaterialManager::DestroyMaterial(m_pMat);
    wolf::TextureManager::DestroyTexture(m_pTex);
    delete m_pModel;
    delete m_pGrid;
    delete m_pOrbitCam;
    delete m_pLight1Dbg;
    delete m_pLight2Dbg;
}

void SampleShadowMaps::init()
{
    const glm::vec3 LIGHT1_COLOR(1.0f,1.0f,0.0f);
    const glm::vec3 LIGHT2_COLOR(1.0f,0.0f,0.0f);

	// Only init if not already done
    if(!m_pModel)
    {
        m_pTex = wolf::TextureManager::CreateTexture("data/diffuseGray.png");
        m_pTex->SetWrapMode(wolf::Texture::WM_Repeat);

        const std::string MATNAME = "SampleShadowMaps";
        m_pMat = wolf::MaterialManager::CreateMaterial(MATNAME);
        m_pMat->SetProgram("data/twoPointLights.vsh", "data/twoPointLights.fsh");
        m_pMat->SetDepthTest(true);
        m_pMat->SetDepthWrite(true);

        m_pMat->SetUniform("u_light1Color", LIGHT1_COLOR);
        m_pMat->SetUniform("u_light1Attenuation", glm::vec3(0.0f,0.3f,0.0f));
        m_pMat->SetUniform("u_light2Color", LIGHT2_COLOR);
        m_pMat->SetUniform("u_light2Attenuation", glm::vec3(0.0f,0.3f,0.0f));

        m_pMat->SetUniform("u_ambientLight", glm::vec3(0.2f,0.2f,0.2f));
        m_pMat->SetUniform("u_specularColor", glm::vec3(1.0f,1.0f,1.0f));
        m_pMat->SetUniform("u_shininess", 200.0f);
        m_pMat->SetUniform("u_diffuseTex", 0);

        SingleMaterialProvider matProvider(MATNAME);
        m_pModel = new wolf::Model("data/unrealSmall.fbx", matProvider);

        glm::vec3 min = m_pModel->getAABBMin();
        glm::vec3 max = m_pModel->getAABBMax();
        glm::vec3 center = m_pModel->getAABBCenter();

        m_pOrbitCam = new OrbitCamera(m_pApp);
        m_pOrbitCam->focusOn(min,max);

        float gridSize = 2.5f * wolf::max(max.x,max.z);
        m_pGrid = new Grid3D(10, gridSize / 10.0f);
        m_pGrid->hideAxes();

        m_pLight1Dbg = new Sphere(0.25f);
        m_pLight1Dbg->SetColor(LIGHT1_COLOR);

        m_pLight2Dbg = new Sphere(0.25f);
        m_pLight2Dbg->SetColor(LIGHT2_COLOR);
    }

    printf("Successfully initialized Shadow Maps Sample\n");
}

void SampleShadowMaps::update(float dt) 
{
    m_pOrbitCam->update(dt);
    m_pGrid->update(dt);

    m_timer += dt;

    float sinTheta = sin(m_timer);
    float cosTheta = cos(m_timer);
    float height = 20.0f * ((1.0f + sinTheta) / 2.0f);
    m_light1Pos = glm::vec3(12.0f * cosTheta, height, 12.0f * sinTheta);
    m_pLight1Dbg->SetPosition(m_light1Pos);

    sinTheta = sin(m_timer + PI);
    cosTheta = cos(m_timer + PI);
    height = 20.0f - 20.0f * ((1.0f + sinTheta) / 2.0f);
    m_light2Pos = glm::vec3(12.0f * cosTheta, height, 12.0f * sinTheta);
    m_pLight2Dbg->SetPosition(m_light2Pos);
}

void SampleShadowMaps::render(int width, int height)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 mProj = m_pOrbitCam->getProjMatrix(width, height);
	glm::mat4 mView = m_pOrbitCam->getViewMatrix();
	glm::mat4 mWorld = glm::rotate(glm::mat4(1.0f), m_rot, glm::vec3(0.0f, 1.0f, 0.0f));

    m_pGrid->render(mView, mProj);

    m_pMat->SetUniform("u_viewPos", m_pOrbitCam->getViewPosition());
    m_pMat->SetUniform("u_light1PosRange", glm::vec4(m_light1Pos, 100.0f));
    m_pMat->SetUniform("u_light2PosRange", glm::vec4(m_light2Pos, 100.0f));

    m_pTex->Bind();
    m_pModel->Render(mWorld, mView, mProj);

    m_pLight1Dbg->Render(mWorld,mView,mProj);
    m_pLight2Dbg->Render(mWorld,mView,mProj);
}

