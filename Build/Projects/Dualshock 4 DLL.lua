project "Dualshock 4 DLL"
	targetname "Dualshock4"
	language "C"
	kind "SharedLib"
	links {
		"Setupapi.lib",
		"Hid.lib",
	}
	includedirs ( project_root .. "Dualshock4/Include/" )
	files{
		project_root .. "Dualshock4/Source/**.def",
		project_root .. "Dualshock4/Source/**.h", 
		project_root .. "Dualshock4/Source/**.c",
		project_root .. "Dualshock4/Include/**.h", 
		project_root .. "Dualshock4/Include/**.hpp",
	}

	filter { "platforms:Win64", "configurations:Debug"}
		targetdir (project_root .. "Dualshock4/Lib/Debug/")	

	filter { "platforms:Win64", "configurations:Release"}
		targetdir (project_root .. "Dualshock4/Lib/")

	filter { "platforms:Win32", "configurations:Debug"}
		targetdir (project_root .. "Dualshock4/Lib32/Debug/")		

	filter { "platforms:Win32", "configurations:Release"}
		targetdir (project_root .. "Dualshock4/Lib32/")	
