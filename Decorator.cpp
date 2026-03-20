#include "Decorator.hpp"

Decorator::Decorator(const std::string& msg) : message(msg) {}

Decorator::~Decorator(){}

std::string Decorator::decorate() const {
    return (message);
}

std::string Decorator::getMessage() const {
    return (message);
}
