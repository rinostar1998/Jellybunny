#include <Jellybunny.h>

class ExampleLayer : public Jellybunny::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}
	void OnUpdate() override
	{
		if (Jellybunny::Input::IsKeyPressed(JB_KEY_TAB))
			JB_IFO("TAB");
	}
	void OnEvent(Jellybunny::Event& event) override
	{

	}
};

class Sandbox : public Jellybunny::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Jellybunny::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};


Jellybunny::Application* Jellybunny::CreateApplication()
{
	return new Sandbox();
}