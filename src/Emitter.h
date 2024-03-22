#pragma once

#include "../wolf/wolf.h"
#include "tinyxml2.h"
#include "Transform.h"
#include "Particle.h"
#include "Affector.h"
#include "AddVelocityAffector.h"
#include "FadeAffector.h"
#include "ScaleAffector.h"
#include "GravityAffector.h"

struct Vertex
{
    GLfloat x,y,z;
    GLubyte r,g,b,a;

};

class Emitter
{
    public:

    enum class Mode
    {
        eContinuous,
        eBurst
    };

    Emitter(const Transform* p_ParentTransform, const glm::vec3& vOffset, const std::string& strFilePath);
    ~Emitter();

    public:
    void Update(float dt);
    void Render(const glm::mat4& p_mView, const glm::mat4& p_mProj);
    void Init();

    bool CreateEmitterFromXml(const std::string& p_strPath);
    bool CreateAffectorFromXML(tinyxml2::XMLNode* p_pNode);
    
    void SetName(const std::string& p_strName);
    void AddAffector(Affector* p_pAffector);

    private:

    bool ParseRandomRange(const std::string& p_strPropertyFamily, tinyxml2::XMLElement* pElem);
    bool ParseConstant(const std::string& p_strPropertyFamily, tinyxml2::XMLElement* pElem);

    void AddToPool(Particle* p);
    void AddToActive(Particle* p);
    Particle* GetFreeParticle();
    void RemoveFromActive(Particle* p);
    void RemoveFromActiveTail(Particle* p);
    
    void SpawnParticle();
    void ParticleKilled(Particle* p);

    void CalcBurstTime();

    std::string m_strName = "";

    // Render stuff
    wolf::VertexBuffer* m_pVB = nullptr;
    wolf::VertexDeclaration* m_pDecl = nullptr;
    wolf::Material* m_pMat = nullptr;

    // Emitter transform
    const Transform* m_pParentTransform;
    glm::vec3 m_vOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glm::mat4 m_mTransform = glm::mat4();

    bool m_bDirtyTransform = false;


    // Particle pools
    int m_numParticles = 0;
    Particle* pParticles = nullptr;

    Particle* m_pFreeListHead = 0;
    Particle* m_pActiveListHead = 0;
    Particle* m_pActiveListTail = 0;

    // Emitter Spawn properties

    Mode m_eMode = Mode::eContinuous;
    
    float m_fBirthRate = 0.0f;
    
    bool m_bRandomBirthRate = false;
    float m_fBirthRateMin = 0.0f;
    float m_fBirthRateMax = 0.0f;
    float m_fSpawnAcc = 0.0f;

    float m_fBurstTime = 0.0f;

    bool m_bRandomBurstSpawns = false;
    bool m_bRepeatBurst = false;
    unsigned int m_uiBurstSpawnMin = 0;
    unsigned int m_uiBurstSpawnMax = 0;

    float m_fBurstTimeMin = 0.0f;
    float m_fBurstTimeMax = 0.0f;

    float m_fTimeToBurst = 0.0f;

    float m_fEmitterLifetime = 0.0f;
    float m_fEmitterDuration = 0.0f;

    bool m_bRandomColor = true;
    Color m_vColorMin = {0, 0, 0, 0};
    Color m_vColorMax = {0, 0, 0, 0};
    Color m_vColor = {0, 0, 0, 0};

    bool m_bRandomVelocity = false;
    glm::vec3 m_vVelocityMin = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_vVelocityMax = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_vVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

    bool m_bRandomSize = false;
    float m_fSizeMin = 0.0f;
    float m_fSizeMax = 0.0f;
    float m_fSize = 0.0f;

    bool m_bRandomLifetime = false;
    float m_fParticleLifetimeMin = 0.0f;
    float m_fParticleLifetimeMax = 0.0f;
    float m_fParticleLifetime = 0.0f;

    bool m_bRandomFade = false;
    float m_fFadeMin = 0.0f;
    float m_fFadeMax = 0.0f;
    float m_fFade = 0.0f;

    // Affector List

    std::vector<Affector*> m_lAffectors;


};