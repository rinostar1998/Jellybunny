#pragma once

#ifdef JB_PLATFORM_WINDOWS

extern Jellybunny::Application* Jellybunny::CreateApplication();
int main(int argc, char** argv)
{
	printf("Jellybunny Game Engine Init Started -- Successful\n");
	auto app = Jellybunny::CreateApplication();
	app->Run();
	delete app;
}

#endif