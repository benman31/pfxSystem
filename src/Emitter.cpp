#include "Emitter.h"

static const Vertex gs_particleVerts[] = 
{
	{ -0.5f, -0.5f, 0.5f, 0, 0, 0, 0 },
	{ -0.5f,  0.5f, 0.5f, 0, 0, 0, 0 },
	{  0.5f,  0.5f, 0.5f, 0, 0, 0, 0 },
	{  0.5f,  0.5f, 0.5f, 0, 0, 0, 0 },
	{  0.5f, -0.5f, 0.5f, 0, 0, 0, 0 },
	{ -0.5f, -0.5f, 0.5f, 0, 0, 0, 0 },
};

Emitter::Emitter(const Transform* p_ParentTransform, const glm::vec3& vOffset, const std::string& strFilePath)
:
m_pParentTransform(p_ParentTransform),
m_vOffset(vOffset)
{
    CreateEmitterFromXml(strFilePath);
}

Emitter::~Emitter()
{
    delete[] pParticles;
    
    for(auto it = m_lAffectors.begin(); it != m_lAffectors.end(); ++it)
    {
        auto affector = *it;
        delete affector;
    }
    wolf::MaterialManager::DestroyMaterial(m_pMat);
    wolf::BufferManager::DestroyBuffer(m_pVB);
    delete m_pDecl;
}

void Emitter::Init()
{
    pParticles = new Particle[m_numParticles];
    for (unsigned int i = 0; i < m_numParticles; ++i)
    {
        AddToPool(&pParticles[i]);
    }

    if(!m_pMat)
    {
        m_pVB = wolf::BufferManager::CreateVertexBuffer(sizeof(Vertex) * 6 * m_numParticles);

        m_pDecl = new wolf::VertexDeclaration();
        m_pDecl->Begin();
        m_pDecl->AppendAttribute(wolf::AT_Position, 3, wolf::CT_Float);
        m_pDecl->AppendAttribute(wolf::AT_Color, 4, wolf::CT_UByteNorm, sizeof(GLfloat) * 3);
        m_pDecl->SetVertexBuffer(m_pVB);
        
        m_pDecl->End();

        m_pMat = wolf::MaterialManager::CreateMaterial(m_strName);
		m_pMat->SetProgram("data/particle.vsh", "data/particle.fsh");
        m_pMat->SetUniform("u_color", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        
        //Turn off blending for confetti
        if (m_strName.compare("confetti") == 0)
        {
            m_pMat->SetBlend(false);
        }
        else
        {
            m_pMat->SetBlend(true);
        }
        
        // Special cases for fire and smoke blend modes
        if (m_strName.compare("fire") == 0)
        {
            m_pMat->SetBlendMode(wolf::BM_One, wolf::BM_One);
        }
        else
        {
            m_pMat->SetBlendMode(wolf::BM_SrcAlpha, wolf::BM_OneMinusSrcAlpha);
        }
		
    }
}

bool Emitter::CreateEmitterFromXml(const std::string& p_strPath)
{

    tinyxml2::XMLDocument emitterDoc;
	if (emitterDoc.LoadFile(p_strPath.c_str()) != tinyxml2::XML_SUCCESS)
	{
		printf("failed to load file\n");
        return false;
	}

	// Look for the root "emitter" node and return false if it's missing
	tinyxml2::XMLNode* pNode = emitterDoc.FirstChild();
	if (pNode == NULL)
	{
		printf("Failed to read\n");
        return false;
	}

    std::string strNodeValue = pNode->Value();

    if (strNodeValue.compare("emitter") != 0)
    {
        printf("Invalid XML format, emitter element missing.\n");
        return false;
    }

    tinyxml2::XMLElement* pElem = pNode->ToElement();

    // Name
    const char* szEmitterName = pElem->Attribute("name");
    if (szEmitterName == NULL)
    {
        printf("Emitter name attribute missing\n");
        return false;
    }
    m_strName = szEmitterName;

    // Number of Particles
    int iNumParticles;
    if (pElem->QueryIntAttribute("num_particles", &iNumParticles) != tinyxml2::XML_SUCCESS)
    {
        printf("Emitter number of particles attribute missing\n");
        return false;
    }
    m_numParticles = iNumParticles;

    // Duration
    float fDuration;
    if (pElem->QueryFloatAttribute("duration", &fDuration) != tinyxml2::XML_SUCCESS)
    {
        printf("Emitter duration attribute missing\n");
        return false;
    }
    m_fEmitterDuration = fDuration;
    
    // Mode
    const char* szEmitterMode = pElem->Attribute("type");
    if (szEmitterMode == NULL)
    {
        printf("Emitter type attribute missing\n");
        return false;
    }
    else if(strcmp(szEmitterMode, "continuous") == 0)
    {
        m_eMode = Mode::eContinuous;
    }
    else if(strcmp(szEmitterMode, "burst") == 0)
    {
        m_eMode = Mode::eBurst;

        bool bRepeat;
        if (pElem->QueryBoolAttribute("repeat", &bRepeat) == tinyxml2::XML_SUCCESS)
        {
            m_bRepeatBurst = bRepeat;
        }
        else
        {
            printf("Burst repeat attribute missing\n");
            return false;
        }
        const char* szBurstMode;
        if (pElem->QueryAttribute("mode", &szBurstMode) == tinyxml2::XML_SUCCESS)
        {
            if (strcmp(szBurstMode, "random") == 0)
            {
                m_bRandomBurstSpawns = true;
                float fMin, fMax;
                if (pElem->QueryFloatAttribute("min", &fMin) == tinyxml2::XML_SUCCESS)
                {
                    m_fBurstTimeMin = fMin;
                }
                else
                {
                    printf("Min Burst time missing\n");
                    return false;
                }
                
                if (pElem->QueryFloatAttribute("max", &fMax) == tinyxml2::XML_SUCCESS)
                {
                    m_fBurstTimeMax = fMax;
                }
                else
                {
                    printf("Max Burst time missing\n");
                    return false;
                }
            }
            else if (strcmp(szBurstMode, "constant") == 0)
            {
                m_bRandomBurstSpawns = false;
                float fBurstTime;
                if (pElem->QueryFloatAttribute("burstTime", &fBurstTime) == tinyxml2::XML_SUCCESS)
                {
                    m_fBurstTime = fBurstTime;
                }
                else
                {
                    printf("Burst time constant missing\n");
                    return false;
                }
            }
            else
            {
                printf("Burst mode attribute not recognized\n");
                return false;
            }
        }
        else
        {
            printf("Burst repeat attribute missing\n");
            return false;
        }
    }
    else
    {
        printf("Emitter type attribute value invalid\n");
        return false;
    }

    // Spawn Properties
    tinyxml2::XMLNode* pChild = pNode->FirstChild();
    strNodeValue = pChild->Value();
    if (strNodeValue.compare("spawn_property") != 0)
    {
        return false;
    }
    
    while (pChild)
    {
        strNodeValue = pChild->Value();
        if (strNodeValue.compare("spawn_property") != 0)
        {
            // Another Check to  tell us when we Finsihed parsing spawn_property elements 
            break;
        }

        pElem = pChild->ToElement();

        // Name
        const char* szSpawnPropertyName = pElem->Attribute("name");
        if (szSpawnPropertyName == NULL)
        {
            printf("Missing property name.\n");
            return false;
        }
        
        if (strcmp(szSpawnPropertyName, "birthrate") == 0 ||
            strcmp(szSpawnPropertyName, "burstSpawns") == 0 ||
            strcmp(szSpawnPropertyName, "velocity") == 0 ||
            strcmp(szSpawnPropertyName, "color") == 0 ||
            strcmp(szSpawnPropertyName, "size") == 0 ||
            strcmp(szSpawnPropertyName, "lifetime") == 0 ||
            strcmp(szSpawnPropertyName, "fade") == 0
            )
        {
            const char* szPropetyType = pElem->Attribute("type");
            if (szPropetyType == NULL)
            {
                printf("Property type missing.\n");
                return false;
            }
            else if(strcmp(szPropetyType, "random") == 0)
            {
                bool parseResult = ParseRandomRange(szSpawnPropertyName, pElem);

                if (!parseResult)
                {
                    return false;
                }
            }
            else if (strcmp(szPropetyType, "constant") == 0)
            {
                bool parseResult = ParseConstant(szSpawnPropertyName, pElem);

                if (!parseResult)
                {
                    return false;
                }
            }
            else
            {
                printf("Invalid spawn property type.\n");
                return false;
            }
        }
        pChild = pChild->NextSibling();
    }

    // Affectors
    strNodeValue = pChild->Value();
    if (strNodeValue.compare("affector") == 0)
    {
        while(pChild)
        {
            pElem = pChild->ToElement();
            if (!pElem)
            {
                //Skip comment nodes
                pChild = pChild->NextSibling();
                continue;
            }
            // Affector type
            const char* szAffectorName = pElem->Attribute("type");
            if (szAffectorName == NULL)
            {
                printf("Missing Affector name.\n");
                return false;
            }
            else if (strcmp(szAffectorName, "addVelocity") || strcmp(szAffectorName, "scale") || strcmp(szAffectorName, "fade") || strcmp(szAffectorName, "gravity"))
            {
                bool parseResult = CreateAffectorFromXML(pChild);
                if (!parseResult)
                {
                    printf("Affector Parsing error\n");
                    return false;
                }
            }
            else
            {
                printf("Invalid Affector name\n");
                return false;
            }
            pChild = pChild->NextSibling();
        }
    }
    return true;
}

bool Emitter::CreateAffectorFromXML(tinyxml2::XMLNode* p_pNode)
{
    assert(p_pNode);
    Affector* pAff = 0;

    tinyxml2::XMLElement* pElem = p_pNode->ToElement();

    const char* szAffName;
    pElem->QueryAttribute("type", &szAffName);

    if(strcmp(szAffName, "gravity") == 0)
    {
        pAff = new GravityAffector();
    }
    else if (strcmp(szAffName, "fade") == 0)
    {
        tinyxml2::XMLNode* pChild = p_pNode->FirstChild();
        if (!pChild)
        {
            printf("Fade affector missing properties\n");
            return false;
        }
        while (pChild)
        {
            pElem = pChild->ToElement();
            
            if (!pElem)
            {
                //Skip comment nodes
                pChild = pChild->NextSibling();
                continue;
            }

            const char* szElementName = pElem->Value();
            if (strcmp(szElementName, "property"))
            {
                printf("Invalid Affector child node\n");
                return false;
            }
            const char* szPropName;
            if (pElem->QueryAttribute("type", &szPropName) == tinyxml2::XML_SUCCESS)
            {
                if (strcmp(szPropName, "fadeIn") == 0)
                {
                    pAff = new FadeAffector(false);
                }
                else if (strcmp(szPropName, "fadeOut") == 0)
                {
                    pAff = new FadeAffector(true);
                }
                else
                {
                    printf("Fade property attribute name not recognized\n");
                }
            }
            else 
            {
                printf("Property type missing\n");
                return false;   
            }
            pChild = pChild->NextSibling();          
        }
    }
    else if(strcmp(szAffName, "scale") == 0)
    {
        tinyxml2::XMLNode* pChild = p_pNode->FirstChild();
        if (!pChild)
        {
            printf("Scale affector missing properties\n");
            return false;
        }

        pElem = pChild->ToElement();
        
        float fStart, fEnd;
            
        if (pElem->QueryFloatAttribute("start", &fStart) != tinyxml2::XML_SUCCESS)
        {
            printf("Missing Scale 'start' property\n");
            return false;
        }
        
        if (pElem->QueryFloatAttribute("end", &fEnd) != tinyxml2::XML_SUCCESS)
        {
            printf("Missing Scale 'start' property\n");
            return false;
        }
        pAff = new ScaleAffector(fStart, fEnd);
    }
    else if (strcmp(szAffName, "addVelocity") == 0)
    {
        tinyxml2::XMLNode* pChild = p_pNode->FirstChild();
        if (!pChild)
        {
            printf("Add Velocity affector missing properties\n");
            return false;
        }
        while (pChild)
        {
            pElem = pChild->ToElement();

            if (!pElem)
            {
                //Skip comment nodes
                pChild = pChild->NextSibling();
                continue;
            }
            
            const char* szMode;

            if (pElem->QueryAttribute("mode", &szMode) != tinyxml2::XML_SUCCESS)
            {
                printf("Velocity affector missing 'mode' property\n");
                return false;
            }
            if (strcmp(szMode, "random") == 0)
            {
                float fAttr;
                std::vector<std::string>lAttrNames = {"xMin", "yMin", "zMin", "xMax", "yMax", "zMax"};
                std::vector<float> lAttrValues;

                for (auto it = lAttrNames.begin(); it!= lAttrNames.end(); ++it)
                {
                    auto attribute = *it;
                    if (pElem->QueryFloatAttribute(attribute.c_str(), &fAttr) != tinyxml2::XML_SUCCESS)
                    {
                        printf("Random Velocity missing a range attribute\n");
                        return false;
                    }
                    else
                    {
                        lAttrValues.push_back(fAttr);
                    }
                }
                glm::vec3 vStart = glm::vec3(lAttrValues.at(0), lAttrValues.at(1), lAttrValues.at(2));
                glm::vec3 vEnd = glm::vec3(lAttrValues.at(3), lAttrValues.at(4), lAttrValues.at(5));
                pAff = new AddVelocityAffector(vStart, vEnd);
            }
            else if (strcmp(szMode, "constant") == 0)
            {
                float fAttr;
                std::vector<std::string>lAttrNames = {"x", "y", "z"};
                std::vector<float> lAttrValues;

                for (auto it = lAttrNames.begin(); it!= lAttrNames.end(); ++it)
                {
                    auto attribute = *it;
                    if (pElem->QueryFloatAttribute(attribute.c_str(), &fAttr) != tinyxml2::XML_SUCCESS)
                    {
                        printf("Constant Velocity missing a value\n");
                        return false;
                    }
                    else
                    {
                        lAttrValues.push_back(fAttr);
                    }
                }
                glm::vec3 vVel = glm::vec3(lAttrValues.at(0), lAttrValues.at(1), lAttrValues.at(2));
                pAff = new AddVelocityAffector(vVel);
            }
            else 
            {
                printf("Mode not recognized\n");
                return false;
            }
            pChild = pChild->NextSibling();
        }
    }
    else
    {
        printf("Affector name not recognized\n");
    }

    if (pAff)
    {
        m_lAffectors.push_back(pAff);
        return true;
    }  
    return false;
}

bool Emitter::ParseRandomRange(const std::string& p_strPropertyFamily, tinyxml2::XMLElement* pElem)
{
    float fAttr;
    int iAttr;

    std::vector<std::string> lAttrNames;

    bool bParsingError = false;

    if (p_strPropertyFamily.compare("birthrate") == 0 || p_strPropertyFamily.compare("burstSpawns") == 0 || p_strPropertyFamily.compare("size") == 0 || p_strPropertyFamily.compare("fade") == 0 || p_strPropertyFamily.compare("lifetime") == 0)
    {
        lAttrNames = {"min", "max"};
        std::vector<float> lAttrValues;

        for (auto it = lAttrNames.begin(); it!= lAttrNames.end(); ++it)
        {
            auto attribute = *it;
            if (pElem->QueryFloatAttribute(attribute.c_str(), &fAttr) != tinyxml2::XML_SUCCESS)
            {
                return false;
            }
            else
            {
                lAttrValues.push_back(fAttr);
            }
        }
        if (p_strPropertyFamily.compare("birthrate") == 0)
        {
            m_bRandomBirthRate = true;
            m_fBirthRateMin = lAttrValues.at(0);
            m_fBirthRateMax = lAttrValues.at(1);
        }
        else if (p_strPropertyFamily.compare("burstSpawns") == 0)
        {
            m_bRandomBurstSpawns = true;
            m_uiBurstSpawnMin = lAttrValues.at(0);
            m_uiBurstSpawnMax = lAttrValues.at(1);
        }
        else if (p_strPropertyFamily.compare("size") == 0)
        {
            m_bRandomSize = true;
            m_fSizeMin = lAttrValues.at(0);
            m_fSizeMax = lAttrValues.at(1);
        }
        else if (p_strPropertyFamily.compare("fade") == 0)
        {
            m_bRandomFade = true;
            m_fFadeMin = lAttrValues.at(0);
            m_fFadeMax = lAttrValues.at(1);
        }
        else if (p_strPropertyFamily.compare("lifetime") == 0)
        {
            m_bRandomLifetime = false;
            m_fParticleLifetime = lAttrValues.at(0);
            m_fParticleLifetime = lAttrValues.at(1);
        }
        
    }
    else if (p_strPropertyFamily.compare("velocity") == 0)
    {
        lAttrNames = {"xMin", "yMin", "zMin", "xMax", "yMax", "zMax"};
        std::vector<float> lAttrValues;

        for (auto it = lAttrNames.begin(); it!= lAttrNames.end(); ++it)
        {
            auto attribute = *it;
            if (pElem->QueryFloatAttribute(attribute.c_str(), &fAttr) != tinyxml2::XML_SUCCESS)
            {
                return false;
            }
            else
            {
                lAttrValues.push_back(fAttr);
            }
        }
        m_bRandomVelocity = true;
        m_vVelocityMin = glm::vec3(lAttrValues.at(0), lAttrValues.at(1), lAttrValues.at(2));
        m_vVelocityMax = glm::vec3(lAttrValues.at(3), lAttrValues.at(4), lAttrValues.at(5));
    }
    else if (p_strPropertyFamily.compare("color") == 0)
    {
        lAttrNames = {"rMin", "gMin", "bMin", "aMin", "rMax", "gMax", "bMax", "aMax"};
        std::vector<int> lAttrValues;

        for (auto it = lAttrNames.begin(); it!= lAttrNames.end(); ++it)
        {
            auto attribute = *it;
            if (pElem->QueryIntAttribute(attribute.c_str(), &iAttr) != tinyxml2::XML_SUCCESS)
            {
                return false;
            }
            else
            {
                lAttrValues.push_back(iAttr);
            }
        }
        m_bRandomColor = true;
        m_vColorMin = {(GLubyte)lAttrValues.at(0), (GLubyte)lAttrValues.at(1), (GLubyte)lAttrValues.at(2), (GLubyte)lAttrValues.at(3)};
        m_vColorMax = {(GLubyte)lAttrValues.at(4), (GLubyte)lAttrValues.at(5), (GLubyte)lAttrValues.at(6), (GLubyte)lAttrValues.at(7)};
    }
    return true;
}

bool Emitter::ParseConstant(const std::string& p_strPropertyFamily, tinyxml2::XMLElement* pElem)
{
    float fAttr;
    int iAttr;

    std::vector<std::string> lAttrNames;

    bool bParsingError = false;

    if (p_strPropertyFamily.compare("birthrate") == 0 || p_strPropertyFamily.compare("burstSpawns") == 0 || p_strPropertyFamily.compare("size") == 0|| p_strPropertyFamily.compare("fade") == 0 || p_strPropertyFamily.compare("lifetime") == 0)
    {
       
        if (pElem->QueryFloatAttribute("value", &fAttr) != tinyxml2::XML_SUCCESS)
        {
            return false;
        }
        else if (p_strPropertyFamily.compare("birthrate") == 0)
        {
            m_bRandomBirthRate = false;
            m_fBirthRate = fAttr;
        }
        else if (p_strPropertyFamily.compare("burstSpawns") == 0)
        {
            m_bRandomBurstSpawns = false;
            m_fBurstTime = fAttr;
        }
        else if (p_strPropertyFamily.compare("size") == 0)
        {
            m_bRandomSize = false;
            m_fSize = fAttr;
        }
        else if (p_strPropertyFamily.compare("fade") == 0)
        {
            m_bRandomFade = false;
            m_fFade = fAttr;
        }
        else if (p_strPropertyFamily.compare("lifetime") == 0)
        {
            m_bRandomLifetime = false;
            m_fParticleLifetime = fAttr;
        }
        
    }
    else if (p_strPropertyFamily.compare("velocity") == 0)
    {
        lAttrNames = {"x", "y", "z"};
        std::vector<float> lAttrValues;

        for (auto it = lAttrNames.begin(); it!= lAttrNames.end(); ++it)
        {
            auto attribute = *it;
            if (pElem->QueryFloatAttribute(attribute.c_str(), &fAttr) != tinyxml2::XML_SUCCESS)
            {
                return false;
            }
            else
            {
                lAttrValues.push_back(fAttr);
            }
        }
        m_bRandomVelocity = false;
        m_vVelocity = glm::vec3(lAttrValues.at(0), lAttrValues.at(1), lAttrValues.at(2));
    }
    else if (p_strPropertyFamily.compare("color") == 0)
    {
        lAttrNames = {"r", "g", "b", "a"};
        std::vector<int> lAttrValues;

        for (auto it = lAttrNames.begin(); it!= lAttrNames.end(); ++it)
        {
            auto attribute = *it;
            if (pElem->QueryIntAttribute(attribute.c_str(), &iAttr) != tinyxml2::XML_SUCCESS)
            {
                return false;
            }
            else
            {
                lAttrValues.push_back(iAttr);
            }
        }
        m_bRandomColor = false;
        m_vColor = {(GLubyte)lAttrValues.at(0), (GLubyte)lAttrValues.at(1), (GLubyte)lAttrValues.at(2), (GLubyte)lAttrValues.at(3)};
    }
    return true;
}

void Emitter::AddToPool(Particle* p)
{
    p->prev = 0;
    p->next = m_pFreeListHead;
    m_pFreeListHead = p;
}

void Emitter::AddToActive(Particle* p)
{
    // First update head
    p->prev = 0;
    p->next = m_pActiveListHead;
    m_pActiveListHead = p;
    assert(m_pActiveListHead);
    if (!m_pActiveListHead->next)
    {
        m_pActiveListTail = m_pActiveListHead;
    }
    else
    {
        m_pActiveListHead->next->prev = m_pActiveListHead;
    }
}

void Emitter::RemoveFromActive(Particle* p)
{
    if (p == m_pActiveListTail && p == m_pActiveListHead)
    {
        // This is the only active particle, reset pointers to null
        m_pActiveListHead = 0;
        m_pActiveListTail = 0;
        p->next = 0;
        p->prev = 0;
    }
    else if (p == m_pActiveListTail)
    {
        // Remove from tail
        p->prev->next = 0;
        m_pActiveListTail = p->prev;
        p->next = 0;
        p->prev = 0;
    }
    else if (p == m_pActiveListHead)
    {
        // Remove from head
        m_pActiveListHead = p->next;
        m_pActiveListHead->prev = 0;
        p->next = 0;
        p->prev = 0;
    }
    else
    {
        // Remove from middle
        p->next->prev = p->prev;
        p->prev->next = p->next;
        p->next = 0;
        p->prev = 0;
    }
}

Particle* Emitter::GetFreeParticle()
{
    Particle* pRet = m_pFreeListHead;
    if (pRet)
    {
        m_pFreeListHead = m_pFreeListHead->next;
        if ( m_pFreeListHead )
        {
            m_pFreeListHead->prev = 0;
        }

        pRet->prev = 0;
        pRet->next = 0;
        return pRet;
    }
    return 0;
}

void Emitter::SpawnParticle()
{
    Particle* p = GetFreeParticle();
    if (!p)
    {
        p = m_pActiveListTail;
        RemoveFromActive(p);
    }

    p->age = 0.0f;
    p->scale = 1.0f;
    p->affectorFade = 1.0f;
    
    // Color
    if (m_bRandomColor)
    {
        p->color.r = (GLubyte) wolf::randInt(m_vColorMin.r, m_vColorMax.r);
        p->color.g = (GLubyte) wolf::randInt(m_vColorMin.g, m_vColorMax.g);
        p->color.b = (GLubyte) wolf::randInt(m_vColorMin.b, m_vColorMax.b);
        p->color.a = (GLubyte) wolf::randInt(m_vColorMin.a, m_vColorMax.a);
    }
    else 
    {
        p->color = m_vColor;
    }

    // Position
    //TODO add Box emitter support
    p->position = m_pParentTransform->vPos + m_vOffset;

    // Velocity
    if (m_bRandomVelocity)
    {
        p->velocity = wolf::randVec3(m_vVelocityMin, m_vVelocityMax);
    }
    else
    {
        p->velocity = m_vVelocity;
    }

    // Size
    if (m_bRandomSize)
    {
        p->size = wolf::randFloat(m_fSizeMin, m_fSizeMax);
    }
    else
    {
        p->size = m_fSize;
    }

    // Lifetime
    if (m_bRandomLifetime)
    {
        p->lifeTime = wolf::randFloat(m_fParticleLifetimeMin, m_fParticleLifetimeMax);
    }
    else
    {
        p->lifeTime = m_fParticleLifetime;
    }

    // Fade
    if (m_bRandomFade)
    {
        p->fade = wolf::randFloat(m_fFadeMin, m_fFadeMax);
    }
    else
    {
        p->fade = m_fFade;
    }
    
    AddToActive(p);
}

void Emitter::ParticleKilled(Particle* p)
{
    RemoveFromActive(p);
    AddToPool(p);
}

void Emitter::CalcBurstTime()
{
    if (m_bRandomBurstSpawns)
    {
        m_fTimeToBurst = wolf::randFloat(m_fBurstTimeMin, m_fBurstTimeMax);
    }
    else
    {
        m_fTimeToBurst = m_fBurstTime;
    }
}

void Emitter::SetName(const std::string& p_strName)
{
    m_strName = p_strName;
}

void Emitter::AddAffector(Affector* p_pAffector)
{
    m_lAffectors.push_back(p_pAffector);
}

void Emitter::Update(float dt)
{
    m_fEmitterLifetime += dt;
    if (m_fEmitterDuration < 0 || m_fEmitterLifetime < m_fEmitterDuration)
    {
        if (m_eMode == Mode::eContinuous)
        {
            float birthRate = 0.0f;
            if (m_bRandomBirthRate)
            {
                birthRate = wolf::randFloat(m_fBirthRateMin, m_fBirthRateMax);
            }
            else
            {
                birthRate = m_fBirthRate;
            }
            m_fSpawnAcc += birthRate * dt;

            int numSpawns = m_fSpawnAcc > m_numParticles ? m_numParticles : (int) m_fSpawnAcc;  
            
            m_fSpawnAcc -= numSpawns;

            for (unsigned int i = 0; i < numSpawns; ++i)
            {
                SpawnParticle();
            }
        }
        else if (m_eMode == Mode::eBurst)
        {
            m_fTimeToBurst -= dt;
            if (m_fTimeToBurst <= 0)
            {
                int numSpawns = 0;
                if (m_bRandomBurstSpawns)
                {
                    numSpawns = wolf::randInt(m_uiBurstSpawnMin, m_uiBurstSpawnMax);
                }
                if (m_bRepeatBurst)
                {
                    CalcBurstTime();
                }

                for (unsigned int i = 0; i < numSpawns; ++i)
                {
                    SpawnParticle();
                }
            }
        }

    }

    // Update Particles
    Particle* p = m_pActiveListHead;
    while(p)
    {
        p->age += dt;
        if (p->age >= p->lifeTime)
        {
            Particle* pTemp = p->next;
            ParticleKilled(p);
            p = pTemp;

        }
        else
        {
            for (auto it = m_lAffectors.begin(); it != m_lAffectors.end(); ++it)
            {
                auto pAffector = *it;
                pAffector->Apply(dt, p);
            }
            p->position = p->position + (p->velocity * dt);
            p = p->next;
        }
    }
}

void Emitter::Render(const glm::mat4& p_mView, const glm::mat4& p_mProj)
{
    unsigned int activeParticleCount = 0;

    //Look up lock-free buffer implementation if time permits
    Vertex* particleBuffer = new Vertex[m_numParticles * 6];
    unsigned int vertCount = 0;
    Particle* p = m_pActiveListHead;
    //assert(p);

    glm::mat3 viewRot = glm::mat3(p_mView);
    glm::mat3 viewRotTranspose = glm::transpose(viewRot);
    glm::mat4 newViewTranspose = glm::mat4(viewRotTranspose);

    while(p)
    {
        glm::mat4 mWorld = glm::mat4(1.0f);
        mWorld = glm::translate(mWorld, p->position);
        mWorld = mWorld * newViewTranspose; 
        mWorld = mWorld * glm::rotate(glm::mat4(1.0f), m_pParentTransform->vRot.x, glm::vec3(1.0f, 0.0f, 0.0f));
        mWorld = mWorld * glm::rotate(glm::mat4(1.0f), m_pParentTransform->vRot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	    mWorld = mWorld * glm::rotate(glm::mat4(1.0f), m_pParentTransform->vRot.z, glm::vec3(0.0f, 0.0f, 1.0f));
        mWorld = glm::scale(mWorld, p->size * p->scale * m_pParentTransform->vScale);

        for (unsigned int i = 0; i < 6; ++i)
        {
            glm::vec4 vertPos = glm::vec4(gs_particleVerts[i].x, gs_particleVerts[i].y, gs_particleVerts[i].z, 1.0f);
            vertPos = mWorld * vertPos;
            particleBuffer[vertCount].x = vertPos.x;
            particleBuffer[vertCount].y = vertPos.y;
            particleBuffer[vertCount].z = vertPos.z;
            particleBuffer[vertCount].r = p->color.r;
            particleBuffer[vertCount].g = p->color.g;
            particleBuffer[vertCount].b = p->color.b;
            particleBuffer[vertCount].a = (GLubyte)(p->color.a * p->fade * p->affectorFade); 

            vertCount++;
            
        }
        activeParticleCount++;        

        p = p->next;
    }

    if (activeParticleCount)
    {

        m_pVB->Write(particleBuffer, sizeof(Vertex) * 6 * activeParticleCount);
        
        m_pDecl->Bind();

        m_pMat->SetUniform("mWorld", glm::mat4(1.0));
        m_pMat->SetUniform("mView", p_mView);
        m_pMat->SetUniform("mProj", p_mProj);

        m_pMat->Apply();
        
        glDrawArrays(GL_TRIANGLES, 0, 6 * activeParticleCount);
    }
    delete[] particleBuffer;
}
