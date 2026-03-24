#include "PolynomialDrawer.hpp"
#include "Polynomial.hpp"

PolynomialDrawer::PolynomialDrawer(Polynomial&& polynomial, glm::int32_t xMin, glm::int32_t xMax) : polynomial(std::move(polynomial)), xMin(xMin), xMax(xMax) {}

void PolynomialDrawer::draw() {
    /// @todo Implement the drawing logic for the polynomial using the provided parameters.
}

void PolynomialDrawer::setPolynomial(Polynomial&& polynomial) { this->polynomial = std::move(polynomial); }
void PolynomialDrawer::setXMin(glm::int32_t xMin) { this->xMin = xMin; }
void PolynomialDrawer::setXMax(glm::int32_t xMax) { this->xMax = xMax; }

const Polynomial& PolynomialDrawer::getPolynomial() const { return polynomial; }
glm::int32_t PolynomialDrawer::getXMin() const { return xMin; }
glm::int32_t PolynomialDrawer::getXMax() const { return xMax; }