include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Jellybunny"
	architecture "x86_64"
	startproject "Jellybunny-Director"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "Jellybunny/vendor/Box2D"
	include "Jellybunny/vendor/GLFW"
	include "Jellybunny/vendor/Glad"
	include "Jellybunny/vendor/imgui"
	include "Jellybunny/vendor/yaml-cpp"
group ""

include "Jellybunny"
include "Jellybunny-Director"
include "Sandbox-Test"