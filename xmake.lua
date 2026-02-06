add_rules("mode.debug", "mode.release")
set_languages("c++17")

-- 声明依赖项：SDL2 及其核心扩展库

add_requires("libsdl2", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image")

-- 如需外设通信，可在此添加相应的包，例如：
-- add_requires("serial")   -- 用于串口通信
-- add_requires("libusb")   -- 用于USB设备通信
-- compiler command 

target("sdl2_test_project")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("libsdl2", "libsdl2_ttf", "libsdl2_mixer", "libsdl2_image")

    -- 如需外设通信，在此链接相应的包，例如：
    -- add_packages("serial")
    -- add_packages("libusb")

    if is_os("windows") then
        add_ldflags("/SUBSYSTEM:CONSOLE")
    end