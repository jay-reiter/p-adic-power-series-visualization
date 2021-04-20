#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

template <unsigned int p> 
class p_adic {
    private:
        std::vector<unsigned int> x;
        /**
         * returns p_adic object with no digits
         */
        p_adic();

    public:
        /**
         * Creates p-adic number specified by given array.
         * Note that passing {2,3,4,5} will return the p-adic number ...005432
         */
        p_adic(std::vector<unsigned int> x);
        // cpy ctor
        p_adic(const p_adic<p>& other); 
        // dtor
        ~p_adic() {}

        // get the index of highest non-zero digit
        unsigned int size() const;

        p_adic operator+(const p_adic<p>& other) const;
        p_adic operator*(const p_adic<p>& other) const;
        /**
         * Returns the digit in the p^k place value
         */
        unsigned int operator[](std::size_t idx) const;
        unsigned int& operator[](std::size_t k);
};

// since this is a templated class
#include "p_adic.hpp"