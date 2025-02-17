#include "jbpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Jellybunny
{

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle)
	{
		JB_CORE_ASS(windowHandle, "YA FORGOT TO STICK A WINDOW HANDLE UP UR ASS! (it's null.)");
	}

	void OpenGLContext::Init()
	{
		JB_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		JB_CORE_ASS(status, "FAILED TO INIT GLAD!");

		JB_CORE_IFO("**************** OPENGL RENDERER REPORT ****************");
		JB_CORE_IFO("VENDOR  NAME **** {0}", (const char*)glGetString(GL_VENDOR));
		JB_CORE_IFO("VERSION NAME **** {0}", (const char*)glGetString(GL_VERSION));
		JB_CORE_IFO("RENDER  NAME **** {0}", (const char*)glGetString(GL_RENDERER));
		JB_CORE_IFO("********************** END REPORT **********************");
	}

	void OpenGLContext::SwapBuffers()
	{
		JB_PROFILE_FUNCTION();
		glfwSwapBuffers(m_windowHandle);
	}

}