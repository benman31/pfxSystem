#pragma once

#include "../wolf/wolf.h"
#include <list>
#include "../samplefw/OrbitCamera.h"
#include "Emitter.h"
#include "Transform.h"

class Effect
{
    public:
    Effect(const std::string& strFilepath);
    ~Effect();
    
    void Init();
    void Update(float dt);
    void Render(const glm::mat4& p_mView, const glm::mat4& p_mProj);

    void SetPosition(const glm::vec3& p_vPos);
    void Translate(const glm::vec3& p_vPos);
    const glm::vec3& GetPos() { return m_vPos; }

    void SetRotation(const glm::vec3& p_vRot);
    void Rotate(const glm::vec3& p_vRot);
    const glm::vec3 GetRotation() { return m_vRot; }

    void SetScale(const glm::vec3& p_vScale);
    void Scale(const glm::vec3& p_vScale);
    const glm::vec3& GetScale() { return m_vScale; }
    
    // TODO implement States
    void Play();
    void Stop();
    void Pause();

    private:

    bool CreateEffectFromXml(const std::string& p_strFilePath);

    std::string m_strName;

    std::list<Emitter*> m_emitters;

    glm::vec3 m_vPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_vRot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_vScale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    bool m_bDirtyTransform;

    Transform m_transform;
};