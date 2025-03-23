#include <Jellybunny.h>
#include <Jellybunny/Core/EntryPoint.h>

#include "Sandbox2D.h"

class Sandbox : public Jellybunny::Application
{
public:
	Sandbox(Jellybunny::ApplicationSpecification& specification) : Application(specification)
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}
};


Jellybunny::Application* Jellybunny::CreateApplication(Jellybunny::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.name = "Sandbox App Test";
	spec.workingDirectory = "../Jellybunny-Director";
	spec.commandLineArgs = args;
	return new Sandbox(spec);
}