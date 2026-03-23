add_executable(
    Assignment1
    main.cpp
    polynomial/Polynomial.cpp
    polynomial/PolynomialDrawer.cpp
    window/Window.cpp
    vulkan/VulkanBase.cpp
)

target_link_libraries(
    Assignment1
    PRIVATE
    glm::glm
    glfw
    fmt::fmt
    Vulkan::Vulkan
)

target_compile_definitions(Assignment1 PRIVATE GLFW_INCLUDE_VULKAN)

if(WIN32)
    target_compile_definitions(Assignment1 PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()