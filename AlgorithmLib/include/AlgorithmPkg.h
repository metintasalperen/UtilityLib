#ifndef ALGORITHMPKG_H
#define ALGORITHMPKG_H

#include <concepts>
#include <vector>

namespace UtilityLib
{
    namespace Algorithm
    {
        // Concept to check if C is a container
        template<typename C>
        concept Container = requires(C c)
        {
            std::begin(c);
            std::end(c);
        };

        // Concept to check if container C has the same type as V
        // std::vector<int>, int   -> true
        // std::vector<int>, float -> false
        template <typename C, typename V>
        concept ValueMatches = Container<C> && std::is_same_v<V, typename C::value_type>;

        // IsExist()
        // 
        // Summary:
        // Check if the value is in container
        // 
        // Arguments:
        // const C& container  --- In (C must be a container such as std::vector)
        // const V& value      --- In (V must be same type as value type of C and V must be equality comparable)
        // 
        // Returns:
        // bool
        template <typename C, typename V>
        requires Container<C> && ValueMatches<C, V> && std::equality_comparable<V>
        constexpr bool IsExist(const C& container, const V& value)
        {
            for (const auto& elem : container)
            {
                if (elem == value)
                {
                    return true;
                }
            }

            return false;
        }
    }
}

#endif