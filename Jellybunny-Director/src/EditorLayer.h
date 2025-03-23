#pragma once

#include "Jellybunny.h"
#include "Panels/SceneHierarachyPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Jellybunny/Renderer/EditorCamera.h"

namespace Jellybunny
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep time) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void OnOverlayRender();
		void NewScene();
		void LoadScene();
		void LoadScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();

		void OnSceneSimulate();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();
	private:
		OrthographicCameraController m_CameraController;

		Ref<VertexArray> m_VertexArray;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_FrameBuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;

		Ref<Texture2D> m_Texture;

		bool m_ViewportFocused, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		std::string m_FilePath = "";

		glm::vec4 m_Color = { 1.0f, 1.0f, 0.0f, 1.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

		bool m_ShowPhysicsColliders = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		AssetBrowserPanel m_AssetBrowserPanel;

		// Editor Resources
		Ref<Texture2D> m_IconPlay, m_IconStop, m_IconSimulate;


	};
}