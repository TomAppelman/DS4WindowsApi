project "Dualshock 4 Static"
	targetname "Dualshock4Static"
	language "C"
	kind "StaticLib"
	links {
		"Setupapi.lib",
		"Hid.lib",
	}
	defines "DUALSHOCK4_STATIC_LIB"
	includedirs (project_root .. "Dualshock4/Include/")
	files{
		project_root .. "Dualshock4/Source/**.h",
		project_root .. "Dualshock4/Source/**.c",
		project_root .. "Dualshock4/Include/**.h",
		project_root .. "Dualshock4/Include/**.hpp"
	}
	filter { "platforms:Win64", "configurations:Debug"}
		targetdir (project_root .. "Dualshock4/Lib/Debug/")	

	filter { "platforms:Win64", "configurations:Release"}
		targetdir (project_root .. "Dualshock4/Lib/")

	filter { "platforms:Win32", "configurations:Debug"}
		targetdir (project_root .. "Dualshock4/Lib32/Debug/")		

	filter { "platforms:Win32", "configurations:Release"}
		targetdir (project_root .. "Dualshock4/Lib32/")


		