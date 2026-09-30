cmake_minimum_required(VERSION 3.21)

project(ccpp2026)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 命名约定：源文件名 = 目录名去掉 pNN_ 前缀，扩展名 .cpp（C 语言用 .c）。
#     level1/p04_hanoi/            -> hanoi.cpp          (目录名 p04_hanoi)
#     level1/p09_safe_array_cpp/   -> safe_array_cpp.cpp
#     level0/bubbleSort/           -> bubbleSort.cpp
#     level2/PI/                   -> PI.cpp
#
# 扫描 level0 / level1 / level2 下所有题目目录，自动生成可执行目标。
# 写新题只需按上面的约定建目录放文件，在 CLion 里 reload 一下 CMake
# 目标就出来了，不用回来改这个文件；还没写的目录自动跳过。
#
# 每个目录只认这一个主源文件，目录里其它 .cpp / .c 一律不参与构建，
# 可以随便放草稿、旧版本、测试代码，不会互相干扰。

set(_levels level0 level1 level2)

foreach(_lv IN LISTS _levels)
    file(GLOB _dirs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${_lv}"
         "${CMAKE_CURRENT_SOURCE_DIR}/${_lv}/*")

    foreach(_dir IN LISTS _dirs)
        if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_lv}/${_dir}")
            continue()
        endif()
        # 跳过 .git / .idea / .vscode 这类隐藏目录
        if(_dir MATCHES "^\\.")
            continue()
        endif()

        # 目录名去掉开头连续的 pxx_ / pxx 前缀
        #     p04_hanoi        -> hanoi
        #     p09_safe_array_cpp -> safe_array_cpp
        #     bubbleSort / PI  -> 原样
        set(_name "${_dir}")
        if(_name MATCHES "^p[0-9]+_(.+)$")
            set(_name "${CMAKE_MATCH_1}")
        elseif(_name MATCHES "^p[0-9]+(.+)$")
            set(_name "${CMAKE_MATCH_1}")
        endif()

        # 主源文件：<目录名去前缀>.cpp / .c
        set(_main_src "")
        foreach(_ext cpp c cxx cc)
            set(_cand "${CMAKE_CURRENT_SOURCE_DIR}/${_lv}/${_dir}/${_name}.${_ext}")
            if(EXISTS "${_cand}")
                set(_main_src "${_cand}")
                break()
            endif()
        endforeach()

        if(NOT _main_src)
            message(STATUS "skip ${_lv}/${_dir}  (还没有 ${_name}.cpp)")
            continue()
        endif()

        # 只认这一个主文件，同目录其它 .c/.cpp 忽略
        # 目标名 = 目录名，保证 exe 输出名与原版一致
        add_executable("${_dir}" "${_main_src}")
        message(STATUS "add ${_dir}  <-  ${_lv}/${_dir}/${_name}.cpp")
    endforeach()
endforeach()
