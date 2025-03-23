#pragma once

#include "SceneCamera.h"
#include "Jellybunny/Core/UUID.h"
#include "Jellybunny/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Jellybunny
{
	struct IDComponent
	{
		UUID ID;
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale    = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position) : position(position) {}

		glm::mat4 GetTransform() const 
		{ 
			static glm::mat4 trans = glm::mat4(1.0f);
			return glm::translate(trans, position) * (rotation.x != 0 || rotation.y != 0 || rotation.z != 0 ? glm::toMat4(glm::quat(rotation)) * glm::scale(trans, {scale.x, scale.y, 1.0f}) : glm::scale(trans, {scale.x, scale.y, 1.0f})); // hehe fuck you! // why the hell would u do this
		}
	};

	struct SpriteComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float uvScale = 1;

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& color) : Color(color) {}
	};

	struct CircleComponent
	{
		glm::vec4 Color{ 1.0f, 0.0f, 0.0f, 1.0f };
		float Radius = 0.5;
		float Thickness = 1;
		float Softness = 0.005f;

		CircleComponent() = default;
		CircleComponent(const CircleComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	// Forward Decl
	class ScriptableEntity;

	struct LocalModuleComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(LocalModuleComponent*);

		template<typename T>
		void Load()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](LocalModuleComponent* lmc) { delete lmc->Instance; lmc->Instance = nullptr; };
		}
	};

	// Physics
	struct Rigidbody2DComponent
	{
		enum class BodyType
		{
			Passive = 0, Active, Controlled
		};
		BodyType Type = BodyType::Passive;
		bool FixedRotation = false;

		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 offset = { 0.0f, 0.0f };
		glm::vec2 size = { 0.5f, 0.5f };

		float Density = 100.0f;
		float Friction = 0.8f;
		float Restitution = 0.05f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct BallCollider2DComponent
	{
		glm::vec2 offset = { 0.0f, 0.0f };
		float radius = 0.5f;

		float Density = 100.0f;
		float Friction = 0.8f;
		float Restitution = 0.05f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		BallCollider2DComponent() = default;
		BallCollider2DComponent(const BallCollider2DComponent&) = default;
	};

	template<typename ...Component>
	struct ComponentGroup
	{

	};

	using AllComponents = ComponentGroup
	<
		TransformComponent, 
		SpriteComponent, 
		CameraComponent, 
		LocalModuleComponent, 
		Rigidbody2DComponent, 
		BoxCollider2DComponent, 
		CircleComponent, 
		BallCollider2DComponent
	>;
}