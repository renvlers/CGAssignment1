#include "PolynomialDrawer.hpp"
#include "Polynomial.hpp"
#include "glm/fwd.hpp"

PolynomialDrawer::PolynomialDrawer(Polynomial&& polynomial, glm::int32_t xMin, glm::int32_t xMax) : polynomial(std::move(polynomial)), xMin(xMin), xMax(xMax) {}

std::vector<Vertex> PolynomialDrawer::draw() {
    glm::int32_t dForward = polynomial.getA() + polynomial.getB();
    glm::int32_t dBackward = polynomial.getA() - polynomial.getB();

    glm::int32_t d2 = polynomial.getA() + polynomial.getA();

    glm::int32_t x = 0;
    glm::int32_t y = polynomial.getC();

    while (x < xMin) {
        y += dForward;
        dForward += d2;
        x++;
    }

    while (x > xMin) {
        y += dBackward;
        dBackward += d2;
        x--;

        dForward = d2 - dBackward;
    }

    std::vector<Vertex> vertices;
    while (x <= xMax) {
        vertices.push_back({static_cast<float>(x), static_cast<float>(y)});
        x++;
        y += dForward;
        dForward += d2;
    }

    return std::move(vertices);
}

void PolynomialDrawer::setPolynomial(Polynomial&& polynomial) { this->polynomial = std::move(polynomial); }
void PolynomialDrawer::setXMin(glm::int32_t xMin) { this->xMin = xMin; }
void PolynomialDrawer::setXMax(glm::int32_t xMax) { this->xMax = xMax; }

const Polynomial& PolynomialDrawer::getPolynomial() const { return polynomial; }
glm::int32_t PolynomialDrawer::getXMin() const { return xMin; }
glm::int32_t PolynomialDrawer::getXMax() const { return xMax; }