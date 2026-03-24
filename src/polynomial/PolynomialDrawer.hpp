#pragma once

#include "glm/fwd.hpp"
#ifndef _POLYNOMIAL_DRAWER_HPP_
#define _POLYNOMIAL_DRAWER_HPP_

#include "Polynomial.hpp"
#include <glm/glm.hpp>

/**
 * @class PolynomialDrawer
 * @brief A class responsible for drawing a polynomial on a graphical interface.
 */
class PolynomialDrawer {
private:
    /**
     * @name The polynomial to be drawn
     */
    Polynomial polynomial;

    /**
     * @name The minimum and maximum x-values for the drawing area
     */
    /// @{
    glm::int32_t xMin;
    glm::int32_t xMax;
    /// @}

public:
    /**
     * @brief Constructor to initialize the polynomial drawer with a polynomial and drawing parameters.
     *
     * @param polynomial The polynomial to be drawn
     * @param xMin The minimum x-value for the drawing area
     * @param xMax The maximum x-value for the drawing area
     *
     * @note The constructor takes ownership of the polynomial object using move semantics to optimize performance.
     */
    PolynomialDrawer(Polynomial&& polynomial, glm::int32_t xMin, glm::int32_t xMax);

    /**
     * @brief Draws the polynomial on the graphical interface.
     *
     * This function should be implemented to render the polynomial based on the provided parameters.
     */
    void draw();

    /**
     * @brief Setters of the polynomial drawing parameters.
     *
     * @param polynomial The polynomial to be drawn
     * @param xMin The minimum x-value for the drawing area
     * @param xMax The maximum x-value for the drawing area
     *
     * @note Polynomial is set using move semantics to optimize performance when updating the polynomial to be drawn.
     */
    /// @{
    void setPolynomial(Polynomial&& polynomial);
    void setXMin(glm::int32_t xMin);
    void setXMax(glm::int32_t xMax);
    /// @}

    /**
     * @brief Getters of the polynomial drawing parameters.
     *
     * @return The current polynomial, xMin or xMax for drawing the polynomial.
     */
    /// @{
    const Polynomial& getPolynomial() const;
    glm::int32_t getXMin() const;
    glm::int32_t getXMax() const;
    /// @}
};

#endif // _POLYNOMIAL_DRAWER_HPP_