#include "SceneHierarachyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Jellybunny/Scene/Components.h"
#include <filesystem>

namespace Jellybunny
{
	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Tree");

		if (m_Context)
		{

			for (auto entityID : m_Context->m_Registry.view<entt::entity>())
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			}

			if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
				m_SelectionContext = {};

			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Worthless Entity")) m_Context->CreateEntity();
				ImGui::EndPopup();
			}

		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawProperties(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool EntityKilled = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Kill Entity")) EntityKilled = true;
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (EntityKilled) { m_Context->KillEntity(entity); if (m_SelectionContext == entity) { m_SelectionContext = {}; } }
	}

	static void DrawVec3Control(const std::string label, glm::vec3& values, float reset = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.2f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.1f, 0.0f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = reset;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.75f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 1.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.75f, 0.0f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = reset;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.1f, 0.75f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.2f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.1f, 0.75f, 1.0f));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = reset;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::Spacing();
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction function)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.4f);
			if (ImGui::Button(":", ImVec2{ 22.0f, 22.0f }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) { if (ImGui::MenuItem("Remove Component")) removeComponent = true; ImGui::EndPopup(); }

			if (open)
			{
				function(component);
				ImGui::TreePop();
			}
			if (removeComponent) entity.RemoveComponent<T>();
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponent(const std::string& entryName, const std::string& errorNameA, const std::string& errorNameB)
	{
		if (ImGui::MenuItem(entryName.c_str()))
		{
			if (!m_SelectionContext.HasComponent<T>())
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
			else
			{
				JB_CORE_WAR(errorNameA);
				if (!errorNameB.empty()) JB_CORE_WAR(errorNameB);
			}
		}
	}

	void SceneHierarchyPanel::DrawProperties(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");
		{
			if (ImGui::BeginPopup("AddComponent"))
			{
				DisplayAddComponent<CameraComponent>("Camera", "~unh~... ~Did ya forget ya added the component already~? (^w^) ..!(UwU)!", "STOP SEDUCING THE DEVS ALREADY AND DO UR JOB CORRECTLY!!! (...sheesh.)");
				DisplayAddComponent<SpriteComponent>("Sprite", "~didn't ya add it already...~? ^u^", "");
				DisplayAddComponent<CircleComponent>("Circle", "auhmmm.. ya already added it, silly~!", "");
				DisplayAddComponent<Rigidbody2DComponent>("Rigidbody 2D", "..heehee whoops! Ya already added that, cutie! mmmhhh...", "WHAT DID I JUST SAY!? STOP FLIRTING WITH THE DEVS! THEY DON'T GET PAID ENOUGH TO DEAL WITH UR SHENANIGANS!");
				DisplayAddComponent<BoxCollider2DComponent>("Box Collider 2D", "~unnghh... didn't... ya do that already... oh! mmmhsjaasd...", "...DID I JUST HEAR SOME MOANS COMING FROM OVER THERE??? GET BACK ON THE JOB, YA WEIRDO!");
				DisplayAddComponent<BallCollider2DComponent>("Ball Collider 2D", "mmm..! ~Ya already have enough balls~! UwU!", "OK, PLEASE STOP!");
				ImGui::EndPopup();
			}
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("position", component.position);
			glm::vec3 rotation = glm::degrees(component.rotation);
			DrawVec3Control("rotation", rotation);
			component.rotation = glm::radians(rotation);
			DrawVec3Control("scale", component.scale);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			const char* projectionTypeStrs[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeStr = projectionTypeStrs[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Type", currentProjectionTypeStr))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeStr = projectionTypeStrs[i];
					if (ImGui::Selectable(projectionTypeStrs[i], isSelected))
					{
						currentProjectionTypeStr = projectionTypeStrs[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspSize = glm::degrees(camera.GetPerspectiveFov());
				if (ImGui::DragFloat("Fov", &perspSize)) camera.SetPerspectiveFov(glm::radians(perspSize));
				float perspNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near Clip", &perspNear)) camera.SetPerspectiveNearClip(perspNear);
				float perspFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far Clip", &perspFar)) camera.SetPerspectiveFarClip(perspFar);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize)) camera.SetOrthographicSize(orthoSize);
				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near Clip", &orthoNear)) camera.SetOrthographicNearClip(orthoNear);
				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far Clip", &orthoFar)) camera.SetOrthographicFarClip(orthoFar);
			}

			ImGui::Checkbox("Is Main Camera", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		});

		DrawComponent<SpriteComponent>("Sprite", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					component.Texture = Texture2D::Create(texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("UV Scale", &component.uvScale, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<CircleComponent>("Circle", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Softness", &component.Softness, 0.0001f, 0.0f, 2.0f);
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
		{
			const char* bodyTypeStrs[] = { "Passive", "Active", "Controlled" };
			const char* currentbodyTypeStr = bodyTypeStrs[(int)component.Type];
			if (ImGui::BeginCombo("Body Type", currentbodyTypeStr))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentbodyTypeStr = bodyTypeStrs[i];
					if (ImGui::Selectable(bodyTypeStrs[i], isSelected))
					{
						currentbodyTypeStr = bodyTypeStrs[i];
						component.Type = (Rigidbody2DComponent::BodyType)i;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
		});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.size));
			ImGui::DragFloat("Density", &component.Density, 0.01f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f);
		});

		DrawComponent<BallCollider2DComponent>("Ball Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
				ImGui::DragFloat("Radius", &component.radius, 0.1f);
				ImGui::DragFloat("Density", &component.Density, 0.01f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f);
			});
	}
}