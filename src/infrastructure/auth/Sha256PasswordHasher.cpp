#include "infrastructure/auth/Sha256PasswordHasher.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "picosha2.h"

namespace cf::infrastructure {

Sha256PasswordHasher::Sha256PasswordHasher()
    : rng_(std::random_device{}()) {}

std::string Sha256PasswordHasher::sha256(const std::string& data) const {
    return picosha2::hash256_hex_string(data);
}

std::string Sha256PasswordHasher::generate_salt() const {
    std::uniform_int_distribution<int> dist(0, 15);
    const char hex[] = "0123456789abcdef";
    std::string salt(16, '0');
    for (char& c : salt) {
        c = hex[dist(rng_)];
    }
    return salt;
}

std::string Sha256PasswordHasher::hash(const std::string& plain) const {
    std::string salt = generate_salt();
    return salt + "$" + sha256(salt + plain);
}

bool Sha256PasswordHasher::verify(const std::string& plain,
                                  const std::string& stored) const {
    auto sep = stored.find('$');
    if (sep == std::string::npos) return false;
    std::string salt = stored.substr(0, sep);
    std::string expected_hash = stored.substr(sep + 1);
    return sha256(salt + plain) == expected_hash;
}

}
