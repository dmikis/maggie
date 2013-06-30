project "bondinfo"
kind "StaticLib"
language "C"
files { "src/**.c", "include/**.h" }
includedirs { "include", "../logger/include", "/opt/local/include" }
targetdir "lib"
