#include "Polynomial.hpp"

Polynomial::Polynomial(glm::float64_t a, glm::float64_t b, glm::float64_t c) : a(a), b(b), c(c) {}
Polynomial::Polynomial(const Polynomial& other) : a(other.a), b(other.b), c(other.c) {}
Polynomial::Polynomial(Polynomial&& other) noexcept : a(other.a), b(other.b), c(other.c) {
    other.a = 0;
    other.b = 0;
    other.c = 0;
}

Polynomial& Polynomial::operator=(const Polynomial& other) {
    if (this != &other) {
        a = other.a;
        b = other.b;
        c = other.c;
    }

    return *this;
}

Polynomial& Polynomial::operator=(Polynomial&& other) noexcept {
    if (this != &other) {
        a = other.a;
        b = other.b;
        c = other.c;

        other.a = 0;
        other.b = 0;
        other.c = 0;
    }

    return *this;
}

glm::float64_t Polynomial::getA() const { return a; }
glm::float64_t Polynomial::getB() const { return b; }
glm::float64_t Polynomial::getC() const { return c; }

void Polynomial::setA(glm::float64_t a) { this->a = a; }
void Polynomial::setB(glm::float64_t b) { this->b = b; }
void Polynomial::setC(glm::float64_t c) { this->c = c; }