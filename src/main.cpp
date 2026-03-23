#include "window/Window.hpp"

int main() {
    Window window(800, 600, "Polynomial", true);
    window.show([]() {
        // This is where the rendering logic will go. For now, it's just a placeholder.
    });
}