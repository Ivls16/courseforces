#pragma once

#include <random>

#include "domain/services/IPasswordHasher.h"

namespace cf::infrastructure {

// SHA-256 via picosha2 + random hex salt.
// Format stored: "<16-char hex salt>$<sha256-hex>"
class Sha256PasswordHasher : public domain::IPasswordHasher {
public:
    Sha256PasswordHasher();

    std::string hash(const std::string& plain) const override;
    bool verify(const std::string& plain, const std::string& stored) const override;

private:
    std::string generate_salt() const;
    std::string sha256(const std::string& data) const;

    mutable std::mt19937 rng_;
};

}
