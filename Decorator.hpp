#ifndef DECORATOR_HPP
# define DECORATOR_HPP

#include <iostream>
#include <string>

// macro to control verbose mode
#define VERBOSE 1

class Decorator {
    private:
        std::string message;
    public:
        Decorator(const std::string& msg);
        virtual ~Decorator();
        virtual std::string decorate() const;
        std::string getMessage() const;
};

#endif