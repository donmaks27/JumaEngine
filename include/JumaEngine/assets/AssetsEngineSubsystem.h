// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineSubsystem.h"

#include <jutils/jlist.h>
#include <jutils/jmap.h>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexComponent.h"

namespace JumaEngine
{
    JUTILS_CREATE_MULTICAST_DELEGATE2(OnAssetsEngineSubsystemMaterialParamEvent, AssetsEngineSubsystem*, subsystem, const jstringID&, paramName);

	class AssetsEngineSubsystem final : public EngineSubsystem
	{
		JUMAENGINE_CLASS(AssetsEngineSubsystem, EngineSubsystem)

	public:
		AssetsEngineSubsystem() = default;
		virtual ~AssetsEngineSubsystem() override = default;

        OnAssetsEngineSubsystemMaterialParamEvent onEngineInternalParamChanged;


		void clear();

		EngineObjectPtr<Texture> getEngineTexture(const jstringID& textureName);
        EngineObjectPtr<Texture> getTexture(const jstringID& textureName);

		EngineObjectPtr<Shader> getEngineShader(const jstringID& shaderName);
        EngineObjectPtr<Shader> getShader(const jstringID& shaderName);

		Material* createMaterial(const EngineObjectPtr<Shader>& shader);
        Material* createMaterial(Material* baseMaterial);
        void destroyMaterial(Material* material);

        template<JumaRE::ShaderUniformType Type>
        bool setGlobalParamValue(const jstringID& name, const typename JumaRE::ShaderUniformInfo<Type>::value_type& value)
        {
            if (!m_GlobalMaterialParams.contains(name, Type) || !m_GlobalMaterialParams.setValue<Type>(name, value))
            {
                return false;
            }
            onEngineInternalParamChanged.call(this, name);
            return true;
        }
        template<JumaRE::ShaderUniformType Type>
        bool getGlobalParamValue(const jstringID& name, typename JumaRE::ShaderUniformInfo<Type>::value_type& outValue) const
        {
            return m_GlobalMaterialParams.getValue<Type>(name, outValue);
        }
        const JumaRE::MaterialParamsStorage& getGlobalParams() const { return m_GlobalMaterialParams; }

        jstringID getVertexComponentID(VertexComponent component) const;
        JumaRE::VertexBuffer* getVertexBuffer_Cube();
        JumaRE::VertexBuffer* getVertexBuffer_Plane2D();

	protected:

		virtual bool initSubsystem() override;
		virtual void clearSubsystem() override;

	private:

        jmap<jstringID, EngineObjectPtr<Texture>> m_EngineTextures;
        jmap<jstringID, EngineObjectPtr<Texture>> m_Textures;

		jmap<jstringID, EngineObjectPtr<Shader>> m_EngineShaders;
        jmap<jstringID, EngineObjectPtr<Shader>> m_Shaders;

		jlist<Material> m_Materials;
        jarray<const Material*> m_DestroyingMaterials;
		JumaRE::MaterialParamsStorage m_GlobalMaterialParams;

        jmap<VertexComponent, jstringID> m_VertexComponentIDs;
        jlist<Mesh> m_Meshes;
        JumaRE::VertexBuffer* m_VertexBuffer_Cube = nullptr;
        JumaRE::VertexBuffer* m_VertexBuffer_Plane2D = nullptr;


		void onRenderEngineDestroying(JumaRE::RenderEngine* renderEngine) { clear(); }

		EngineObjectPtr<Texture> getTexture(jmap<jstringID, EngineObjectPtr<Texture>>& texturesList, const jstringID& textureName, const jstring& contentFolder) const;
		EngineObjectPtr<Shader> getShader(jmap<jstringID, EngineObjectPtr<Shader>>& shadersList, const jstringID& shaderName, const jstring& contentFolder) const;

        void onMaterialClear(Material* material);
	};
}
