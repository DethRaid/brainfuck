workspace "brainfuck-tools"
    configurations { "Debug", "Release" }
    platforms { "Win64", "Linux" }

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x64"

    filter { "platforms:Linux" }
        system "Linux"
        architecture "x64"

-- TODO: Lean how subprojects work

project "libbf"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    location "build/libbf"
    targetdir "bin/%{cfg.buildcfg}"
    sysincludedirs { "libs/bf/include" }

    files { "libs/bf/**.cpp", "libs/bf/**.hpp" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "bf"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    location "build/bf"
    targetdir "bin/%{cfg.buildcfg}"

    links "libbf"
    sysincludedirs { "libs/bf/include" }    -- TODO: Get these from the linked libs

    files { "apps/bf/**.cpp", "apps/bf/**.hpp" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
