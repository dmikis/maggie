solution "maggie"

    configurations { "Debug", "Release" }

    flags { "ExtraWarnings" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

    include "utils/strbuf"
    include "logger"
    include "parser"
    include "mol2"
    include "bondinfo"
    include "dg"
    include "maggie"
