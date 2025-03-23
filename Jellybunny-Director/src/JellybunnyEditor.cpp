#include <Jellybunny.h>
#include <Jellybunny/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Jellybunny
{
	class JellybunnyEditor : public Application
	{
	public:
		JellybunnyEditor(Jellybunny::ApplicationSpecification& specification) : Application(specification)
		{
			PushLayer(new EditorLayer());
		}

		~JellybunnyEditor()
		{
		}
	};


	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.name = "Sandbox App Test";
		spec.workingDirectory = "";
		spec.commandLineArgs = args;
		return new JellybunnyEditor(spec);
	}
}