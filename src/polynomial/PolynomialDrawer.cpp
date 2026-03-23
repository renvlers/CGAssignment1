#include "PolynomialDrawer.hpp"
#include "Polynomial.hpp"

PolynomialDrawer::PolynomialDrawer(Polynomial&& polynomial, glm::float64_t xMin, glm::float64_t xMax, glm::float64_t delta) : polynomial(std::move(polynomial)), xMin(xMin), xMax(xMax), delta(delta) {}

void PolynomialDrawer::draw() {
    /// @todo Implement the drawing logic for the polynomial using the provided parameters.
}

void PolynomialDrawer::setPolynomial(Polynomial&& polynomial) { this->polynomial = std::move(polynomial); }
void PolynomialDrawer::setXMin(glm::float64_t xMin) { this->xMin = xMin; }
void PolynomialDrawer::setXMax(glm::float64_t xMax) { this->xMax = xMax; }
void PolynomialDrawer::setDelta(glm::float64_t delta) { this->delta = delta; }

const Polynomial& PolynomialDrawer::getPolynomial() const { return polynomial; }
glm::float64_t PolynomialDrawer::getXMin() const { return xMin; }
glm::float64_t PolynomialDrawer::getXMax() const { return xMax; }
glm::float64_t PolynomialDrawer::getDelta() const { return delta; }