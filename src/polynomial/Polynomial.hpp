#pragma once

#ifndef _POLYNOMIAL_HPP_
#define _POLYNOMIAL_HPP_

#include <glm/glm.hpp>

/**
 * @class Polynomial
 * @brief A class representing a polynomial of the form ax^2 + bx + c.
 */
class Polynomial {
private:
    /**
     * @name Coefficients of the polynomial
     *
     * Coefficient a corresponds to x^2, b corresponds to x, and c is the constant term.
     */
    /// @{
    glm::float64_t a;
    glm::float64_t b;
    glm::float64_t c;
    /// @}

public:
    /**
     * @brief Constructor to initialize the coefficients of the polynomial.
     *
     * @param a Coefficient of x^2
     * @param b Coefficient of x
     * @param c Constant term
     */
    Polynomial(glm::float64_t a, glm::float64_t b, glm::float64_t c);

    /**
     * @brief Copy constructor for the Polynomial class.
     *
     * @param other The polynomial to be copied from.
     */
    Polynomial(const Polynomial& other);

    /**
     * @brief Move constructor for the Polynomial class.
     *
     * @param other The polynomial to be moved from.
     */
    Polynomial(Polynomial&& other) noexcept;

    /**
     * @brief Assignment operator for the Polynomial class.
     *
     * @param other The polynomial to be assigned from.
     * @return A reference to the current polynomial after assignment.
     */
    Polynomial& operator=(const Polynomial& other);

    /**
     * @brief Move assignment operator for the Polynomial class.
     *
     * @param other The polynomial to be moved from.
     * @return A reference to the current polynomial after move assignment.
     */
    Polynomial& operator=(Polynomial&& other) noexcept;

    /**
     * @brief Getters for the coefficients of the polynomial.
     *
     * @return The coefficient a, b, or c of the polynomial.
     */
    /// @{
    glm::float64_t getA() const;
    glm::float64_t getB() const;
    glm::float64_t getC() const;
    /// @}

    /**
     * @brief Setters for the coefficients of the polynomial.
     *
     * @param a Coefficient of x^2
     * @param b Coefficient of x
     * @param c Constant term
     */
    /// @{
    void setA(glm::float64_t a);
    void setB(glm::float64_t b);
    void setC(glm::float64_t c);
    /// @}
};

#endif // _POLYNOMIAL_HPP_