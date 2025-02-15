#pragma once

#ifdef JB_PLATFORM_WINDOWS

extern Jellybunny::Application* Jellybunny::CreateApplication();
int main(int argc, char** argv)
{
	Jellybunny::Log::Init();
	JB_CORE_WAR("Init Log");
	int a = 10;
	JB_IFO("Init Log 2 Var={0}", a);


	printf("Jellybunny Game Engine Init Started -- Successful\n");
	auto app = Jellybunny::CreateApplication();
	app->Run();
	delete app;
}

#endif