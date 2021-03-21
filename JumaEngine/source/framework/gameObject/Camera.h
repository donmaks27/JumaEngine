// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "SceneObject.h"

namespace JumaEngine
{
	class Camera : public SceneObject
	{
	public:
		Camera() = default;
		virtual ~Camera() override = default;

		const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }

		bool isPerspective() const { return m_Perspective; }
		void setPerspective(bool enable);
		
		float getNearClipDistance() const { return m_NearClipDistance; }
		float getFarClipDistance() const { return m_FarClipDistance; }
		void setClipDistances(float nearDistance, float farDistance);
		void setNearClipDistance(const float distance) { setClipDistances(distance, getFarClipDistance()); }
		void setFarClipDistance(const float distance) { setClipDistances(getNearClipDistance(), distance); }
		
		float getFOV() const { return m_FOV; }
		void setFOV(float angle);

		float getOrthoWidth() const { return m_OrthoWidth; }
		void setOrthoWidth(float width);
	
	protected:
		
		virtual void onRegister() override;
		
		virtual void onTransformChanged() override;
	
	private:

		glm::mat4 m_ProjectionMatrix = glm::identity<glm::mat4>();
		glm::mat4 m_ViewMatrix = glm::identity<glm::mat4>();

		bool m_Perspective = true;
		float m_NearClipDistance = 0.01f;
		float m_FarClipDistance = 100000.0f;
		// For prespective only
		float m_FOV = 90.0f;
		// For orthographic only
		float m_OrthoWidth = 512.0f;
		

		void updateProjectionMatrix();
		void updateViewMatrix();
	};
}
