#pragma once

#include <string>

#include "domain/services/IPasswordHasher.h"

namespace cf::tests {

class FakePasswordHasher : public domain::IPasswordHasher {
public:
    std::string hash(const std::string& plain) const override {
        return "hashed:" + plain;
    }

    bool verify(const std::string& plain, const std::string& hashed) const override {
        return hash(plain) == hashed;
    }
};

}
