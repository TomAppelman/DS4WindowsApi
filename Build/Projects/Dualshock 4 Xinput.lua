project "Dualshock 4 Xinput"
	language "C"
	targetname "xinput1_3"
	kind "SharedLib"
	dependson "Dualshock4 Static"
	files{
		project_root .. "XInputWrapper/Source/**.def",
		project_root .. "XInputWrapper/Source/**.c",
		project_root .. "XInputWrapper/Source/**.h",
	 	project_root .. "XInputWrapper/Source/**.cpp"
	}
	includedirs (project_root .. "Dualshock4/include/")

	postbuildcommands { 
		--"copy /Y \"..\\..\\XInputWrapper\\Lib\\Debug\\xinput1_3.dll\" \"..\\..\\XInputWrapper\\Lib\\Debug\\xinput1_4.dll\"",
		--"copy /Y xinput1_3.dll xinput1_4.dll"
		"copy /Y \"$(TargetDir)$(TargetName).dll\" \"$(TargetDir)xinput1_1.dll\"",
		"copy /Y \"$(TargetDir)$(TargetName).dll\" \"$(TargetDir)xinput1_2.dll\"",
		"copy /Y \"$(TargetDir)$(TargetName).dll\" \"$(TargetDir)xinput1_4.dll\"",
		"copy /Y \"$(TargetDir)$(TargetName).dll\" \"$(TargetDir)xinput9_1_0.dll\"",
		"del /Q \"$(TargetDir)$(TargetName).exp\"",
		"del /Q \"$(TargetDir)$(TargetName).ilk\"",
		"del /Q \"$(TargetDir)$(TargetName).pdb\"",
		"del /Q \"$(TargetDir)$(TargetName).lib\""
	 }

--
	filter { "platforms:Win64", "configurations:Debug"} 
		links (project_root .. "Dualshock4/Lib/Debug/Dualshock4Static.lib")

	filter { "platforms:Win64", "configurations:Release"} 
		links (project_root .. "Dualshock4/Lib/Dualshock4Static.lib")

	filter { "platforms:Win32", "configurations:Debug"} 
		links (project_root .. "Dualshock4/Lib32/Debug/Dualshock4Static.lib")

	filter { "platforms:Win32", "configurations:Release"} 
		links (project_root .. "Dualshock4/Lib32/Dualshock4Static.lib")

--
	filter { "platforms:Win64", "configurations:Debug"}
		targetdir (project_root .. "XInputWrapper/Lib/Debug/")

	filter { "platforms:Win64", "configurations:Release"}
		targetdir (project_root .. "XInputWrapper/Lib/")

	filter { "platforms:Win32", "configurations:Debug"}
		targetdir (project_root .. "XInputWrapper/Lib32/Debug/")

	filter { "platforms:Win32", "configurations:Release"}
		targetdir (project_root .. "XInputWrapper/Lib32/")