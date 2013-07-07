project "parser"
kind "StaticLib"
language "C"
files { "src/**.c", "include/**.h" }
includedirs { "include" }
targetdir "lib"

project "parser_test"
kind "StaticLib"
language "C"
files { "tests/**.c", "tests/**.h" }
includedirs { "include", "/opt/local/include" }
targetdir "lib"
