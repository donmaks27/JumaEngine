cmake_minimum_required(VERSION 3.12)

project(JumaEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(JumaRE)
add_subdirectory(JumaEngineContent)

list(APPEND JUMA_ENGINE_HEADER_FILES
    include/JumaEngine/core.h

    include/JumaEngine/assets/Asset.h
    include/JumaEngine/assets/AssetsEngineSubsystem.h
    include/JumaEngine/assets/AssetType.h
    include/JumaEngine/assets/Material.h
    include/JumaEngine/assets/MaterialBase.h
    include/JumaEngine/assets/MaterialInstance.h
    include/JumaEngine/assets/MaterialParamType.h
    include/JumaEngine/assets/Mesh.h
    include/JumaEngine/assets/Texture.h
    include/JumaEngine/assets/TextureBase.h
    include/JumaEngine/assets/VertexBuffer.h
    include/JumaEngine/assets/VertexComponent.h

    include/JumaEngine/engine/AsyncEngineSubsystem.h
    include/JumaEngine/engine/ConfigEngineSubsystem.h
    include/JumaEngine/engine/Engine.h
    include/JumaEngine/engine/EngineClass.h
    include/JumaEngine/engine/EngineContextObject.h
    include/JumaEngine/engine/EngineObject.h
    include/JumaEngine/engine/EngineObjectOwner.h
    include/JumaEngine/engine/EngineObjectPtr.h
    include/JumaEngine/engine/EngineSubsystem.h

    include/JumaEngine/game/GameEngine.h
    include/JumaEngine/game/GameInstance.h

    include/JumaEngine/render/RenderContext.h
    include/JumaEngine/render/RenderEngineSubsystem.h
    include/JumaEngine/render/RenderTarget.h

    include/JumaEngine/widgets/CursorWidget.h
    include/JumaEngine/widgets/ImageWidget.h
    include/JumaEngine/widgets/OverlayWidget.h
    include/JumaEngine/widgets/Widget.h
    include/JumaEngine/widgets/WidgetAlignment.h
    include/JumaEngine/widgets/WidgetContainer.h
    include/JumaEngine/widgets/WidgetContext.h
    include/JumaEngine/widgets/WidgetsCreator.h
)
list(APPEND JUMA_ENGINE_PRIVATE_HEADER_FILES
    src/assets/AssetFilesParsing.h
)
list(APPEND JUMA_ENGINE_SOURCE_FILES
    src/assets/AssetFilesParsing.cpp
    src/assets/AssetsEngineSubsystem.cpp
    src/assets/Material.cpp
    src/assets/MaterialBase.cpp
    src/assets/MaterialInstance.cpp
    src/assets/Mesh.cpp
    src/assets/Texture.cpp

    src/engine/AsyncEngineSubsystem.cpp
    src/engine/ConfigEngineSubsystem.cpp
    src/engine/Engine.cpp
    src/engine/EngineObject.cpp
    src/engine/EngineObjectOwner.cpp
    src/engine/EngineObjectPtr.cpp

    src/game/GameEngine.cpp
    src/game/GameInstance.cpp

    src/render/RenderEngineSubsystem.cpp
    src/render/RenderTarget.cpp

    src/widgets/CursorWidget.cpp
    src/widgets/ImageWidget.cpp
    src/widgets/OverlayWidget.cpp
    src/widgets/Widget.cpp
    src/widgets/WidgetContainer.cpp
    src/widgets/WidgetContext.cpp
    src/widgets/WidgetsCreator.cpp
)

add_library(JumaEngine STATIC ${JUMA_ENGINE_SOURCE_FILES})
target_include_directories(JumaEngine PUBLIC include PRIVATE
    third_party/nothing/include
)
target_link_libraries(JumaEngine PUBLIC JumaRE)