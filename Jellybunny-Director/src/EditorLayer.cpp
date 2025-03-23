#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Jellybunny/Scene/SceneSerializer.h"

#include "Jellybunny/Utils/PlatformUtils.h"

#include "ImGuizmo.h"

#include "Jellybunny/Meth/Math.h"

namespace Jellybunny
{
	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_Color({ 0.2f, 0.3f, 0.8f, 1.0f })
	{
	}

	void EditorLayer::OnAttach()
	{
		JB_PROFILE_FUNCTION();

		m_CameraController.SetZoomLevel(5.0f);
		m_Texture	   = Jellybunny::Texture2D::Create("assets/textures/Frown.bmp");
		m_IconPlay	   = Jellybunny::Texture2D::Create("Resources/Icons/Play.png");
		m_IconStop	   = Jellybunny::Texture2D::Create("Resources/Icons/Stop.png");
		m_IconSimulate = Jellybunny::Texture2D::Create("Resources/Icons/Simulate.png");

		FramebufferSpecs fbspec;
		fbspec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbspec.Width = 1280;
		fbspec.Height = 720;
		m_FrameBuffer = Framebuffer::Create(fbspec);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpecification().commandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(90.0f, 1.778f, 0.1f, 10000.0f);
		Renderer2D::SetLineThickness(3.0f);
	}

	void EditorLayer::OnDetach()
	{
		JB_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep time)
	{
		JB_PROFILE_FUNCTION();

		if (FramebufferSpecs spec = m_FrameBuffer->GetSpec();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
		// Render
		Renderer2D::ResetStats();
		m_FrameBuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0, 0, 1 });
		RenderCommand::Clear();

		m_FrameBuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				if (m_ViewportFocused) m_CameraController.OnUpdate(time);
				m_EditorCamera.OnUpdate(time);

				m_ActiveScene->OnUpdateEditor(time, m_EditorCamera);
				break;
			}
			case SceneState::Simulate:
			{
				m_EditorCamera.OnUpdate(time);

				m_ActiveScene->OnUpdateSimulation(time, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(time);
				break;
			}
		}

		// UpdateScene

		auto [x, y] = ImGui::GetMousePos();
		x -= m_ViewportBounds[0].x;
		y -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		y = viewportSize.y - y;
		int mouseX = (int)x;
		int mouseY = (int)y;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int entity = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = entity == -1 ? Entity() : Entity((entt::entity)entity, m_ActiveScene.get());
		}

		OnOverlayRender();

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		JB_PROFILE_FUNCTION();

		static bool dockSpaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "LCtrl+N")) NewScene();
				if (ImGui::MenuItem("Load...", "LCtrl+O")) LoadScene();
				if (ImGui::MenuItem("Save", "LCtrl+S")) SaveScene();
				if (ImGui::MenuItem("Save As...", "LCtrl+Alt+S")) SaveSceneAs();
				if (ImGui::MenuItem("Exit")) Application::Get().Die();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();

			m_SceneHierarchyPanel.OnImGuiRender();
			m_AssetBrowserPanel.OnImGuiRender();

			ImGui::Begin("Stats");

			auto stats = Renderer2D::GetStats();
			ImGui::Text("Renderer2D : Stats");
			std::string name = "None";
			if (m_HoveredEntity)
				name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("HOV ENTITY : %s", name.c_str());

			ImGui::Text("DRAW CALLS : %d", stats.DrawCalls);
			ImGui::Text("QUAD COUNT : %d", stats.QuadCount);
			ImGui::Text("VERT COUNT : %d", stats.GetTotalVertexCount());
			ImGui::Text("INDX COUNT : %d", stats.GetTotalIndexCount());

			ImGui::End();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Physics Colliders?", &m_ShowPhysicsColliders);
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t texID = m_FrameBuffer->GetColorAttachmentRendererID();
			ImGui::Image(reinterpret_cast<void*>(texID), ImVec2{m_ViewportSize.x,  m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					LoadScene(std::filesystem::path(g_AssetPath) / path);
				}
				ImGui::EndDragDropTarget();
			}


			// gizmos
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				float windowWidth = ImGui::GetWindowWidth();
				float windowHeight = ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				const glm::mat4& camProj = m_EditorCamera.GetProjection();
				glm::mat4 camView = m_EditorCamera.GetViewMatrix();

				// Entity Transform
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4& transform = tc.GetTransform();

				// Snapping
				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = 
					(ImGuizmo::OPERATION(m_GizmoType) == ImGuizmo::OPERATION::TRANSLATE ? 0.5f : 
					(ImGuizmo::OPERATION(m_GizmoType) == ImGuizmo::OPERATION::ROTATE ? 10.0f : 0.125f));

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(camView), glm::value_ptr(camProj), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 rotation;
					Math::DecomposeTransform(transform, tc.position, rotation, tc.scale);
					glm::vec3 deltaRotation = rotation - tc.rotation;
					tc.rotation += deltaRotation;
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_ButtonHovered].x, colors[ImGuiCol_ButtonHovered].y, colors[ImGuiCol_ButtonHovered].z, 0.25f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(colors[ImGuiCol_ButtonActive].x, colors[ImGuiCol_ButtonActive].y, colors[ImGuiCol_ButtonActive].z, 0.5f));

		ImGui::Begin("##toolbox", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;
		ImVec4 tintColor = toolbarEnabled ? ImVec4(1, 1, 1, 1) : ImVec4(0.5, 0.1, 0.1, 1);

		{
			float size = ImGui::GetWindowHeight() - 4;
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			ImGui::SameLine();
			if (toolbarEnabled && ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0, 0, 0, 0), tintColor))
			{
				if (m_SceneState == SceneState::Edit) OnScenePlay();
				else if (m_SceneState == SceneState::Play) OnSceneStop();
			}
		}
		ImGui::SameLine();
		{
			float size = ImGui::GetWindowHeight() - 4;
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (toolbarEnabled && ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0, 0, 0, 0), tintColor))
			{
				if (m_SceneState == SceneState::Edit) OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate) OnSceneStop();
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		if(m_SceneState == SceneState::Edit) m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(JB_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(JB_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if(m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shorcuts
		if (e.IsRepeat()) return false;
		bool ctrl = (Input::IsKeyPressed(Key::LeftControl));
		bool altCtrl = (Input::IsKeyPressed(Key::RightControl)); // hehehe~, there'll be even MORE shortcuts when right ctrl is pressed and nobody will notice!!!
		bool shift = (Input::IsKeyPressed(Key::LeftShift));
		bool altShift = (Input::IsKeyPressed(Key::RightShift)); // hehehe~, there'll be even MORE shortcuts when right shift is pressed and nobody will notice!!!
		bool alt = (Input::IsKeyPressed(Key::LeftAlt));
		bool altAlt = (Input::IsKeyPressed(Key::RightAlt)); // hehehe~, there'll be even MORE shortcuts when right alt is pressed and nobody will notice!!!
		bool rightMouseDown = Input::IsMouseButtonPressed(1);
		switch (e.GetKeyCode())
		{
			case (int)Key::S: { if (ctrl && alt) SaveSceneAs(); else if (ctrl) SaveScene(); break; }
			case (int)Key::N: { if (ctrl) NewScene(); break; }
			case (int)Key::O: { if (ctrl) LoadScene(); break; }
			case (int)Key::D: { if (ctrl) OnDuplicateEntity(); break; }
			case (int)Key::Q: { if (!shift) m_GizmoType = -1; break; }
			case (int)Key::W: { if (!shift) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break; }
			case (int)Key::E: { if (!shift) m_GizmoType = ImGuizmo::OPERATION::ROTATE; break; }
			case (int)Key::R: { if (!shift) m_GizmoType = ImGuizmo::OPERATION::SCALE; break; }
		}
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			auto camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera) return;
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else Renderer2D::BeginScene(m_EditorCamera);

		if(m_ShowPhysicsColliders)
		{
			// box colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 pos = tc.position + glm::vec3(bc2d.offset, 0.001f);
					glm::vec3 scale = tc.scale * glm::vec3(bc2d.size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.position)
						* glm::rotate(glm::mat4(1.0f), tc.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.offset, 0.001f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawWireframeRect(transform, glm::vec4(0, 0, 1, 1));
				}
			}

			// ball colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BallCollider2DComponent>();
				for (auto entity : view)
				{
					float epsilon = 0.0001f;
					glm::vec3 camForwardDir = m_EditorCamera.GetForwardDirection();
					glm::vec3 projCollider = camForwardDir * glm::vec3(epsilon);

					auto [tc, bc2d] = view.get<TransformComponent, BallCollider2DComponent>(entity);
					glm::vec3 pos = tc.position + glm::vec3(bc2d.offset, -projCollider.z);
					glm::vec3 scale = tc.scale * glm::vec3(bc2d.radius * 2);
					glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.0f), pos), scale);
					Renderer2D::DrawCircle(transform, glm::vec4(0, 0, 1, 1), 0.1f);
				}
			}
		}

		// Draw selected entity outline 
		if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
		{
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer2D::DrawWireframeRect(transform.GetTransform(), glm::vec4(0.0f, 0.5f, 1.0f, 1.0f));
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::LoadScene()
	{
		std::string filepath = FileDialogs::LoadFile("Jellybunny Level (*.jbl)\0*.jbl\0");
		if (!filepath.empty())
		{
			LoadScene(filepath);
		}
	}

	void EditorLayer::LoadScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
		{
			JB_WAR("WoOaAh! ThAt felt weird, cutie! Do that again ^UwU^!");
			JB_WAR("...WHAT AM I GONNA DO WITH YOU THESE DAYS!?? ...weirdo.");
			OnSceneStop();
		}

		if (path.extension() != ".jbl")
		{
			JB_WAR("mmmm unh! ...~looks like ya loaded a different kind of file!~ ^.^");
			JB_WAR(" Unfortunately, it's not my type! ;)))");
			JB_WAR("--ENOUGH WITH THAT WEIRDO CRAP!");
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);
			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
			SceneSerializer serializer(scene);
			serializer.Serialize(path.string());
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Jellybunny Level (*.jbl)\0*.jbl\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty()) SerializeScene(m_ActiveScene, m_EditorScenePath);
		else SaveSceneAs();
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate) OnSceneStop();
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play) OnSceneStop();
		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
	}

	void EditorLayer::OnSceneStop()
	{
		JB_CORE_ASS(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);
		if(m_SceneState == SceneState::Play) m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate) m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}


	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit) return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity) m_EditorScene->DuplicateEntity(selectedEntity);
	}
}