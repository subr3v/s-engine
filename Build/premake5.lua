function addModuleFiles(moduleName)
	includedirs { "../" .. moduleName .. "/include" }
	local path = "../" .. moduleName .. "/"
	files { path .. "include/**.h", path .. "include/**.hpp" , path .. "source/**.cpp" }
end

function dependsOn( args )
	for i,moduleName in ipairs(args) do
		includedirs { "../" .. moduleName .. "/include" }
		links { moduleName }
	end
end

function includesOnly(moduleName)
	includedirs { "../" .. moduleName .. "/include" }
end

function includeBoostModule( modules )
    for i,moduleName in ipairs(modules) do
		includedirs { "../Externals/booost" .. moduleName .. "/include" }
	end
end

workspace "Engine_Solution"
	configurations { "Debug", "Release" }
	location "../ProjectFiles"
	includedirs { "../include" }
	includedirs { "../Externals/imgui" }
	includedirs { "../Externals/glm" }
	includedirs { "../Externals/assimp/include" }
	includedirs { "../Engine_Core/include" }
	includedirs { "../Externals/zlib" }
	includedirs { "../Externals/lua/src" }
	includedirs { "../Externals/zlib", "../Externals/minizip" }
	defines { "GLM_FORCE_RADIANS", "_USE_MATH_DEFINES", "_WINSOCKAPI_", "GLM_SWIZZLE" , "NOMAXMIN" , "WIN32_LEAN_AND_MEAN" }
	libdirs { "../Externals/Libraries" }
	disablewarnings { "4996", "4800", "4005" }
	links { "ws2_32" , "d3d11", "d3dcompiler" , "Dxguid" }
	language "C++"
	flags { "WinMain", "MultiProcessorCompile", "Symbols" }
	debugdir "../Working_Directory"

	filter "configurations:Debug"
		defines { "DEBUG", "_DEBUG" }
		optimize "Debug"
		targetdir "../Binaries/Debug"

	filter "configurations:Release"
		defines { "NDEBUG" }
		flags { "LinkTimeOptimization" }
		optimize "Full"
		targetdir "../Binaries/Release"
   
project "Engine_Core"
	kind "StaticLib"
	addModuleFiles("Engine_Core")
	
project "Engine_Graphics"
	kind "StaticLib"
	addModuleFiles("Engine_Graphics")
	dependsOn { "Engine_Core", "Engine_Serialization" }

project "Engine_Rendering"
	kind "StaticLib"
	addModuleFiles("Engine_Rendering")
	dependsOn { "Engine_Core" , "Engine_Graphics", "Engine_Serialization" }

project "Engine_Networking"
	kind "StaticLib"
	defines { "_WINSOCK_DEPRECATED_NO_WARNINGS" }
	addModuleFiles("Engine_Networking")
	dependsOn { "Engine_Core", "Engine_Serialization" }

project "Engine_Serialization"
	kind "StaticLib"
	addModuleFiles("Engine_Serialization")
	dependsOn { "Engine_Core" }
	links { "External_zlib" , "External_minizip" }

project "Engine_ImGui"
	kind "StaticLib"
	addModuleFiles("Engine_ImGui")
	dependsOn { "Engine_Core" , "Engine_Graphics", "Engine_Serialization", "Engine_Networking" , "Engine_Rendering" }
	
project "Engine_Framework"
	kind "StaticLib"
	addModuleFiles("Engine_Framework")
	dependsOn { "Engine_Core" , "Engine_Graphics", "Engine_Serialization", "Engine_Networking" , "Engine_Rendering", "Engine_ImGui" ,
	 "Engine_LuaModule" }

project "Engine_LuaModule"
	kind "StaticLib"
	addModuleFiles("Engine_LuaModule")
	dependsOn { "External_lua", "Engine_Serialization", "Engine_Core" }
	
project "SEngine"
	kind "StaticLib"
	addModuleFiles("SEngine")
	dependsOn { "Engine_Core" , "Engine_Graphics", "Engine_Serialization", "Engine_Networking" , "Engine_Rendering", "Engine_ImGui", "Engine_Framework" }
	
project "3D_Demo"
	kind "WindowedApp"
	addModuleFiles("3D_Demo")
	dependsOn { "Engine_Core" , "Engine_Graphics", "Engine_Serialization", "Engine_Networking" , "Engine_Rendering", "Engine_ImGui", "Engine_Framework", "Engine_LuaModule" }
	dependsOn { "SEngine" }
	links { "External_imgui" }
	
project "FFTA_Demake"
	kind "WindowedApp"
	addModuleFiles("FFTA_Demake")
	dependsOn { "Engine_Core" , "Engine_Graphics", "Engine_Serialization", "Engine_Networking" , "Engine_Rendering", "Engine_ImGui", "Engine_Framework", "Engine_LuaModule" }
	dependsOn { "SEngine" }
	links { "External_imgui" }

project "ProceduralGenerationApp"
	kind "WindowedApp"
	addModuleFiles("ProceduralGenerationApp")
	dependsOn { "Engine_Core" , "Engine_Graphics", "Engine_Serialization", "Engine_Networking" , "Engine_Rendering", "Engine_ImGui", "Engine_Framework", "Engine_LuaModule" }
	dependsOn { "SEngine" }
	links { "External_imgui" }
	
project "Asset_Texture_Loader"
	kind "SharedLib"
	addModuleFiles("Asset_Texture_Loader")
	dependsOn{ "Engine_Core", "Engine_Serialization", "Engine_Graphics" }
	includesOnly( "Engine_Asset_Converter" )
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
project "Asset_Model_Loader"
	kind "SharedLib"
	addModuleFiles("Asset_Model_Loader")
	dependsOn{ "Engine_Core", "Engine_Serialization", "Engine_Graphics", "Engine_Rendering" }
	includesOnly( "Engine_Asset_Converter" )
	defines { "_CRT_SECURE_NO_WARNINGS" }
	links { "External_Assimp" }

project "Engine_Asset_Converter"
	kind "ConsoleApp"
	addModuleFiles("Engine_Asset_Converter")
	dependsOn{ "Engine_Core", "Engine_Serialization" }
	links { "External_zlib" , "External_minizip" }
	includedirs { "../Externals/zlib", "../Externals/minizip" }
	links { "External_zlib" , "External_minizip" }

project "External_imgui"
	kind "StaticLib"
	files { "../Externals/imgui/imgui.cpp" }
	files { "../Externals/imgui/imgui_draw.cpp" }

project "External_zlib"
	kind "StaticLib"
	language "C"
	files { "../Externals/zlib/*.c" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	disablewarnings { "4996" }

project "External_lua"
	kind "StaticLib"
	language "C++"
	files { "../Externals/lua/src/*.c", "../Externals/lua/src/*.h" }
	
project "External_minizip"
	kind "StaticLib"
	language "C"
	files { "../Externals/minizip/*.c" }
	links { "External_zlib" }
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
project "External_Assimp"
	kind "SharedLib"
	files { "../Externals/assimp/code/**.cpp", "../Externals/assimp/code/**.c" }	
	files { "../Externals/assimp/contrib/**.cpp", "../Externals/assimp/contrib/**.c" }
	includedirs { "../Externals/assimp/code/BoostWorkaround/" }
	includedirs { "../Externals/assimp/contrib/**/include" }
	includedirs { "../Externals/rapidjson/include/" }
	includedirs { "../Externals/assimp-revision-file" }
	disablewarnings { "4351", "4800" }
	defines { "ASSIMP_BUILD_BOOST_WORKAROUND", "ASSIMP_BUILD_SINGLETHREADED", "ASSIMP_BUILD_DLL_EXPORT" }
	defines { "ASSIMP_BUILD_NO_C4D_IMPORTER", "ASSIMP_BUILD_NO_OPENGEX_IMPORTER", "OPENDDL_STATIC_LIBARY","ASSIMP_BUILD_NO_BLEND_IMPORTER", "ASSIMP_BUILD_NO_IFC_IMPORTER" , "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }