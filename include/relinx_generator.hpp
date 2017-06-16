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

#include <limits>
#include "relinx.hpp"
#include "uuid.hpp"

namespace nstd
{

class uuid_iterator_adapter
{
public:
    using self_type = uuid_iterator_adapter;
    using value_type = uuid::uuid;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::input_iterator_tag;

    uuid_iterator_adapter() = default;
    uuid_iterator_adapter(bool begin_iterator_flag) : _uuid(begin_iterator_flag ? uuid::uuid::generate_random() : nstd::uuid::uuid()) { }
    uuid_iterator_adapter(const uuid_iterator_adapter &) = default;
    uuid_iterator_adapter(uuid_iterator_adapter &&) = default;
    uuid_iterator_adapter &operator=(const uuid_iterator_adapter &) = default;
    uuid_iterator_adapter &operator=(uuid_iterator_adapter &&) = default;

    auto operator==(const self_type &s) const -> bool
    {
        return _uuid == s._uuid;
    }

    auto operator!=(const self_type &s) const -> bool
    {
        return !(*this == s);
    }

    auto operator*() const -> const value_type&
    {
        return _uuid;
    }

    auto operator->() -> const value_type
    {
        return *(*this);
    }

    auto operator++() -> self_type&
    {
        _uuid = uuid::uuid::generate_random();

        return *this;
    }

    auto operator++(int) -> self_type
    {
        auto __tmp = *this;

        ++(*this);

        return __tmp;
    }

protected:
    uuid::uuid _uuid;
};

class random_iterator_adapter
{
public:
    using self_type = random_iterator_adapter;
    using value_type = uint64_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::input_iterator_tag;

    random_iterator_adapter() = default;
    random_iterator_adapter(bool begin_iterator_flag) : _result(begin_iterator_flag ? uuid::uuid::get_random_number() : std::numeric_limits<uint64_t>::max()) { }
    random_iterator_adapter(const random_iterator_adapter &) = default;
    random_iterator_adapter(random_iterator_adapter &&) = default;
    random_iterator_adapter &operator=(const random_iterator_adapter &) = default;
    random_iterator_adapter &operator=(random_iterator_adapter &&) = default;

    auto operator==(const self_type &s) const -> bool
    {
        return _result == s._result;
    }

    auto operator!=(const self_type &s) const -> bool
    {
        return !(*this == s);
    }

    auto operator*() const -> const value_type&
    {
        return _result;
    }

    auto operator->() -> const value_type
    {
        return *(*this);
    }

    auto operator++() -> self_type&
    {
        do
        {
            _result = uuid::uuid::get_random_number();
        }
        while (_result == std::numeric_limits<uint64_t>::max());

        return *this;
    }

    auto operator++(int) -> self_type
    {
        auto __tmp = *this;

        ++(*this);

        return __tmp;
    }

protected:
    uint64_t _result { std::numeric_limits<uint64_t>::max() };
};

auto from_uuid()
{
    return nstd::relinx::from(uuid_iterator_adapter(true), uuid_iterator_adapter());
}

auto from_random()
{
    return nstd::relinx::from(random_iterator_adapter(true), random_iterator_adapter());
}

}