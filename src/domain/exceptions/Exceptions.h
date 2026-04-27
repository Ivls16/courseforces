#pragma once

#include <stdexcept>
#include <string>

namespace cf::domain {

class DomainException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class NotFoundException : public DomainException {
public:
    using DomainException::DomainException;
};

class ConflictException : public DomainException {
public:
    using DomainException::DomainException;
};

class ForbiddenException : public DomainException {
public:
    using DomainException::DomainException;
};

class AuthenticationException : public DomainException {
public:
    using DomainException::DomainException;
};

}
