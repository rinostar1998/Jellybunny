#pragma once
#include "Jellybunny/Core/Core.h"
#include "Jellybunny/Core/Application.h"

#ifdef JB_PLATFORM_WINDOWS

extern Jellybunny::Application* Jellybunny::CreateApplication(ApplicationCommandLineArgs args);
int main(int argc, char** argv)
{
	Jellybunny::Log::Init();

	JB_PROFILE_BEGIN_SESSION("Startup", "JellybunnyProfile-Startup.json");
	auto app = Jellybunny::CreateApplication({ argc, argv });
	JB_PROFILE_END_SESSION();

	JB_PROFILE_BEGIN_SESSION("Runtime", "JellybunnyProfile-Runtime.json");
	app->Run();
	JB_PROFILE_END_SESSION();

	JB_PROFILE_BEGIN_SESSION("Shutdown", "JellybunnyProfile-Shutdown.json");
	delete app;
	JB_PROFILE_END_SESSION();
}

#endif