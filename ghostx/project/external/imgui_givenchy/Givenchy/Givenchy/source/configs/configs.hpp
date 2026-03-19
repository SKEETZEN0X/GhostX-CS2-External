#pragma once

#include <ShlObj.h>
#include <string>
#include <filesystem>

namespace Engine
{

	static std::string appdata_path()
	{
		PWSTR path = nullptr;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path)))
		{
			std::wstring wstrPath(path);
			CoTaskMemFree(path);
			return std::string(wstrPath.begin(), wstrPath.end());
		}
		return "";
	}
	
	class configs {
	public:
		static void save(const char* name);
		static void load(const char* name);
	};

}