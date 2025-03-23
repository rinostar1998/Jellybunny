project "Jellybunny-Director"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    buildoptions{"/utf-8"}

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Jellybunny/vendor/spdlog/include",
		"%{wks.location}/Jellybunny/src",
		"%{wks.location}/Jellybunny/vendor",
		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Jellybunny"
	}

	filter "system:windows"
		systemversion "latest"

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