add_rules("mode.debug", "mode.release")
set_languages("c++17")
add_requires("libsdl2 >=2.30.0", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image", "serial")
-- debug plot
add_requires("imgui latest", {configs = {sdl2 = true, sdl_renderer = true}})
add_requires("implot latest")

set_defaultmode("debug")



target("sdl2_test_project")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("libsdl2", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image","serial")
    on_load(function (target)
        if is_mode("debug") then
            target:add("packages", "imgui", "implot")
            target:add("defines", "ENABLE_GUI")
            
            target:set("symbols", "debug")
            target:set("optimize", "none")
            target:set("warnings", "all")
            target:add("defines", "DEBUG")
        else
            target:set("symbols", "hidden")
            target:set("optimize", "fastest")
            target:add("defines", "NDEBUG")
        end
    end)

    if is_os("windows") then
        add_ldflags("/SUBSYSTEM:CONSOLE")
        if is_mode("debug") then
            add_syslinks("user32", "gdi32", "shell32", "opengl32") 
        end
    elseif is_os("linux") then
        add_ldflags("-lpthread", "-lrt", "-lGL")
    end
    after_build(function (target)
        cpResToDir = target:targetdir() .. "/res"
        os.mkdir(cpResToDir)
        local files = os.files("res/*")
        for _, file in ipairs(files) do
            os.cp(file, cpResToDir)
        end
        msg = "Building finished, cp res files to " .. cpResToDir
        print(msg)
    end)

