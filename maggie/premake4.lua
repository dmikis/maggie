dofile "../tools/premake4/pkgconfig.lua"

project "maggie"
kind "ConsoleApp"
language "C"
files { "src/**.c", "include/**.h" }
includedirs {
    "./include",
    "../parser/include",
    "../mol2/include",
    "../bondinfo/include",
    "../structdata/include",
    "../dg/include"
}

links {
    "logger",
    "parser",
    "mol2",
    "bondinfo",
    "dg"
}

targetdir "bin"

configuration "linux or macosx"
    buildoptions { pkgConfig("cflags", { "glib-2.0", "sqlite3", "gsl" }) }
    linkoptions { pkgConfig("libs", { "glib-2.0", "sqlite3", "gsl" }) }
