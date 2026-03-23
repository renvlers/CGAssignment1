#pragma once

#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include <GLFW/glfw3.h>
#include <functional>

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
    uint32_t width;
    uint32_t height;
    const char* title;
    bool resizable;

public:
    /**
     * @brief Constructor to initialize the window.
     */
    Window(uint32_t width, uint32_t height, const char* title, bool resizable = true);

    /**
     * @brief Destructor to clean up resources.
     */
    ~Window();

    /**
     * @brief Show the window and start the main loop.
     */
    void show(std::function<void()> renderCallback);

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