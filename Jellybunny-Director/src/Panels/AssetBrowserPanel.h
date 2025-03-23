#pragma once
#include <filesystem>
#include "Jellybunny/Renderer/Texture.h"

namespace Jellybunny
{
	class AssetBrowserPanel
	{
	public:
		AssetBrowserPanel();
		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}