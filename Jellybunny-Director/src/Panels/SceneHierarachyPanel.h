#pragma once
#include "Jellybunny/Scene/Scene.h"
#include "Jellybunny/Core/Core.h"
#include "Jellybunny/Core/Log.h"
#include "Jellybunny/Scene/Entity.h"

namespace Jellybunny
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponent(const std::string& entryName, const std::string& errorNameA, const std::string& errorNameB);
		void DrawEntityNode(Entity entity);
		void DrawProperties(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}