add_rules("mode.debug", "mode.release")
set_languages("c++17")
add_requires("libsdl2 >=2.30.0", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image", "serial")
set_defaultmode("debug")



target("sdl2_test_project")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("libsdl2", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image","serial")
    on_load(function (target)
        if is_mode("debug") then
            target:set("symbols", "debug")
            target:set("optimize", "none")
            target:set("warnings", "all")
            target:add("defines", "DEBUG")
            print("Building in DEBUG mode")
        elseif is_mode("release") then
            target:set("symbols", "hidden")
            target:set("optimize", "fastest")
            target:add("defines", "NDEBUG")
            print("Building in RELEASE mode")
        end
    end)

    if is_os("windows") then
        add_ldflags("/SUBSYSTEM:CONSOLE")
        if is_mode("debug") then
            add_cxflags("/Zi")
            add_ldflags("/DEBUG")
        end
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
    end )

