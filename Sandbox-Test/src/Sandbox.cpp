#include <Jellybunny.h>

class Sandbox : public Jellybunny::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};


Jellybunny::Application* Jellybunny::CreateApplication()
{
	return new Sandbox();
}