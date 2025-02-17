#include <Jellybunny.h>
#include <Jellybunny/Core/EntryPoint.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public Jellybunny::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f), m_TriRot(0.0f)
	{
		m_VertexArray = Jellybunny::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
		};

		m_VertexBuffer = Jellybunny::VertexBuffer::Create(vertices, sizeof(vertices));

		Jellybunny::BufferLayout layout =
		{
			{ Jellybunny::ShaderDataType::Float3, "a_Position" },
			{ Jellybunny::ShaderDataType::Float4, "a_Color" }
		};

		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		m_IndexBuffer = Jellybunny::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_VertexArray2 = Jellybunny::VertexArray::Create();

		float vertices2[4 * 5] = {
			-0.75f, -0.75f, 0.0f, 0.0f, 0.0f,
			 0.75f, -0.75f, 0.0f, 1.0f, 0.0f,
		    -0.75f,  0.75f, 0.0f, 0.0f, 1.0f,
			 0.75f,  0.75f, 0.0f, 1.0f, 1.0f
		};

		m_VertexBuffer2 = Jellybunny::VertexBuffer::Create(vertices2, sizeof(vertices2));

		Jellybunny::BufferLayout layout2 =
		{
			{ Jellybunny::ShaderDataType::Float3, "a_Position" },
			{ Jellybunny::ShaderDataType::Float2, "a_TexCoord" }
		};

		m_VertexBuffer2->SetLayout(layout2);
		m_VertexArray2->AddVertexBuffer(m_VertexBuffer2);

		unsigned int indices2[6] = { 0, 1, 2, 1, 3, 2 };
		m_IndexBuffer2 = Jellybunny::IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t));
		m_VertexArray2->SetIndexBuffer(m_IndexBuffer2);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}

		)";

		m_Shader = Jellybunny::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";

		std::string flatColorFragSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}

		)";

		m_flatColorShader = Jellybunny::Shader::Create("FlatColor", flatColorVertexSrc, flatColorFragSrc);

		auto texShader = m_ShaderLib.Load("assets/shaders/Texture.glsl");
		m_Texture = Jellybunny::Texture2D::Create("assets/textures/FrownExtraMad.png");

		std::dynamic_pointer_cast<Jellybunny::OpenGLShader>(texShader)->Bind();
		std::dynamic_pointer_cast<Jellybunny::OpenGLShader>(texShader)->UploadUniformInt("u_Texture", 0);
	}
	void OnUpdate(Jellybunny::Timestep time) override
	{
		// Update
		m_CameraController.OnUpdate(time);

		// Render
		auto [x, y] = Jellybunny::Input::GetMousePosition();

		m_TriRot.x = y;
		m_TriRot.z = x;
		
		Jellybunny::RenderCommand::SetClearColor({ 0.1f, 0, 0, 1 });
		Jellybunny::RenderCommand::Clear();

		Jellybunny::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::vec4 redColor(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 pissColor(1.0f, 1.0f, 0.0f, 0.0f);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));

		std::dynamic_pointer_cast<Jellybunny::OpenGLShader>(m_flatColorShader)->Bind();
		std::dynamic_pointer_cast<Jellybunny::OpenGLShader>(m_flatColorShader)->UploadUniformFloat3("u_Color", m_Color2);

		for (int y = 0; y < 25; y++)
		{
			for (int x = 0; x < 25; x++)
			{
				glm::vec3 pos(x * 0.1f, y * 0.1f, 0);
				glm::mat4 transform =
					glm::translate(glm::mat4(1.0f), pos) * 
					glm::rotate(glm::mat4(1.0f), glm::radians((float)x), { 1.0f, 0.0f, 0.0f }) *
					glm::rotate(glm::mat4(1.0f), glm::radians((float)-y), { 0.0f, 1.0f, 0.0f }) *
					glm::rotate(glm::mat4(1.0f), glm::radians((float)x + y), { 0.0f, 0.0f, 1.0f }) * scale;
				Jellybunny::Renderer::Render(m_VertexArray2, m_flatColorShader, transform);
			}
		}
		auto texShader = m_ShaderLib.Get("Texture");

		Jellybunny::Renderer::Render(m_VertexArray2, texShader, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));
		m_Texture->Bind();
		//Tri: Jellybunny::Renderer::Render(m_VertexArray, texShader);


		Jellybunny::Renderer::EndScene();
	}
	void OnEvent(Jellybunny::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Peepee Color", glm::value_ptr(m_Color2));
		ImGui::End();
	}

private:
	Jellybunny::ShaderLibrary m_ShaderLib;
	Jellybunny::Ref<Jellybunny::Shader> m_Shader;
	Jellybunny::Ref<Jellybunny::Shader> m_flatColorShader;
	Jellybunny::Ref<Jellybunny::VertexBuffer> m_VertexBuffer;
	Jellybunny::Ref<Jellybunny::IndexBuffer> m_IndexBuffer;
	Jellybunny::Ref<Jellybunny::VertexArray> m_VertexArray;
	Jellybunny::Ref<Jellybunny::VertexBuffer> m_VertexBuffer2;
	Jellybunny::Ref<Jellybunny::IndexBuffer> m_IndexBuffer2;
	Jellybunny::Ref<Jellybunny::VertexArray> m_VertexArray2;

	Jellybunny::Ref<Jellybunny::Texture2D> m_Texture;

	Jellybunny::OrthographicCameraController m_CameraController;

	glm::vec3 m_TriRot;
	glm::vec3 m_Color2{1.0f, 1.0f, 0.0f};
};

class Sandbox : public Jellybunny::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}
};


Jellybunny::Application* Jellybunny::CreateApplication()
{
	return new Sandbox();
}