project "Dualshock 4 Sample"
	targetname "SampleApp"
	language "C++"
	kind "ConsoleApp"
	dependson "Dualshock4 Static"
	files{
		project_root .. "SampleApp/Source/**.h",
		project_root .. "SampleApp/Source/**.cpp",
	}
	includedirs (project_root .. "Dualshock4/include/")

	filter { "platforms:Win64", "configurations:Debug"} 
		links (project_root .. "Dualshock4/Lib/Debug/Dualshock4Static.lib")

	filter { "platforms:Win64", "configurations:Release"} 
		links (project_root .. "Dualshock4/Lib/Dualshock4Static.lib")

	filter { "platforms:Win32", "configurations:Debug"} 
		links (project_root .. "Dualshock4/Lib32/Debug/Dualshock4Static.lib")

	filter { "platforms:Win32", "configurations:Release"} 
		links (project_root .. "Dualshock4/Lib32/Dualshock4Static.lib")

	filter { "platforms:Win64", "configurations:Debug"}
		targetdir (project_root .. "SampleApp/Bin/Debug/")	

	filter { "platforms:Win64", "configurations:Release"}
		targetdir (project_root .. "SampleApp/Bin/")

	filter { "platforms:Win32", "configurations:Debug"}
		targetdir (project_root .. "SampleApp/Bin32/Debug/")		

	filter { "platforms:Win32", "configurations:Release"}
		targetdir (project_root .. "SampleApp/Bin32/")	

