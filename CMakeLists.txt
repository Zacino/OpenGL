# 基础项目设置
cmake_minimum_required(VERSION 3.10)# CMake最低版本要求
project(OpenGL) # 项目名称

# (可选但推荐) 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. 查找依赖库
# glew glfw cmake自带配置，glm库才有配置，需要多config
# !! 注意在vscode的c_cpp_properties.json 里加上路径
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)


# 4. 定义源文件列表
# file(GLOB SOURCES src/*.cpp) 找到 src 目录下所有 .cpp 文件，然后放到变量 SOURCES 里。
file(GLOB SOURCES
    src/*.cpp
    src/vendor/imgui/*.cpp
    src/vendor/stb_image/*.cpp
    src/tests/*.cpp
)

# set(FOO bar) 定义一个变量 FOO = "bar", 以后用这个变量，要写 ${FOO}
# 覆盖变量 SOURCES 里面包含 main.cpp 以及之前的 ${SOURCES}
set(SOURCES
    main.cpp
    ${SOURCES}
)

# 5. 生成可执行文件，把指定的文件一起编译链接
add_executable(MyApp ${SOURCES})  # 等价于：add_executable(MyApp main.cpp  src/a.cpp s rc/b.cpp)

# 6. 指定内部头文件的搜索路径
# target_include_directories(<target> [SCOPE] [items...])   
#           taget为add_executable定义目标     scope指定包含目录的作用域；todo不太理解
# 只把src添加到了目录里，src/verder/下的头文件需要在include的时候指定路径，
#           也可以把src/vender添加，include的时候不用指定文件夹路径了
target_include_directories(MyApp PRIVATE src)

# 7. 链接外部依赖库
target_link_libraries(MyApp PRIVATE
    glfw       # 链接GLFW
    GLEW::GLEW # 链接GLEW (现代CMake的推荐写法)
    OpenGL::GL # 链接OpenGL框架 (现代CMake的推荐写法)
    glm::glm
)

# 8. 【解决资源路径问题的关键步骤】
# 这个命令会在构建时，将 "res" 文件夹完整地复制到生成的可执行文件所在的目录
# CMAKE_CURRENT_SOURCE_DIR 指的是当前 CMakeLists.txt 所在的目录（项目根目录）
# CMAKE_RUNTIME_OUTPUT_DIRECTORY 指的是可执行文件（如MyGame.exe）最终生成的目录
add_custom_command(TARGET MyApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:MyApp>/res"
    COMMENT "Copying resources to build directory"
)