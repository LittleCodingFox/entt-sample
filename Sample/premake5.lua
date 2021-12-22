include("conanbuildinfo.premake.lua")

workspace "Sample"
	conan_basic_setup()

	configurations { "Debug", "Release" }
	
	project "Sample"
		kind "ConsoleApp"
		language "C++"

		files {
			"*.hpp",
			"*.h",
			"*.cpp"
		}
		
		linkoptions { conan_exelinkflags }
		
		if os.target() == "windows" then
			links { "opengl32.lib" }
			buildoptions { "/std:c++17" }
		end

		filter "configurations:Debug"
			defines({ "DEBUG" })

			symbols "On"
 
		filter "configurations:Release"
			defines({ "NDEBUG" })

			optimize "On"
