workspace "Jellybunny"

	filter "action:vs*"
		toolset "v143"

	architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

	staticruntime "on"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Jellybunny/vendor/GLFW/include"
IncludeDir["Glad"] = "Jellybunny/vendor/Glad/include"
IncludeDir["ImGui"] = "Jellybunny/vendor/imgui"
IncludeDir["glm"] = "Jellybunny/vendor/glm"
IncludeDir["stb_image"] = "Jellybunny/vendor/stb_image"

include "Jellybunny/vendor/GLFW"
include "Jellybunny/vendor/Glad"
include "Jellybunny/vendor/imgui"

project "Jellybunny"

	location "Jellybunny"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "jbpch.h"
	pchsource "Jellybunny/src/jbpch.cpp"


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"User32.lib",
		"Gdi32.lib",
		"Shell32.lib"
	}

	buildoptions{"/utf-8"}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"JB_PLATFORM_WINDOWS",
			"JB_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}


	filter "configurations:Debug"
		defines "JB_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "JB_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "JB_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox-Test"
	location "Sandbox-Test"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Jellybunny/vendor/spdlog/include",
		"Jellybunny/src",
		"Jellybunny/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Jellybunny"
	}

	buildoptions{"/utf-8"}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"JB_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "JB_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "JB_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "JB_DIST"
		runtime "Release"
		optimize "on"
