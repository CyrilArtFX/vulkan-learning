# Install script for directory: F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/lib/Debug/assimp-vc143-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/lib/Release/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/lib/MinSizeRel/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/lib/RelWithDebInfo/assimp-vc143-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/bin/Debug/assimp-vc143-mtd.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/bin/Release/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/bin/MinSizeRel/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/bin/RelWithDebInfo/assimp-vc143-mt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/anim.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/aabb.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/ai_assert.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/camera.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/color4.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/color4.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/config.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/defs.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Defines.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/cfileio.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/light.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/material.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/material.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/matrix3x3.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/matrix3x3.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/matrix4x4.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/matrix4x4.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/mesh.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/pbrmaterial.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/postprocess.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/quaternion.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/quaternion.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/scene.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/metadata.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/texture.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/types.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/vector2.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/vector2.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/vector3.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/vector3.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/version.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/cimport.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/importerdesc.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Importer.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/DefaultLogger.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/ProgressHandler.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/IOStream.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/IOSystem.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Logger.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/LogStream.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/NullLogger.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/cexport.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Exporter.hpp"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/DefaultIOStream.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/DefaultIOSystem.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/ZipArchiveIOSystem.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/SceneCombiner.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/fast_atof.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/qnan.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/BaseImporter.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Hash.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/MemoryIOWrapper.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/ParsingUtils.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/StreamReader.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/StreamWriter.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/StringComparison.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/StringUtils.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/SGSpatialSort.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/GenericProperty.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/SpatialSort.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/SkeletonMeshBuilder.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/SmoothingGroups.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/SmoothingGroups.inl"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/StandardShapes.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/RemoveComments.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Subdivision.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Vertex.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/LineSplitter.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/TinyFormatter.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Profiler.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/LogAux.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Bitmap.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/XMLTools.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/IOStreamBuffer.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/CreateAnimMesh.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/irrXMLWrapper.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/BlobIOSystem.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/MathFunctions.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Macros.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Exceptional.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/ByteSwapper.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Compiler/pushpack1.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Compiler/poppack1.h"
    "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/Debug/assimp-vc143-mtd.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "F:/ArtFX_Exercices/vulkan-learning/externals/assimp-5.0.1/code/RelWithDebInfo/assimp-vc143-mt.pdb")
  endif()
endif()

