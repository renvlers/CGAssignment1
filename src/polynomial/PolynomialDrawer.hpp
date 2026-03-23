#pragma once

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
    glm::float64_t xMin;
    glm::float64_t xMax;
    /// @}

    /**
     * @name The step size for drawing the polynomial.
     */
    glm::float64_t delta;

public:
    /**
     * @brief Constructor to initialize the polynomial drawer with a polynomial and drawing parameters.
     *
     * @param polynomial The polynomial to be drawn
     * @param xMin The minimum x-value for the drawing area
     * @param xMax The maximum x-value for the drawing area
     * @param delta The step size for drawing the polynomial
     *
     * @note The constructor takes ownership of the polynomial object using move semantics to optimize performance.
     */
    PolynomialDrawer(Polynomial&& polynomial, glm::float64_t xMin, glm::float64_t xMax, glm::float64_t delta);

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
     * @param delta The step size for drawing the polynomial
     *
     * @note Polynomial is set using move semantics to optimize performance when updating the polynomial to be drawn.
     */
    /// @{
    void setPolynomial(Polynomial&& polynomial);
    void setXMin(glm::float64_t xMin);
    void setXMax(glm::float64_t xMax);
    void setDelta(glm::float64_t delta);
    /// @}

    /**
     * @brief Getters of the polynomial drawing parameters.
     *
     * @return The current polynomial, xMin, xMax, or delta used for drawing the polynomial.
     */
    /// @{
    const Polynomial& getPolynomial() const;
    glm::float64_t getXMin() const;
    glm::float64_t getXMax() const;
    glm::float64_t getDelta() const;
    /// @}
};

#endif // _POLYNOMIAL_DRAWER_HPP_