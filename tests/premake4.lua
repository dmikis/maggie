project "tests"
kind "ConsoleApp"
language "C"
files { "**.c" }

includedirs {
    "/opt/local/include",
    "../parser/tests"
}

libdirs { "/opt/local/lib" }

links {
    "parser_test",
    "check"
}

