#pragma once

#include <string>

namespace Jellybunny
{
	class FileDialogs
	{
	public:
		// Returns empty string if cancelled
		static std::string LoadFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}