#pragma once
#include "../wolf/wolf.h"

struct Transform
{
    glm::vec3 vPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 vRot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 vScale = glm::vec3(1.0f, 1.0f, 1.0f);
};

/*class Transform
	{
	public:
		//------------------------------------------------------------------------------
		// Public methods.
		//------------------------------------------------------------------------------
		Transform();
		virtual ~Transform();

		// Scale
		void Scale(const glm::vec3& p_vScale);
		void SetScale(const glm::vec3& p_vScale);
		const glm::vec3& GetScale() const;

		// Rotate
		void Rotate(const glm::vec3& p_vRotation);
		void SetRotation(const glm::vec3& p_vRotation);
		const glm::quat& GetRotation() const;

		// Translate
		void Translate(const glm::vec3& p_vTranslation);
		void SetTranslation(const glm::vec3& p_vTranslation);
		const glm::vec3& GetTranslation() const;

		// Transformation and DerivedTransformation
		const glm::mat4& GetTransformation() const;

	private:

		// Whether the transformation matrix needs to be recalculated because the transform has changed
		bool m_bDirty;

		// The scale, rotation and translation
		glm::vec3 m_vScale;
		glm::vec3 m_vTranslation;
		glm::vec3 m_vRotation;


		// The combined transform
		glm::mat4 m_mTransform;
	};
    */