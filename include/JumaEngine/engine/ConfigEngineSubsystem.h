// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "EngineSubsystem.h"

#include <jutils/jmap.h>
#include <jutils/jstringID.h>

namespace JumaEngine
{
	class ConfigEngineSubsystem : public EngineSubsystem
	{
		JUMAENGINE_CLASS(ConfigEngineSubsystem, EngineSubsystem)

	public:
		ConfigEngineSubsystem() = default;
		virtual ~ConfigEngineSubsystem() override = default;

		bool getValue(const jstringID& config, const jstringID& section, const jstringID& key, jstring& outValue) const;

	protected:

		virtual bool initSubsystem() override;
		virtual void clearSubsystem() override;

	private:

		jmap<jstringID, jmap<std::tuple<jstringID, jstringID>, jstring>> m_LoadedConfigs;
	};
}
