cmake_minimum_required(VERSION 3.24)
project(AG_CLion)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

set(FONTS ./Fonts)
set(GEOMETRY ./Geometry)
set(LIBRARIES ../Libraries)
set(PATTERNS ./Patterns)
set(PRECOMPILED_HEADERS ./PrecompiledHeaders)
set(RENDERING ./Rendering)
set(VCPKG C:/vcpkg/installed/x64-windows)

include_directories(${FONTS})
include_directories(${GEOMETRY})
include_directories(${LIBRARIES})
include_directories(${PATTERNS})
include_directories(${PRECOMPILED_HEADERS})
include_directories(${RENDERING})
include_directories(${LIBRARIES}/ImGuiFileDialog)
include_directories(${LIBRARIES}/lodepng)
include_directories(${VCPKG}/include/)
include_directories(${VCPKG}/include/assimp)
include_directories(${VCPKG}/include/GLFW/include)
include_directories(${VCPKG}/include/gl)
include_directories(${VCPKG}/include/glm)

set_source_files_properties(stdafx.cpp
        PROPERTIES
        COMPILE_FLAGS "-include stdafx.h")

link_directories(${VCPKG}/lib)

set(SOURCE_FILES
        main.cpp
        ${GEOMETRY}/AABB.cpp
        ${GEOMETRY}/AABB.h
        ${GEOMETRY}/BasicGeometry.h
        ${GEOMETRY}/ChronoUtilities.h
        ${GEOMETRY}/Circle.cpp
        ${GEOMETRY}/Circle.h
        ${GEOMETRY}/Line.cpp
        ${GEOMETRY}/Line.h
        ${GEOMETRY}/Point.cpp
        ${GEOMETRY}/Point.h
        ${GEOMETRY}/PointCloud.cpp
        ${GEOMETRY}/PointCloud.h
        ${GEOMETRY}/Polygon.cpp
        ${GEOMETRY}/Polygon.h
        ${GEOMETRY}/RandomUtilities.h
        ${GEOMETRY}/RayLine.cpp
        ${GEOMETRY}/RayLine.h
        ${GEOMETRY}/SegmentLine.cpp
        ${GEOMETRY}/SegmentLine.h
        ${GEOMETRY}/Triangle.cpp
        ${GEOMETRY}/Triangle.h
        ${GEOMETRY}/Vect2d.cpp
        ${GEOMETRY}/Vect2d.h
        ${GEOMETRY}/Vertex.cpp
        ${GEOMETRY}/Vertex.h
        ${GEOMETRY}/Voxelization.cpp
        ${GEOMETRY}/Voxelization.h
        ${PATTERNS}/Singleton.h
        ${PRECOMPILED_HEADERS}/InclDraw2D.h
        ${PRECOMPILED_HEADERS}/InclGeom2D.h
        ${PRECOMPILED_HEADERS}/stdafx.cpp
        ${PRECOMPILED_HEADERS}/stdafx.h
        ${RENDERING}/ApplicationState.h
        ${RENDERING}/Camera.cpp
        ${RENDERING}/Camera.h
        ${RENDERING}/CameraGuiAdapter.cpp
        ${RENDERING}/CameraGuiAdapter.h
        ${RENDERING}/CameraProjection.cpp
        ${RENDERING}/CameraProjection.h
        ${RENDERING}/DrawCircle.cpp
        ${RENDERING}/DrawCircle.h
        ${RENDERING}/DrawLine.cpp
        ${RENDERING}/DrawLine.h
        ${RENDERING}/DrawMesh.cpp
        ${RENDERING}/DrawMesh.h
        ${RENDERING}/DrawPoint.cpp
        ${RENDERING}/DrawPoint.h
        ${RENDERING}/DrawPointCloud.cpp
        ${RENDERING}/DrawPointCloud.h
        ${RENDERING}/DrawPolygon.cpp
        ${RENDERING}/DrawPolygon.h
        ${RENDERING}/DrawRay.cpp
        ${RENDERING}/DrawRay.h
        ${RENDERING}/DrawSegment.cpp
        ${RENDERING}/DrawSegment.h
        ${RENDERING}/DrawTriangle.cpp
        ${RENDERING}/DrawTriangle.h
        ${RENDERING}/DrawVoxelization.cpp
        ${RENDERING}/DrawVoxelization.h
        ${RENDERING}/FBO.cpp
        ${RENDERING}/FBO.h
        ${RENDERING}/FBOScreenshot.cpp
        ${RENDERING}/FBOScreenshot.h
        ${RENDERING}/GUI.cpp
        ${RENDERING}/GUI.h
        ${RENDERING}/GuiUtilities.cpp
        ${RENDERING}/GuiUtilities.h
        ${RENDERING}/Image.cpp
        ${RENDERING}/Image.h
        ${RENDERING}/InputManager.cpp
        ${RENDERING}/InputManager.h
        ${RENDERING}/InterfaceAdapter.h
        ${RENDERING}/Model3D.cpp
        ${RENDERING}/Model3D.h
        ${RENDERING}/Renderer.cpp
        ${RENDERING}/Renderer.h
        ${RENDERING}/RenderingShader.cpp
        ${RENDERING}/RenderingShader.h
        ${RENDERING}/SceneContent.cpp
        ${RENDERING}/SceneContent.h
        ${RENDERING}/ShaderProgram.cpp
        ${RENDERING}/ShaderProgram.h
        ${RENDERING}/ShaderProgramDB.cpp
        ${RENDERING}/ShaderProgramDB.h
        ${RENDERING}/Texture.cpp
        ${RENDERING}/Texture.h
        ${RENDERING}/TextureList.cpp
        ${RENDERING}/TextureList.h
        ${RENDERING}/VAO.cpp
        ${RENDERING}/VAO.h
        ${RENDERING}/Window.cpp
        ${RENDERING}/Window.h
        ${LIBRARIES}/ImGuiFileDialog/ImGuiFileDialog.h
        ${LIBRARIES}/ImGuiFileDialog/ImGuiFileDialog.cpp
        ${LIBRARIES}/lodepng/lodepng.h
        ${LIBRARIES}/lodepng/lodepng.cpp
        ${FONTS}/lato.hpp
        ${FONTS}/lato.cpp
        ${FONTS}/IconsFontAwesome5.h
        ${FONTS}/font_awesome.hpp
        ${FONTS}/font_awesome.cpp
        ${FONTS}/font_awesome_2.cpp)
add_executable(AG_CLion ${SOURCE_FILES})

target_link_libraries(AG_CLion imgui imguizmo glfw3dll glew32 OpenGL32 assimp-vc143-mt)

if (WIN32)
    set(VCPKG_BIN "${VCPKG}/bin")

    file(GLOB RUNTIME_DLLS
        "${VCPKG_BIN}/glew32.dll"
        "${VCPKG_BIN}/glfw3.dll"
        "${VCPKG_BIN}/assimp-vc143-mt.dll"
        "${VCPKG_BIN}/zlib1.dll"
        "${VCPKG_BIN}/poly2tri.dll"
        "${VCPKG_BIN}/minizip.dll"
        "${VCPKG_BIN}/pugixml.dll"
        "${VCPKG_BIN}/kubazip.dll"
    )

    add_custom_command(TARGET AG_CLion POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RUNTIME_DLLS}
        $<TARGET_FILE_DIR:AG_CLion>
        COMMAND_EXPAND_LISTS
    )

    # Assets
    set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/../vs/Source/Assets")
    if (EXISTS "${ASSETS_DIR}")
        add_custom_command(TARGET AG_CLion POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_DIR}"
            "$<TARGET_FILE_DIR:AG_CLion>/Assets"
        )
    endif()
endif()