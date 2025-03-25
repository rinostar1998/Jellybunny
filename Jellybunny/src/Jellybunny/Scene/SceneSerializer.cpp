#include "jbpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4) return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Jellybunny
{
	std::string Rigidbody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Jellybunny::Rigidbody2DComponent::BodyType::Passive:	 return "PASS";
		case Jellybunny::Rigidbody2DComponent::BodyType::Active:	 return "ACT";
		case Jellybunny::Rigidbody2DComponent::BodyType::Controlled: return "CONT";
		}
		JB_CORE_ASS(false, "(I'M TOO TIRED TO BE ANGRY RIGHT NOW...) UNKNOWN SHADER TYPE.");
		return {};
	}

	Rigidbody2DComponent::BodyType Rigidbody2DBodyTypeFromString(const std::string& bodyType)
	{
		if (bodyType == "PASS") return Rigidbody2DComponent::BodyType::Passive;
		if (bodyType == "ACT")  return Rigidbody2DComponent::BodyType::Active;
		if (bodyType == "CONT") return Rigidbody2DComponent::BodyType::Controlled;
		return Rigidbody2DComponent::BodyType::Passive;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}


	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		JB_CORE_ASS(entity.HasComponent<IDComponent>());
		out << YAML::BeginMap;
		out << YAML::Key << "ENTY" << YAML::Value << entity.GetUUID();
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TAGCMP";
			out << YAML::BeginMap;
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key <<  "TAG" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TRNSCMP";
			out << YAML::BeginMap;
			auto& transformComp = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "POS" << YAML::Value << transformComp.position;
			out << YAML::Key << "ROT" << YAML::Value << transformComp.rotation;
			out << YAML::Key << "SCL" << YAML::Value << transformComp.scale;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CMRACMP";
			out << YAML::BeginMap;

			auto& cameraComp = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComp.Camera;
			out << YAML::Key << "CMRA" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "PROJTYP" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "FOV" << YAML::Value << camera.GetPerspectiveFov();
			out << YAML::Key << "PSPNR" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PSPFR" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OTHSZ" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OTHNR" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OTHFR" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "PRMY" << YAML::Value << cameraComp.Primary;
			out << YAML::Key << "FIXASRT" << YAML::Value << cameraComp.FixedAspectRatio;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SPRTCMP";
			out << YAML::BeginMap;
			auto& spriteComp = entity.GetComponent<SpriteComponent>();
			out << YAML::Key << "CLR" << YAML::Value << spriteComp.Color;
			if(spriteComp.Texture) out << YAML::Key << "TXUR" << YAML::Value << spriteComp.Texture->GetPath();
			out << YAML::Key << "UVSCL" << YAML::Value << spriteComp.uvScale;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<CircleComponent>())
		{
			out << YAML::Key << "CRCL";
			out << YAML::BeginMap;
			auto& circleComp = entity.GetComponent<CircleComponent>();
			out << YAML::Key << "CLR" << YAML::Value << circleComp.Color;
			out << YAML::Key << "THK" << YAML::Value << circleComp.Thickness;
			out << YAML::Key << "SFT" << YAML::Value << circleComp.Softness;
			out << YAML::EndMap;
		}
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "RGDBDY2D";
			out << YAML::BeginMap;
			auto& rb2dComp = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BDYTYP" << YAML::Value << Rigidbody2DBodyTypeToString(rb2dComp.Type);
			out << YAML::Key << "FXDROT" << YAML::Value << rb2dComp.FixedRotation;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BXCLD2D";
			out << YAML::BeginMap;
			auto& bx2dCmp = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "OFFSET" << YAML::Value << bx2dCmp.offset;
			out << YAML::Key << "SIZE" << YAML::Value << bx2dCmp.size;
			out << YAML::Key << "DENS" << YAML::Value << bx2dCmp.Density;
			out << YAML::Key << "FRCT" << YAML::Value << bx2dCmp.Friction;
			out << YAML::Key << "RST" << YAML::Value << bx2dCmp.Restitution;
			out << YAML::Key << "RTTH" << YAML::Value << bx2dCmp.RestitutionThreshold;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<BallCollider2DComponent>())
		{
			out << YAML::Key << "BLCLD2D";
			out << YAML::BeginMap;
			auto& bl2dCmp = entity.GetComponent<BallCollider2DComponent>();
			out << YAML::Key << "OFFSET" << YAML::Value << bl2dCmp.offset;
			out << YAML::Key << "RAD" << YAML::Value << bl2dCmp.radius;
			out << YAML::Key << "DENS" << YAML::Value << bl2dCmp.Density;
			out << YAML::Key << "FRCT" << YAML::Value << bl2dCmp.Friction;
			out << YAML::Key << "RST" << YAML::Value << bl2dCmp.Restitution;
			out << YAML::Key << "RTTH" << YAML::Value << bl2dCmp.RestitutionThreshold;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "SCNE" << YAML::Value << "UNTLTD";
		out << YAML::Key << "ENT_ARR" << YAML::Value << YAML::BeginSeq;
		for (const auto [entityID, ref] : m_Scene->m_Registry.storage<TransformComponent>().each())
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity) return;
			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filePath)
	{
		// Implement
		JB_CORE_ASS(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		JB_CORE_ASS(strStream.str().length(), "FUCK");
		YAML::Node data = YAML::Load(strStream.str());

		if (!data["SCNE"]) return false;
		std::string sceneName = data["SCNE"].as<std::string>();
		JB_CORE_TRC("DESERIALIZING THE FUCKER : '{0}'", sceneName);
		auto entites = data["ENT_ARR"];
		if (entites)
		{
			for (auto entity : entites)
			{
				uint32_t uuid = entity["ENTY"].as<uint64_t>();
				std::string name;
				auto tagComponent = entity["TAGCMP"];
				if (tagComponent) name = tagComponent["TAG"].as<std::string>();
				JB_CORE_TRC("DESERIALIZED THE ENTITY WITH ID : {0} / NAME : {1}", uuid, name);
				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);
				auto transformComponent = entity["TRNSCMP"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.position = transformComponent["POS"].as<glm::vec3>();
					tc.rotation = transformComponent["ROT"].as<glm::vec3>();
					tc.scale = transformComponent["SCL"].as<glm::vec3>();
				}
				auto cameraComponent = entity["CMRACMP"];
				if(cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& props = cameraComponent["CMRA"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)props["PROJTYP"].as<int>());
					cc.Camera.SetPerspectiveFov(props["FOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(props["PSPNR"].as<float>());
					cc.Camera.SetPerspectiveFarClip(props["PSPFR"].as<float>());
					cc.Camera.SetOrthographicSize(props["OTHSZ"].as<float>());
					cc.Camera.SetOrthographicNearClip(props["OTHNR"].as<float>());
					cc.Camera.SetOrthographicFarClip(props["OTHFR"].as<float>());
					cc.Primary = cameraComponent["PRMY"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FIXASRT"].as<bool>();
				}
				auto spriteComp = entity["SPRTCMP"];
				if (spriteComp)
				{
					auto& sc = deserializedEntity.AddComponent<SpriteComponent>();
					sc.Color = spriteComp["CLR"].as<glm::vec4>();
					if(spriteComp["TXUR"]) sc.Texture = Texture2D::Create(spriteComp["TXUR"].as<std::string>());
					if(spriteComp["UVSCL"]) sc.uvScale = spriteComp["UVSCL"].as<float>();
				}
				auto circleComp = entity["CRCL"];
				if (circleComp)
				{
					auto& cc = deserializedEntity.AddComponent<CircleComponent>();
					cc.Color = circleComp["CLR"].as<glm::vec4>();
					cc.Thickness = circleComp["THK"].as<float>();
					cc.Softness = circleComp["SFT"].as<float>();
				}
				auto rb2dComp = entity["RGDBDY2D"];
				if (rb2dComp)
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = Rigidbody2DBodyTypeFromString(rb2dComp["BDYTYP"].as<std::string>());
					rb2d.FixedRotation = rb2dComp["FXDROT"].as<bool>();
				}

				auto boxCol2dComp = entity["BXCLD2D"];
				if (boxCol2dComp)
				{
					auto& bxCol2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bxCol2d.offset = boxCol2dComp["OFFSET"].as<glm::vec2>();
					bxCol2d.size = boxCol2dComp["SIZE"].as<glm::vec2>();
					bxCol2d.Density = boxCol2dComp["DENS"].as<float>();
					bxCol2d.Friction = boxCol2dComp["FRCT"].as<float>();
					bxCol2d.Restitution = boxCol2dComp["RST"].as<float>();
					bxCol2d.RestitutionThreshold = boxCol2dComp["RTTH"].as<float>();
				}

				auto ballCol2dComp = entity["BLCLD2D"];
				if (ballCol2dComp)
				{
					auto& blCol2d = deserializedEntity.AddComponent<BallCollider2DComponent>();
					blCol2d.offset = ballCol2dComp["OFFSET"].as<glm::vec2>();
					blCol2d.radius = ballCol2dComp["RAD"].as<float>();
					blCol2d.Density = ballCol2dComp["DENS"].as<float>();
					blCol2d.Friction = ballCol2dComp["FRCT"].as<float>();
					blCol2d.Restitution = ballCol2dComp["RST"].as<float>();
					blCol2d.RestitutionThreshold = ballCol2dComp["RTTH"].as<float>();
				}
			}
		}
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
	{
		// Implement
		JB_CORE_ASS(false);
		return false;
	}
}