#pragma once

#include "Jellybunny/Core/Timestep.h"
#include "Jellybunny/Core/UUID.h"
#include "Jellybunny/Renderer/EditorCamera.h"

#include "entt.hpp"

class b2World;

namespace Jellybunny
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void KillEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateEditor(Timestep time, EditorCamera& camera);
		void OnUpdateRuntime(Timestep time);
		void OnViewportResize(uint32_t width, uint32_t height);
		void OnUpdateSimulation(Timestep time, EditorCamera& camera);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DInit();
		void OnPhysics2DDeath();

		void RenderScene(EditorCamera& camera);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}