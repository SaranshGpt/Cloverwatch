//
// Created by saransh on 7/23/25.
//

#ifndef PTR_TYPES_H
#define PTR_TYPES_H

#include <type_traits>

namespace Cloverwatch {

    enum class RefIntent {

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

    template <RefIntent intent, typename T>
    class IntentRef {
    public:
        T& ref;

        constexpr explicit IntentRef(T &ptr) : ref(ptr) {}

        template <RefIntent intent_other>
        constexpr explicit IntentRef(IntentRef<intent_other, T> other) : ref(other.ref) {}

        constexpr T& operator->() const {return ref; }
        constexpr T& operator*() const {return ref; }
    };

    template <RefIntent intent>
    class IntentRef<intent, void> {
    public:
        void* ref;

        constexpr explicit IntentRef(void* ptr) : ref(ptr) {}

        template <RefIntent intent_other>
        constexpr explicit IntentRef(IntentRef<intent_other, void> other) : ref(other.ref) {}
    };


    template <typename T>
    using ReadRef = IntentRef<RefIntent::READONLY, const T>;

    template <typename T>
    using WriteRef = IntentRef<RefIntent::READWRITE, T>;

    template <typename T>
    using ReadBufferRef = IntentRef<RefIntent::BUFFER_READONLY, const T>;

    template <typename T>
    using WriteBufferRef = IntentRef<RefIntent::BUFFER_READWRITE, T>;

    template <typename T>
    using MoveRef = IntentRef<RefIntent::MOVE_OWNERSHIP, T>;

    template <typename T>
    using CopyRef = IntentRef<RefIntent::COPY_CONTENTS, const T>;

    template <typename T>
    constexpr ReadRef<T> ToRead(T& ref) {
        return ReadRef<T>(ref);
    }

    template <typename T>
    constexpr WriteRef<T> ToWrite(T& ref) {
        return WriteRef<T>(ref);
    }

    template <typename T>
    constexpr ReadBufferRef<T> ToReadBuffer(T& ref) {
        return ReadBufferRef<T>(ref);
    }

    template <typename T>
    constexpr WriteBufferRef<T> ToWriteBuffer(T& ref) {
        return WriteBufferRef<T>(ref);
    }

    template <typename T>
    constexpr CopyRef<T> ToCopy(T& ref) {
        return CopyRef<T>(ref);
    }

    template <typename T>
    constexpr ReadRef<T> ToRead(const T& ref) {
        return ReadRef<T>(ref);
    }

    template <typename T>
    constexpr WriteRef<T> ToWrite(const T& ref) {
        return WriteRef<T>(ref);
    }

    template <typename T>
    constexpr ReadBufferRef<T> ToReadBuffer(const T& ref) {
        return ReadBufferRef<T>(ref);
    }

    template <typename T>
    constexpr WriteBufferRef<T> ToWriteBuffer(const T& ref) {
        return WriteBufferRef<T>(ref);
    }

    template <typename T>
    constexpr CopyRef<T> ToCopy(const T& ref) {
        return CopyRef<T>(ref);
    }

};

#endif //PTR_TYPES_H