project "strbuff"
kind "StaticLib"
language "C"
files { "src/**.c", "include/**.h" }
includedirs { "./include" }
targetdir "lib"
