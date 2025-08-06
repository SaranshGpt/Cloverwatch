//
// Created by saransh on 7/23/25.
//

#ifndef PTR_TYPES_H
#define PTR_TYPES_H
#include "c_types.h"

namespace Cloverwatch {

    enum class PtrIntent {

        // The memory value at the location may be read from for the duration of the function scope
        READONLY,

        // The memory value at the location may be written to for the duration of the function scope 
        READWRITE,

        // The memory value at the location may be read from beyond the duration of the function scope
        BUFFER_READONLY,

        // The memory value at the location may be written to beyond the duration of the function scope
        BUFFER_READWRITE,

        // The pointer's ownership shall be transferred to the function
        MOVE_OWNERSHIP,

        // A copy of the memory at the location shall be made. To make RAM usage more predictable when dealing with large buffers
        COPY_CONTENTS
    };

    template <PtrIntent intent, typename T>
    class IntentPtr {
    public:
        T* ptr;

        constexpr explicit IntentPtr(T* ptr) : ptr(ptr) {}

        template <PtrIntent intent_other>
        constexpr explicit IntentPtr(IntentPtr<intent_other, T> other) : ptr(other.ptr) {}

        template <typename U = T>
        constexpr typename std::enable_if<!std::is_void<U>::value, U&>::type operator*() const { return *ptr; }
        constexpr T* operator->() const { return ptr; }
		constexpr bool operator==(const IntentPtr& other) const { return ptr == other.ptr; }
		constexpr bool operator!=(const IntentPtr& other) const { return ptr != other.ptr; }

        constexpr bool operator==(const T* other_ptr) const { return ptr == other_ptr; }
        constexpr bool operator!=(const T* other_ptr) const { return ptr != other_ptr; }
    };

    template <typename T>
    using ReadPtr = IntentPtr<PtrIntent::READONLY, const T>;

    template <typename T>
    using WritePtr = IntentPtr<PtrIntent::READWRITE, T>;

    template <typename T>
    using ReadBufferPtr = IntentPtr<PtrIntent::BUFFER_READONLY, const T>;

    template <typename T>
    using WriteBufferPtr = IntentPtr<PtrIntent::BUFFER_READWRITE, T>;

    template <typename T>
    using MovePtr = IntentPtr<PtrIntent::MOVE_OWNERSHIP, T>;

    template <typename T>
    using CopyPtr = IntentPtr<PtrIntent::COPY_CONTENTS, const T>;

};

#endif //PTR_TYPES_H