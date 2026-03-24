#pragma once

#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <GLFW/glfw3.h>

/**
 * @class Window
 * @brief A class responsible for managing the application window and its properties.
 */
class Window {
private:
    /**
     * @name The GLFW window object
     */
    GLFWwindow* window;

    /**
     * @name Properties of the window such as width, height, and title
     */
    /// @{
    uint32_t width;
    uint32_t height;
    const char* title;
    bool resizable;
    bool limitFrameRate;
    /// @}

public:
    /**
     * @name Default window size
     *
     */
    static constexpr VkExtent2D DEFAULT_WINDOW_SIZE{1280, 720};

    /**
     * @brief Constructor to initialize the window.
     */
    Window(uint32_t width, uint32_t height, const char* title, bool resizable = true, bool limitFrameRate = false);

    /**
     * @brief Destructor to clean up resources.
     */
    ~Window();

    /**
     * @brief Show the window and start the main loop.
     */
    void show();

    /**
     * @brief Check if the window should close.
     *
     * @return true if the window should close, false otherwise.
     */
    bool shouldClose() const;

    /**
     * @brief Destroy the window and clean up resources.
     *
     * @note This function is usually called automatically in the destructor, but can be called manually if needed.
     */
    void destroy();
};

#endif // _WINDOW_HPP_