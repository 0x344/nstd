#pragma once

/*
MIT License
Copyright (c) 2017 Arlen Keshabyan (arlen.albert@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

extern "C"
{
#include "external/sqlite/sqlite3.h"
}

#define MODERN_SQLITE_STD_OPTIONAL_SUPPORT
#include "external/sqlite_modern_cpp/sqlite_modern_cpp.h"
#include <vector>

namespace nstd
{

namespace sqlite = sqlite;

namespace db
{

struct scoped_transaction
{
    scoped_transaction(nstd::sqlite::database &db) : _db(db) { _db << "begin;"; };
    ~scoped_transaction() { if (!_rollback) _db << "commit;"; else _db << "rollback;"; };

    void rollback(bool rollback = true) { _rollback = rollback; }

private:
    nstd::sqlite::database &_db;
    bool _rollback { false };
};

template<typename Target, typename... AttrTypes>
struct result_builder
{
    std::vector<Target> data;

    void operator()(AttrTypes... args)
    {
        data.emplace_back(std::forward<AttrTypes&&>(args)...);
    };
};

}
}
