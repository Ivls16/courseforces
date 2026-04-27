#pragma once

#include <SQLiteCpp/SQLiteCpp.h>

namespace cf::infrastructure {

class SqlSchema {
public:
    static void initialize(SQLite::Database& db);
};

}
