// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "CameraComponent.h"

#include "utils/log.h"
#include "utils/math_functions.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
	void CameraComponent::onRegister()
	{
		updateProjectionMatrix();
		updateViewMatrix();
	}
	
	void CameraComponent::updateProjectionMatrix()
	{
		const float windowAspectRatio = SystemFunctions::getWindowAspectRatio(this);
		if (isPerspective())
		{
			m_ProjectionMatrix = glm::perspective(
				glm::radians(getFOV()),
				windowAspectRatio,
				getNearClipDistance(),
				getFarClipDistance()
			);
		}
		else
		{
			const float orthoHeight = windowAspectRatio > 0 ? m_OrthoWidth / windowAspectRatio : Consts::SmallNumber;
			const float orthoWidthHalf = m_OrthoWidth / 2;
			const float orthoHeightHalf = glm::max(orthoHeight, Consts::SmallNumber) / 2;
			m_ProjectionMatrix = glm::ortho(
				-orthoWidthHalf, orthoWidthHalf, 
				-orthoHeightHalf, orthoHeightHalf,
				getNearClipDistance(), getFarClipDistance()
			);
		}
	}
	void CameraComponent::updateViewMatrix()
	{
		const glm::vec3 location = getWorldLocatoin();
		m_ViewMatrix = glm::lookAt(location, location + getWorldRotation().toDirection(), Consts::VectorUp);
	}

	void CameraComponent::setPerspective(const bool enable)
	{
		if (m_Perspective != enable)
		{
			m_Perspective = enable;
			updateProjectionMatrix();
		}
	}

	void CameraComponent::setClipDistances(const float nearDistance, const float farDistance)
	{
		const float newNear = glm::max(Consts::SmallNumber, nearDistance);
		const float newFar = glm::max(newNear + Consts::SmallNumber, farDistance);
		if (!MathFunctions::isNearlyEquals(newNear, m_NearClipDistance) || !MathFunctions::isNearlyEquals(newFar, m_FarClipDistance))
		{
			m_NearClipDistance = newNear;
			m_FarClipDistance = newFar;
			updateProjectionMatrix();
		}
	}

	void CameraComponent::setFOV(const float angle)
	{
		const float newAngle = glm::clamp(angle, 0.0f, 360.0f);
		if (!MathFunctions::isNearlyEquals(m_FOV, newAngle))
		{
			m_FOV = newAngle;
			if (isPerspective())
			{
				updateProjectionMatrix();
			}
		}
	}

	void CameraComponent::setOrthoWidth(const float width)
	{
		const float newWidth = glm::max(Consts::SmallNumber, width);
		if (!MathFunctions::isNearlyEquals(m_OrthoWidth, newWidth))
		{
			m_OrthoWidth = newWidth;
			if (!isPerspective())
			{
				updateProjectionMatrix();
			}
		}
	}

	void CameraComponent::onTransformChanged()
	{
		updateViewMatrix();
	}

	void CameraComponent::onGameStarted()
	{
		Super::onGameStarted();

		updateRotation(180.0f);
	}

	void CameraComponent::tick(double deltaTime)
	{
		Super::tick(deltaTime);

		updateRotation(m_Angle + deltaTime * 90.0f);
	}

	void CameraComponent::updateRotation(const float angle)
	{
		m_Angle = angle;
		while (m_Angle >= 360.0f)
		{
			m_Angle -= 360.0f;
		}
		
		const glm::vec3 direction = Rotation(0.0f, angle).toDirection();
		setWorldTransform({ direction * 50.0f, { 0.0f, angle + 180.0f } });
	}
}
