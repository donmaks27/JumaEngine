// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "TextureFileImporter_DevIL.h"
#include "IL/il.h"
#include "utils/jlog.h"

namespace JumaEngine
{
    void TextureFileImporter_DevIL::onRegister()
    {
        Super::onRegister();

        ilInit();
    }

    bool TextureFileImporter_DevIL::importFileInternal(const asset_ptr<TextureBase>& outTexture, const jstring& filePath)
    {
        const size_t filePathLength = filePath.size();
        std::wstring wFilePath;
        wFilePath.reserve(filePathLength);
        size_t convertetCharsCount = 0;
        mbstowcs_s(&convertetCharsCount, wFilePath.data(), filePathLength + 1, filePath.c_str(), filePathLength);

        const ILuint imageIndex = ilGenImage();
        ilBindImage(imageIndex);
        if (ilLoadImage(wFilePath.c_str()) == IL_FALSE)
        {
#if JLOG_ENABLED
            const ILenum errorCode = ilGetError();
            JUMA_LOG(warning, jstring(JTEXT("Fail to load image. Code: ")) + TO_JTEXT(errorCode));
#endif
            return false;
        }

        bool result = false;
        const int32 type = ilGetInteger(IL_IMAGE_TYPE);
        if (type == IL_UNSIGNED_BYTE)
        {
            TextureFormat format = TextureFormat::None;
            switch (ilGetInteger(IL_IMAGE_FORMAT))
            {
            case IL_RGB: format = TextureFormat::R8G8B8; break;
            case IL_RGBA: format = TextureFormat::R8G8B8A8; break;
            case IL_BGR: format = TextureFormat::B8G8R8; break;
            case IL_BGRA: format = TextureFormat::B8G8R8A8; break;
            case IL_ALPHA:
            case IL_LUMINANCE:
            case IL_LUMINANCE_ALPHA: format = TextureFormat::R8; break;
            default: ;
            }
            if (format != TextureFormat::None)
            {
                const uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
                const uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);
                const uint8* data = ilGetData();
                if ((width > 0) && (height > 0) && (data != nullptr))
                {
                    copyTextureData(outTexture, { width, height }, format, data);
                    result = true;
                }
            }
        }

        ilDeleteImage(imageIndex);
        return result;
    }
}
