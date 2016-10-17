-----------------------------------------
--	Globals
-----------------------------------------
project_root = "../../Projects/"
extern_root = "../../Extern/"

-----------------------------------------
--	Solution configuration
-----------------------------------------
solution "Dualshock4"
	configurations {"Debug", "Release"}
	platforms { "Win32", "Win64"}
	

	-- 	ide filters
	filter "action:vs*"
		defines  { 
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_SECURE_NO_WARNINGS",
			"_WINDOWS"
		}

	configuration "vs2015"
		location "vs2015"

	-- platform filters
	filter { "platforms:Win32" }
	    system "Windows"
	    architecture "x32"
	    defines  { "WIN32"}
	    flags "EnableSSE2"

	filter { "platforms:Win64" }
	    system "Windows"
	    architecture "x64"
	   	defines "WIN64"

	--	Configuration filters
	filter "configurations:Debug"
		defines {"_DEBUG","DEBUG_BUILD"}
		flags {"Symbols"}
		optimize "Off"
		editandcontinue "On"

	filter "configurations:Release"
		defines 	{"NDEBUG"}
		optimize 	"Full"



-----------------------------------------
--				Projects
-----------------------------------------
require "Projects/Dualshock 4 Static"
require "Projects/Dualshock 4 DLL"
require "Projects/Dualshock 4 Sample"
require "Projects/Dualshock 4 Xinput"

--
solution "Dualshock4"
	startproject "Dualshock 4 Utility"