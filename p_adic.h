#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <cmath>

template <unsigned int p> class p_adic;

template <unsigned int p>
p_adic<p> power_series(p_adic<p> x, unsigned k, p_adic<p> a(unsigned));

template <unsigned int p>
std::ostream& operator<<(std::ostream& out, const p_adic<p>& num);

template <unsigned int p> 
class p_adic {
    private:
        /**
         * stores the place value digits of the p-adic number
         * smallest place value starts at smaller index
         */
        std::vector<unsigned int> x;

        /**
         * stores the number of digits to the right of the decimal point
         * if this is 0, the number is a p-adic integer
         * 
         * this is also the maximal m so that number can be written as yp^(-m)
         * where y is a p-adic integer
         */
        unsigned m;

        /**
         * returns p_adic object with no digits
         */
        p_adic();

        /**
         * gets the index (in the vector) of the highest non-zero digit
         * only for coding reasons; no intentional mathematical use
         */
        unsigned int size() const;

        /**
         * multiplies a p_adic number by a single digit
         * must have 0 <= a <= p
         */
        p_adic single_digit_multiply(const unsigned int a, const p_adic<p>& b) const;

    public:
        /**
         * Creates p-adic integer specified by given array.
         * Note that passing {2,3,4,5} will return the p-adic integer ...005432
         */
        p_adic(std::vector<unsigned int> x);

        /**
         * creates a p-adic number specified by given array and with m "decimal" places
         * Note that passing {2,3,4,5}, 2 will return the p-adic integer ...0054.32
         */
        p_adic(std::vector<unsigned int> x, unsigned m);
        // cpy ctor
        p_adic(const p_adic<p>& other); 
        // dtor
        ~p_adic() {}

        
        // get the p-adic order; index of first non-zero digit, reading from right to left
        int ord() const;
        // get the p-adic norm
        double norm() const;
        // raises to non-negative integer power
        p_adic pow(unsigned int b) const;

        p_adic operator+(const p_adic<p>& other) const;
        p_adic& operator+=(const p_adic<p>& rhs);
        p_adic operator*(const p_adic<p>& other) const;
        p_adic& operator*=(const p_adic<p>& rhs);

        /**
         * Returns the digit in the p^k place value
         */
        unsigned int operator[](std::size_t idx) const;
        unsigned int& operator[](std::size_t k);

        // friend template functions
        friend std::ostream& operator<< <> (std::ostream& out, const p_adic<p>& num);
        friend p_adic<p> power_series <> (p_adic<p> x, unsigned k, p_adic<p> a(unsigned));
};

// since this is a templated class
#include "p_adic.hpp"