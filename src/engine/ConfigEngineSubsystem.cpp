// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/ConfigEngineSubsystem.h"

#include <filesystem>
#include <jutils/configs/ini_parser.h>

namespace JumaEngine
{
	bool ConfigEngineSubsystem::initSubsystem()
	{
		if (!Super::initSubsystem())
		{
			return false;
		}

		const std::string configDirectory = "./config/";
		if (std::filesystem::is_directory(configDirectory))
		{
			for (const auto& directoryEntry : std::filesystem::directory_iterator(configDirectory))
			{
				if (directoryEntry.is_directory())
				{
					continue;
				}

				const std::filesystem::path& filePath = directoryEntry.path();
				if (filePath.extension() != ".ini")
				{
					continue;
				}

				const jmap<jstring, jmap<jstring, jstring>> fileData = jutils::ini::parseFile(jstring(filePath.string()));
				if (fileData.isEmpty())
				{
					continue;
				}
			
				jmap<std::tuple<jstringID, jstringID>, jstring>& configData = m_LoadedConfigs.add(jstring(filePath.stem().string()));
				for (const auto& fileSectionData : fileData)
				{
					for (const auto& fileValue : fileSectionData.value)
					{
						configData.add({ fileSectionData.key, fileValue.key }, fileValue.value);
					}
				}
			}
		}
		return true;
	}

	void ConfigEngineSubsystem::clearSubsystem()
	{
		m_LoadedConfigs.clear();

		Super::clearSubsystem();
	}

	bool ConfigEngineSubsystem::getValue(const jstringID& config, const jstringID& section, const jstringID& key, jstring& outValue) const
	{
		const jmap<std::tuple<jstringID, jstringID>, jstring>* configData = m_LoadedConfigs.find(config);
		const jstring* value = configData != nullptr ? configData->find({section, key}) : nullptr;
		if (value == nullptr)
		{
			return false;
		}
		outValue = *value;
		return true;
	}
}
