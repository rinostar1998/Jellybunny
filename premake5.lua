workspace "Jellybunny"
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

include "Jellybunny/vendor/GLFW"

project "Jellybunny"
	location "Jellybunny"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "jbpch.h"
	pchsource "Jellybunny/src/jbpch.cpp"


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}


	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"User32.lib",
		"Gdi32.lib",
		"Shell32.lib"
	}

	buildoptions{"/utf-8"}

	staticruntime "off"
	runtime "Debug"

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"JB_PLATFORM_WINDOWS",
			"JB_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox-Test")
		}


	filter "configurations:Debug"
		defines "JB_DEBUG"
		buildoptions "/MTd"
		symbols "On"

	filter "configurations:Release"
		defines "JB_RELEASE"
		buildoptions "/MT"
		optimize "On"

	filter "configurations:Dist"
		defines "JB_DIST"
		buildoptions "/MT"
		optimize "On"

project "Sandbox-Test"
	location "Sandbox-Test"
	kind "ConsoleApp"
	language "C++"

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
		"Jellybunny/src"
	}

	links
	{
		"Jellybunny"
	}

	buildoptions{"/utf-8"}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"JB_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "JB_DEBUG"
		buildoptions "/MTd"
		symbols "On"

	filter "configurations:Release"
		defines "JB_RELEASE"
		buildoptions "/MT"
		optimize "On"

	filter "configurations:Dist"
		defines "JB_DIST"
		buildoptions "/MT"
		optimize "On"