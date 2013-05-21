project "dg"
kind "StaticLib"
language "C"
files { "src/**.c", "include/**.h" }
includedirs { "./include", "../parser/include"}
libdirs { "../parser/lib/" }
links { "parser" }
targetdir "lib"

configuration "linux or macosx"
    buildoptions { pkgConfig("cflags", { "gsl" }) }
    linkoptions { pkgConfig("libs", { "gsl" }) }
