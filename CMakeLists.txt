cmake_minimum_required(VERSION 3.5)
project(my_drogon_app)

set(DROGON_ENABLE_ORM ON) # 启用 ORM 功能
set(CMAKE_CXX_STANDARD 20) # 设置 C++ 标准为 20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 要求使用 C++ 标准编译

add_subdirectory(dependencies/jwt-cpp) # 添加 jwt-cpp 依赖
add_subdirectory(dependencies/Bcrypt) # 添加 Bcrypt 依赖
find_package(Drogon REQUIRED) # 查找 Drogon 库

# 添加可执行文件
add_executable(${PROJECT_NAME}
    src/main.cc
    src/controllers/api_v1_User.cc
    src/controllers/api_v1_todos.cc
    src/controllers/api_v1_health.cc
    src/models/Users.cc
    src/models/Todos.cc
    dependencies/jwt-cpp
    dependencies/Bcrypt
    src/filters/auth.cc
    src/utils/AppError.cc
    src/utils/token.cc
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Drogon::Drogon
    PRIVATE jwt-cpp::jwt-cpp
    PRIVATE bcrypt
)
# 用于禁用关于使用过时声明的警告
add_compile_options(-Wno-deprecated-declarations)

# 1. 设置源配置文件路径（假设在项目根目录）
set(CONFIG_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/config.json")

# 2. 检查配置文件是否存在
if(NOT EXISTS ${CONFIG_FILE})
    message(FATAL_ERROR "配置文件不存在: ${CONFIG_FILE}")
endif()

# 3. 添加拷贝命令（在构建后自动执行）
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CONFIG_FILE}"                                # 源文件
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"            # 目标目录（自动解析）
        COMMENT "正在拷贝配置文件到: $<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
)
