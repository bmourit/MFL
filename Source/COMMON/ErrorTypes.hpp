// Template error handling mechanism
// Copyright (c) 2024 B. Mourit <bnmguy@gmail.com>
// All rights reserved.

#pragma once

#include <cstdio>
#include <type_traits>

template<typename T, typename E>
class Result {
public:
    Result(T* value, E error, const char* file, int line)
        : value_(value), error_(error), file_(file), line_(line) {}

    T& value() const {
        return *value_;
    }
    E error() const {
        return error_;
    }
    const char* file() const {
        return file_;
    }
    int line() const {
        return line_;
    }

private:
    T* value_;
    E error_;
    const char* file_;
    int line_;
};

// Macro update to return nullptr when an error occurs
#define RETURN_ERROR(type, code) Result<type, decltype(code)>{ nullptr, code, __FILE__, __LINE__ }

template<typename EnumClass, typename InstanceType, typename ErrorCode>
Result<InstanceType, ErrorCode> get_enum_instance(EnumClass Base, EnumClass valid_base, InstanceType& instance) {
    if (Base == valid_base) {
        return { &instance, ErrorCode::OK, nullptr, 0 };
    } else {
        return RETURN_ERROR(InstanceType, ErrorCode::INVALID_SELECTION);
    }
}
