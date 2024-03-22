#include "Effect.h"

Effect::Effect(const std::string& strFilepath)
:
m_bDirtyTransform(true),
m_strName(""),
m_vPos(glm::vec3(0.0f, 0.0f, 0.0f)),
m_vScale(glm::vec3(1.0f, 1.0f, 1.0f)),
m_vRot(glm::vec3(0.0f, 0.0f, 0.0f))
{
    CreateEffectFromXml(strFilepath);
}

Effect::~Effect()
{
    for (auto it = m_emitters.begin(); it != m_emitters.end(); ++it)
    {
        auto pEmitter = *it;
        delete pEmitter;
        pEmitter = 0;
    }
}

void Effect::Init()
{
    for (auto it = m_emitters.begin(); it != m_emitters.end(); ++it)
    {
        auto pEmitter = *it;
        pEmitter->Init();
    }
}

bool Effect::CreateEffectFromXml(const std::string& p_strFilePath)
{
     tinyxml2::XMLDocument emitterDoc;
	if (emitterDoc.LoadFile(p_strFilePath.c_str()) != tinyxml2::XML_SUCCESS)
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

    if (strNodeValue.compare("effect") != 0)
    {
        printf("Invalid XML format, emitter element missing.\n");
        return false;
    }

    tinyxml2::XMLElement* pElem = pNode->ToElement();

    // Name
    const char* szEffectName = pElem->Attribute("name");
    if (szEffectName == NULL)
    {
        return false;
    }
    m_strName = szEffectName;

    // Create Emitters
    tinyxml2::XMLNode* pChild = pNode->FirstChild();
    while (pChild)
    {
        const char* szFile;
        float fOffsetX, fOffsetY, fOffsetZ;
        std::string strEmitterName = pChild->Value();
        if (strEmitterName.compare("emitter") == 0)
        {
            tinyxml2::XMLElement* pElem = pChild->ToElement();
            if (pElem)
            {
                if (pElem->QueryAttribute("file", &szFile) != tinyxml2::XML_SUCCESS)
                {
                    printf("Emitter element missing offset attribute\n");
                    return false;
                }

                if (pElem->QueryFloatAttribute("offsetX", &fOffsetX) != tinyxml2::XML_SUCCESS)
                {
                    printf("Emitter element missing offset attribute\n");
                    return false;
                }

                if (pElem->QueryFloatAttribute("offsetY", &fOffsetY) != tinyxml2::XML_SUCCESS)
                {
                    printf("Emitter element missing offset attribute\n");
                    return false;
                }

                if (pElem->QueryFloatAttribute("offsetZ", &fOffsetZ) != tinyxml2::XML_SUCCESS)
                {
                    printf("Emitter element missing offset attribute\n");
                    return false;
                }
            }
        }
        Emitter* pEmmiter = new Emitter(&m_transform, glm::vec3(fOffsetX, fOffsetY, fOffsetZ), szFile);
        m_emitters.push_back(pEmmiter);

        pChild = pChild->NextSibling();
    }

    return true;
}

void Effect::SetPosition(const glm::vec3& p_vPos)
{
    m_transform.vPos = p_vPos;
}

void Effect::Translate(const glm::vec3& p_vPos)
{
    m_transform.vPos += p_vPos;
}


void Effect::SetRotation(const glm::vec3& p_vRot)
{
    m_transform.vRot = p_vRot;
}

void Effect::Rotate(const glm::vec3& p_vRot)
{
    m_transform.vRot += p_vRot;
}


void Effect::SetScale(const glm::vec3& p_vScale)
{
    m_transform.vScale = p_vScale;
}

void Effect::Scale(const glm::vec3& p_vScale)
{
    m_transform.vScale *= p_vScale;
}

void Effect::Update(float dt)
{
    for (auto it = m_emitters.begin(); it != m_emitters.end(); ++it)
    {
        auto emitter = *it;
        emitter->Update(dt);
    }
}

void Effect::Render(const glm::mat4& p_mView, const glm::mat4& p_mProj)
{
    for (auto it = m_emitters.begin(); it != m_emitters.end(); ++it)
    {
        auto emitter = *it;
        emitter->Render(p_mView, p_mProj);
    }
}