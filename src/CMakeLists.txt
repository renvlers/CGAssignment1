add_executable(
    Assignment1
    main.cpp
    polynomial/Polynomial.cpp
    polynomial/PolynomialDrawer.cpp
    window/Window.cpp
    vulkan/VulkanBase.cpp
    vulkan/Fence.cpp
    vulkan/Semaphore.cpp
    vulkan/CommandBuffer.cpp
    vulkan/CommandPool.cpp
    vulkan/RenderPass.cpp
    vulkan/FrameBuffer.cpp
    vulkan/ShaderModule.cpp
    vulkan/GraphicsPipelineCreateInfoPack.cpp
    vulkan/Pipeline.cpp
    vulkan/PipelineLayout.cpp
    vulkan/Functions.cpp
    util/Result.cpp
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

find_program(Vulkan_GLSLC_EXECUTABLE NAMES glslc HINTS $ENV{VULKAN_SDK}/bin)

if(NOT Vulkan_GLSLC_EXECUTABLE)
    message(FATAL_ERROR "Can't find glslc executable. Please make sure you have Vulkan SDK installed and set VULKAN_SDK environment variable correctly.")
endif()

set(SHADER_SOURCES
    "shader/VertShader.vert"
    "shader/FragShader.frag"
)

set(SPV_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shader/build")
file(MAKE_DIRECTORY ${SPV_OUTPUT_DIR})

set(SPV_OUTPUTS "")

foreach(SHADER IN LISTS SHADER_SOURCES)
    get_filename_component(FILE_NAME ${SHADER} NAME)

    set(SPV_FILE "${SPV_OUTPUT_DIR}/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPV_FILE}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${SPV_FILE}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}
        COMMENT "Compiling ${SHADER} to SPIR-V..."
    )

    list(APPEND SPV_OUTPUTS ${SPV_FILE})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPV_OUTPUTS})
add_dependencies(Assignment1 CompileShaders)