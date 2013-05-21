dofile "../tools/premake4/pkgconfig.lua"

project "mol2"
kind "StaticLib"
language "C"
files { "src/**.c", "include/**.h" }
includedirs { "./include", "../parser/include"}
targetdir "lib"

configuration "linux or macosx"
    buildoptions { pkgConfig("cflags", { "gsl" }) }
    linkoptions { pkgConfig("libs", { "gsl" }) }
