// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/EngineObjectPtr.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
	Engine* EngineObjectPtrBase::s_Engine = nullptr;

	bool EngineObjectPtrBase::isDescriptorValid() const
	{
		return (s_Engine != nullptr) && s_Engine->m_EngineObjectDescriptors.isValid(m_ObjectPointer);
	}
	EngineContextObject* EngineObjectPtrBase::getObject() const
	{
		return s_Engine != nullptr ? s_Engine->m_EngineObjectDescriptors.get(m_ObjectPointer) : nullptr;
	}
	EngineObjectPtrBase::PointerType EngineObjectPtrBase::GetPointerFromObject(EngineContextObject* object)
	{
		return object != nullptr ? object->m_ObjectDescriptor : nullptr;
	}

	bool EngineObjectPtrBase::addReference()
	{
		return (s_Engine != nullptr) && s_Engine->m_EngineObjectDescriptors.addReference(m_ObjectPointer);
	}
	bool EngineObjectPtrBase::removeReference()
	{
		return (s_Engine != nullptr) && s_Engine->m_EngineObjectDescriptors.removeReference(m_ObjectPointer);
	}
}
