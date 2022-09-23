-- premake5.lua

workspace "alica"
   configurations { "Debug", "Release" }

project "alica_solver_interface"
   kind "StaticLib"
   language "C++"
   cppdialect "C++14"
   targetdir "bin/%{cfg.buildcfg}"

   libdirs { 
      os.findlib("uuid")
     }
   
   includedirs {
      "include", 
   }

   files {
      "include/**.h",
      "src/**.cpp", 
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
   


