#ifndef BITMANIPULATIONCLS_H
#define BITMANIPULATIONCLS_H

#include <cstdint>
#include <concepts>
#include <type_traits>
#include <queue>

namespace UtilityLib
{
    namespace BitManipulation
    {
        // TwosComplement()
        // 
        // Summary:
        // Calculates Two's Complement of an integral
        // 
        // Arguments:
        // T value  --- In (T must be an integral type)
        // 
        // Returns
        // T
        template<std::integral T>
        constexpr T TwosComplement(T value)
        {
            return ~value + 1;
        }

        // PrepareBitMask()
        // 
        // Summary:
        // Prepare a bit mask with given length
        // 
        // Arguments:
        // size_t length  --- In
        // 
        // Returns
        // size_t
        constexpr size_t PrepareBitMask(size_t length)
        {
            size_t result = 0;
            for (size_t i = 0; i < length; i++)
            {
                result = (result << 1) + 1;
            }

            return result;
        }

        // GetSubBits()
        // 
        // Summary:
        // Extract sub bits based on provided offset and length
        // 
        // Arguments:
        // T value        --- In (T must be an unsigned integral type)
        // size_t offset  --- In
        // size_t length  --- In
        // 
        // Returns
        // T
        template<std::unsigned_integral T>
        constexpr T GetSubBits(T value, size_t offset, size_t length)
        {
            value = value >> offset;
            T bitMask = static_cast<T>(PrepareBitMask(length));
            value = value & bitMask;

            return value;
        }

        // Reverse()
        // 
        // Summary:
        // Reverses bits of an unsigned integral
        // 
        // Arguments:
        // T value  --- In (T must be an unsigned integral type)
        // 
        // Returns:
        // T
        template<std::unsigned_integral T>
        constexpr T Reverse(T value)
        {
            T result = 0;
            size_t size = sizeof(value);

            for (size_t i = 0; i < size; i++)
            {
                result = (result << 1) | (value & 1);
                value = value >> 1;
            }

            return result;
        }
    }
}

#endif