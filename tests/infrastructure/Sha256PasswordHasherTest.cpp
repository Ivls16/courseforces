#include <gtest/gtest.h>

#include "infrastructure/auth/Sha256PasswordHasher.h"

using cf::infrastructure::Sha256PasswordHasher;

TEST(Sha256PasswordHasherTest, VerifyCorrectPassword) {
    Sha256PasswordHasher h;
    auto hashed = h.hash("mypassword");
    EXPECT_TRUE(h.verify("mypassword", hashed));
}

TEST(Sha256PasswordHasherTest, RejectsWrongPassword) {
    Sha256PasswordHasher h;
    auto hashed = h.hash("correct");
    EXPECT_FALSE(h.verify("wrong", hashed));
}

TEST(Sha256PasswordHasherTest, TwoHashesOfSamePasswordDiffer) {
    Sha256PasswordHasher h;
    auto h1 = h.hash("secret");
    auto h2 = h.hash("secret");
    EXPECT_NE(h1, h2);  // different salts
    EXPECT_TRUE(h.verify("secret", h1));
    EXPECT_TRUE(h.verify("secret", h2));
}

TEST(Sha256PasswordHasherTest, MalformedStoredHashReturnsFalse) {
    Sha256PasswordHasher h;
    EXPECT_FALSE(h.verify("pass", "no_separator_here"));
    EXPECT_FALSE(h.verify("pass", ""));
}
