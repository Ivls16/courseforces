#pragma once

#include <string>

namespace cf::domain {

class IPasswordHasher {
public:
    virtual ~IPasswordHasher() = default;

    virtual std::string hash(const std::string& plain) const = 0;
    virtual bool verify(const std::string& plain, const std::string& hashed) const = 0;
};

}
