workspace "brainfuck-tools"
    configurations { "Debug", "Release" }

project "bf"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    -- TODO: Lean how subprojects work
    files { "apps/bf/**.cpp", "apps/bf/**.hpp" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
