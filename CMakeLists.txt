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


# 4. 定义源文件列表 (手动维护，最清晰)
set(SOURCES
    main.cpp
    src/Render.cpp
    src/IndexBuffer.cpp
    src/VertexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
    src/vendor/stb_image/stb_image.cpp
    src/Texture.cpp
)

# 5. 创建可执行文件
add_executable(MyApp ${SOURCES})


# 6. 指定头文件的搜索路径
# 目标 "Myapp" 需要在 "src" 目录中寻找头文件
# 这样 main.cpp 和 Renderer.cpp 都可以 #include "Renderer.h"
target_include_directories(MyApp PRIVATE src)


# 7. 链接依赖库
# 这是告诉CMake：“编译MyApp时，需要用到这些库”。
# CMake会自动处理头文件路径(-I)和库文件链接(-L -l)。
# "PRIVATE" 表示这些库是 MyApp 的内部实现细节。
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