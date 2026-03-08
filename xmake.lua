add_rules("mode.debug", "mode.release")
set_languages("c++17")

-- 声明依赖项：SDL2 及其核心扩展库
add_requires("libsdl2 >=2.30.0", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image", "serial")

-- 设置默认构建模式为debug
set_defaultmode("debug")



target("sdl2_test_project")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("libsdl2", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image","serial")
    
    -- Debug模式配置
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
        -- Debug模式添加调试信息
        if is_mode("debug") then
            add_cxflags("/Zi")  -- 生成调试信息
            add_ldflags("/DEBUG")  -- 链接调试信息
        end
    end
    after_build(function (target)
        cpResToDir = target:targetdir() .. "/res"
        -- 确保目标目录存在
        os.mkdir(cpResToDir)
        -- 复制res目录下的所有文件
        local files = os.files("res/*")
        for _, file in ipairs(files) do
            os.cp(file, cpResToDir)
        end
        msg = "Building finished, cp res files to " .. cpResToDir
        print(msg)
    end )

