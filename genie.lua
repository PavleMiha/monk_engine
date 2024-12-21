-- Just change solution/project name and project GUID

local PROJECT_DIR          = (path.getabsolute(".") .. "/")
local PROJECT_BUILD_DIR    = path.join(PROJECT_DIR, "build/")
local PROJECT_PROJECTS_DIR = path.join(PROJECT_DIR, "build/")
local PROJECT_RUNTIME_DIR  = path.join(PROJECT_BUILD_DIR, "bin/")
local DEPENDENCY_ROOT_DIR  = PROJECT_DIR

BGFX_DIR      = path.join(DEPENDENCY_ROOT_DIR, "3rdparty/bgfx")
BX_DIR        = path.join(DEPENDENCY_ROOT_DIR, "3rdparty/bx")
BIMG_DIR      = path.join(DEPENDENCY_ROOT_DIR, "3rdparty/bimg")
GLFW_DIR 	  = path.join(DEPENDENCY_ROOT_DIR, "3rdparty/glfw")
GLM_DIR		  = path.join(DEPENDENCY_ROOT_DIR, "3rdparty/glm")
ASSIMP_DIR    = path.join(DEPENDENCY_ROOT_DIR, "3rdparty/assimp")
IMGUI_DIR	  = path.join(BGFX_DIR, "3rdparty/dear-imgui")

-- Required for bgfx and example-common
function copyLib()
end

-- Options
newoption {
   trigger = "usd-importer",
   description = "Enable USD file import",
   value = "off",
   default = "off"
}

newoption {
   trigger = "m3d-importer",
   description = "Enable M3D file import",
   value = "off",
   default = "off"
}

solution "app"
	language				"C++"
	configurations			{ "Debug", "Release" }
	platforms				{ "x64" }

	defines {
		"ENTRY_CONFIG_IMPLEMENT_DEFAULT_ALLOCATOR=1",
		"BX_CONFIG_ENABLE_MSVC_LEVEL4_WARNINGS=1",
		"ENTRY_CONFIG_USE_GLFW=1",
		"ENTRY_CONFIG_IMPLEMENT_MAIN=1"
	}	

	configuration "Release"
		defines "BX_CONFIG_DEBUG=0"

	configuration "Debug"
		defines "BX_CONFIG_DEBUG=1"

	configuration "windows"
		defines {
			"BGFX_CONFIG_RENDERER_DIRECT3D11=1",
		}

	configuration {}
	dofile (path.join(BX_DIR, "scripts/toolchain.lua"))

	if not toolchain(PROJECT_PROJECTS_DIR, DEPENDENCY_ROOT_DIR) then
		return -- no action specified
	end

	project "glfw"
		kind "StaticLib"
		language "C"
		files
		{
			path.join(GLFW_DIR, "include/GLFW/*.h"),
			path.join(GLFW_DIR, "src/context.c"),
			path.join(GLFW_DIR, "src/egl_context.*"),
			path.join(GLFW_DIR, "src/init.c"),
			path.join(GLFW_DIR, "src/input.c"),
			path.join(GLFW_DIR, "src/internal.h"),
			path.join(GLFW_DIR, "src/monitor.c"),
			path.join(GLFW_DIR, "src/null*.*"),
			path.join(GLFW_DIR, "src/osmesa_context.*"),
			path.join(GLFW_DIR, "src/platform.c"),
			path.join(GLFW_DIR, "src/vulkan.c"),
			path.join(GLFW_DIR, "src/window.c"),
		}
		includedirs { path.join(GLFW_DIR, "include") }
		configuration "windows"
			defines "_GLFW_WIN32"
			files
			{
				path.join(GLFW_DIR, "src/win32_*.*"),
				path.join(GLFW_DIR, "src/wgl_context.*")
			}
		configuration "linux"
			defines "_GLFW_X11"
			files
			{
				path.join(GLFW_DIR, "src/glx_context.*"),
				path.join(GLFW_DIR, "src/linux*.*"),
				path.join(GLFW_DIR, "src/posix*.*"),
				path.join(GLFW_DIR, "src/x11*.*"),
				path.join(GLFW_DIR, "src/xkb*.*")
			}
		configuration "macosx"
			defines "_GLFW_COCOA"
			files
			{
				path.join(GLFW_DIR, "src/cocoa_*.*"),
				path.join(GLFW_DIR, "src/posix_thread.h"),
				path.join(GLFW_DIR, "src/nsgl_context.h"),
				path.join(GLFW_DIR, "src/egl_context.h"),
				path.join(GLFW_DIR, "src/osmesa_context.h"),

				path.join(GLFW_DIR, "src/posix_thread.c"),
				path.join(GLFW_DIR, "src/nsgl_context.m"),
				path.join(GLFW_DIR, "src/egl_context.c"),
				path.join(GLFW_DIR, "src/nsgl_context.m"),
				path.join(GLFW_DIR, "src/osmesa_context.c"),                       
			}

		configuration "vs*"
			defines "_CRT_SECURE_NO_WARNINGS"
			buildoptions {
				"/wd 4100",--unused parameter
				"/wd 4244"--conversion
			}

	
	--location				(path.join(PROJECT_PROJECTS_DIR, _ACTION))
	--objdir					(path.join(PROJECT_BUILD_DIR, _ACTION))

	dofile (path.join(BX_DIR,	"scripts/bx.lua"))
	dofile (path.join(BIMG_DIR, "scripts/bimg.lua"))
	dofile (path.join(BIMG_DIR, "scripts/bimg_decode.lua"))
	dofile (path.join(BGFX_DIR, "scripts/bgfx.lua"))

	bgfxProject("", "StaticLib", {})

	project "entry"
		kind "StaticLib"
		language "C++"
		files {
			path.join(BGFX_DIR, "examples/common/entry/*.h"),
			path.join(BGFX_DIR, "examples/common/entry/*.cpp"),
			path.join(BGFX_DIR, "examples/common/bgfx_utils.cpp"),
			path.join(BGFX_DIR, "3rdparty/meshoptimizer/src/*.cpp"),
			path.join(BGFX_DIR, "3rdparty/meshoptimizer/src/*.h"),
		}
		includedirs {
			path.join(GLFW_DIR, "include"),
			path.join(BGFX_DIR, "3rdparty"),
			path.join(BGFX_DIR, "include"),
			path.join(BX_DIR, 	"include"),
			path.join(BIMG_DIR, "include"),
			IMGUI_DIR
		}

	project "glm"
		kind "StaticLib"
		language "C++"
		files {
			path.join(GLM_DIR, "**.hpp")
		}

	project "imgui"
		kind "StaticLib"
		language "C++"
		files {
			path.join(IMGUI_DIR, "*.cpp"),
			path.join(IMGUI_DIR, "*.h"),
			path.join(IMGUI_DIR, "imgui.cpp"),
			path.join(IMGUI_DIR, "imgui_draw.cpp"),
			path.join(IMGUI_DIR, "imgui_internal.h"),
			path.join(IMGUI_DIR, "imgui_widgets.cpp"),
			path.join(IMGUI_DIR, "imgui_tables.cpp"),
			path.join(IMGUI_DIR, "imstb_rectpack.h"),
			path.join(IMGUI_DIR, "imstb_textedit.h"),
			path.join(IMGUI_DIR, "imstb_truetype.h"),
			path.join(IMGUI_DIR, "imgui_demo.cpp"),
			path.join(BGFX_DIR, "examples/common/imgui/imgui.cpp")
		}

		includedirs {
			path.join(BGFX_DIR, "3rdparty"),
			path.join(BGFX_DIR, "include"),
			path.join(BX_DIR, 	"include"),
			path.join(BIMG_DIR, "include"),
		}

		defines {
			"IMGUI_DISABLE_DEFAULT_ALLOCATORS"
		}



	project "assimp"
		kind "StaticLib"
		language "C++"

		--targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		--objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		defines {
			-- "SWIG",
			"ASSIMP_BUILD_NO_OWN_ZLIB",

			"ASSIMP_BUILD_NO_X_IMPORTER",
			"ASSIMP_BUILD_NO_3DS_IMPORTER",
			"ASSIMP_BUILD_NO_MD3_IMPORTER",
			"ASSIMP_BUILD_NO_MDL_IMPORTER",
			"ASSIMP_BUILD_NO_MD2_IMPORTER",
			-- "ASSIMP_BUILD_NO_PLY_IMPORTER",
			"ASSIMP_BUILD_NO_ASE_IMPORTER",
			-- "ASSIMP_BUILD_NO_OBJ_IMPORTER",
			"ASSIMP_BUILD_NO_AMF_IMPORTER",
			"ASSIMP_BUILD_NO_HMP_IMPORTER",
			"ASSIMP_BUILD_NO_SMD_IMPORTER",
			"ASSIMP_BUILD_NO_MDC_IMPORTER",
			"ASSIMP_BUILD_NO_MD5_IMPORTER",
			"ASSIMP_BUILD_NO_STL_IMPORTER",
			"ASSIMP_BUILD_NO_LWO_IMPORTER",
			"ASSIMP_BUILD_NO_DXF_IMPORTER",
			"ASSIMP_BUILD_NO_NFF_IMPORTER",
			"ASSIMP_BUILD_NO_RAW_IMPORTER",
			"ASSIMP_BUILD_NO_OFF_IMPORTER",
			"ASSIMP_BUILD_NO_AC_IMPORTER",
			"ASSIMP_BUILD_NO_BVH_IMPORTER",
			"ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
			"ASSIMP_BUILD_NO_IRR_IMPORTER",
			"ASSIMP_BUILD_NO_Q3D_IMPORTER",
			"ASSIMP_BUILD_NO_B3D_IMPORTER",
			-- "ASSIMP_BUILD_NO_COLLADA_IMPORTER",
			"ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
			"ASSIMP_BUILD_NO_CSM_IMPORTER",
			"ASSIMP_BUILD_NO_3D_IMPORTER",
			"ASSIMP_BUILD_NO_LWS_IMPORTER",
			"ASSIMP_BUILD_NO_OGRE_IMPORTER",
			"ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
			"ASSIMP_BUILD_NO_MS3D_IMPORTER",
			"ASSIMP_BUILD_NO_COB_IMPORTER",
			"ASSIMP_BUILD_NO_BLEND_IMPORTER",
			"ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
			"ASSIMP_BUILD_NO_NDO_IMPORTER",
			"ASSIMP_BUILD_NO_IFC_IMPORTER",
			"ASSIMP_BUILD_NO_XGL_IMPORTER",
			"ASSIMP_BUILD_NO_FBX_IMPORTER",
			"ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
			-- "ASSIMP_BUILD_NO_GLTF_IMPORTER",
			"ASSIMP_BUILD_NO_C4D_IMPORTER",
			"ASSIMP_BUILD_NO_3MF_IMPORTER",
			"ASSIMP_BUILD_NO_X3D_IMPORTER",
			"ASSIMP_BUILD_NO_MMD_IMPORTER",
			
			"ASSIMP_BUILD_NO_STEP_EXPORTER",
			"ASSIMP_BUILD_NO_SIB_IMPORTER",

			-- "ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS",
			-- "ASSIMP_BUILD_NO_FLIPUVS_PROCESS",
			-- "ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS",
			-- "ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS",
			"ASSIMP_BUILD_NO_JOINVERTICES_PROCESS",
			-- "ASSIMP_BUILD_NO_TRIANGULATE_PROCESS",
			"ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS",
			-- "ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS",
			"ASSIMP_BUILD_NO_REMOVEVC_PROCESS",
			"ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS",
			"ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS",
			"ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS",
			-- "ASSIMP_BUILD_NO_VALIDATEDS_PROCESS",
			"ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS",
			"ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS",
			"ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS",
			"ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS",
			"ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS",
			"ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS",
			"ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS",
			"ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS",
			"ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS",
			"ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS",
			"ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS",
			"ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS",
			"ASSIMP_BUILD_NO_DEBONE_PROCESS",
			"ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS",
			"ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS",
		}

		files {
		  	path.join(ASSIMP_DIR, "include/**"),
		  	path.join(ASSIMP_DIR, "code/**"),
		}

		includedirs {
			path.join(ASSIMP_DIR, "code"),
			path.join(ASSIMP_DIR, "include"),
			path.join(ASSIMP_DIR, "contrib/irrXML"),
			path.join(ASSIMP_DIR, "contrib/zlib"),
			path.join(ASSIMP_DIR, "contrib/rapidjson/include"),
		}


		configuration "windows"
			--systemversion "latest"

		configuration "Debug"
			flags {"DebugRuntime", "Symbols"}

		configuration "Release"
			flags {"ReleaseRuntime", "Optimize"}

		configuration {}
			flags {"Cpp17",
			"StaticRuntime"}

	startproject "app"
	
	project "app"
		--uuid				"e0ba3c4d-338b-4517-8bbd-b29311fd6830"
		kind				"WindowedApp"

		files {
							"./src/**.cpp",
							"./src/**.h",
							"./src/**.hpp",
		}
		
		includedirs {
							"./src/",
							path.join(GLFW_DIR, "include"),
							path.join(BX_DIR, "include"),
							path.join(BX_DIR, "3rdparty"),
							path.join(BGFX_DIR, "include"),
							path.join(BGFX_DIR, "3rdparty"),
							path.join(BGFX_DIR, "examples/common"),
							--path.join(BGFX_DIR, "3rdparty/forsyth-too"),
							path.join(BIMG_DIR, "include"),
							path.join(BIMG_DIR, "3rdparty"),
							IMGUI_DIR,
							GLM_DIR,
							path.join(ASSIMP_DIR, "include")
		}

		links {
							"glfw",
							"bgfx",
							"bimg",
							"bimg_decode",
							"bx",
							"imgui",
							"entry",
							"assimp",
							"shaderc"
		}
	
		configuration		"Debug"
			targetsuffix	"_d"
			flags			{ "Symbols" }
			
		configuration		"Release"
			targetsuffix	"_r"
			flags			{ "Optimize" }

		configuration {}
		debugdir			(PROJECT_RUNTIME_DIR)
		targetdir			(PROJECT_RUNTIME_DIR)

		matches = os.matchfiles(path.join(PROJECT_DIR, "assets/**.sc"))

		for i,file in pairs(matches) do
			if not string.find(file, "def.") then
				local shader_type = "vertex"
				if string.find(file, "fs_") then shader_type = "fragment" end
				custombuildtask {
					{ file, PROJECT_BUILD_DIR .. "/bin/shaders/dx11/" .. path.getbasename(file) .. ".bin", { },
					{"echo "..path.join(PROJECT_DIR, "scripts/build_shader_win.bat"),
					 "echo " .. " $(<) ",
					 "call " .. path.join(PROJECT_DIR, "scripts/build_shader_win.bat") .. " $(<) " .. shader_type}}
-- see https://stackoverflow.com/questions/3686837/why-are-my-custom-build-steps-not-running-in-visual-studio for why we need "call"
				}
			end
    	end

		configuration { "vs*" }
		buildoptions
		{
			"/wd 4127", -- Disable 'Conditional expression is constant' for do {} while(0).
			"/wd 4201", -- Disable 'Nonstandard extension used: nameless struct/union'. Used for uniforms in the project.
			"/wd 4345", -- Disable 'An object of POD type constructed with an initializer of the form () will be default-initialized'. It's an obsolete warning.
		}
		linkoptions
		{
			"/ignore:4199", -- LNK4199: /DELAYLOAD:*.dll ignored; no imports found from *.dll
		}
		links
		{ -- this is needed only for testing with GLES2/3 on Windows with VS2008
			"DelayImp",
		}

	configuration "windows"
		links
		{
			"psapi",
		}

	configuration { "vs2010" }
		linkoptions
		{ -- this is needed only for testing with GLES2/3 on Windows with VS201x
			"/DELAYLOAD:\"libEGL.dll\"",
			"/DELAYLOAD:\"libGLESv2.dll\"",
		}

	configuration {}

	group "tools"
	dofile (path.join(BGFX_DIR, "scripts/shaderc.lua"))
	dofile (path.join(BGFX_DIR, "scripts/texturec.lua"))
	dofile (path.join(BGFX_DIR, "scripts/texturev.lua"))
	dofile (path.join(BGFX_DIR, "scripts/geometryc.lua"))
	dofile (path.join(BGFX_DIR, "scripts/geometryv.lua"))
