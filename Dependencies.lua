VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/Jellybunny/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/Jellybunny/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/Jellybunny/vendor/Box2D/include"
IncludeDir["GLFW"] = "%{wks.location}/Jellybunny/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Jellybunny/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Jellybunny/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Jellybunny/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Jellybunny/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/Jellybunny/vendor/entt/include"
IncludeDir["mono"] = "%{wks.location}/Jellybunny/vendor/mono/include"
IncludeDir["shaderc"] = "%{wks.location}/Jellybunny/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Jellybunny/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Jellybunny/vendor/VulkanSDK/Lib"
LibraryDir["mono"] = "%{wks.location}/Jellybunny/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}

Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows

Library["WinSock"] = "Ws2_32.lib"
Library["Winmm"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["Bcrypt"] = "Bcrypt.lib"