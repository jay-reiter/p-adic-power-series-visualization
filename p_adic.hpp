template <unsigned int p>
p_adic<p>::p_adic() {
    this->x = std::vector<unsigned int>({});
    this->m = 0;
}

template <unsigned int p>
p_adic<p>::p_adic(std::vector<unsigned int> x) {
    this->x = x;
    this->m = 0;
}

template <unsigned int p>
p_adic<p>::p_adic(std::vector<unsigned int> x, unsigned m) {
    this->x = x;
    this->m = m;
}

template <unsigned int p>
p_adic<p>::p_adic(const p_adic<p>& other) {
    this->x = other.x;
    this->m = other.m;
}

template <unsigned int p>
unsigned int p_adic<p>::size() const {
    return this->x.size();
}

template <unsigned int p>
int p_adic<p>::ord() const {
    int ord = -this->m;
    auto itr = this->x.begin();
    while (itr != this->x.end()) {
        if (*itr != 0) return ord;
        ord++;
        itr++;
    }
    return INFINITY;
}

template <unsigned int p>
double p_adic<p>::norm() const {
    return std::pow(p, -1. * this->ord());    
}

template <unsigned int p>
p_adic<p> p_adic<p>::inv(unsigned k) const {
    // make sure we have been passed a p-adic integer!
    assert(this->m == 0);
    
    // we use long division to compute 1/a

    // first convert a to a unit in Z_p:
    p_adic<p> a_bar = p_adic<p>(*this);
    unsigned c = 0;
    std::cout << "a_bar=" << a_bar << std::endl;
    while (a_bar.x[0] == 0) {
        a_bar.x.erase(a_bar.x.begin());
        c++;
    }

    // compute x in 0,1,2,... p-1 so a_bar * x == 1 mod p
    // use FLT, then reduce mod p (get 0th index)
    unsigned x = a_bar.pow(p - 2)[0];
    // first non-zero digit of remainder
    unsigned w;

    // pre-allocate a vector of size k
    p_adic<p> quotient = p_adic<p>(std::vector<unsigned>(k, 0U));
    // initialize remainder to 1
    p_adic<p> remainder = p_adic<p>({DIGIT_ACCURACY, 0U});
    remainder.x[0] = 1;

    std::cout << "Remainder = " << remainder << std::endl;

    for (unsigned i = 0; i < k; i++) {
        // get first nonzero digit of remainder
        unsigned j = 0;
        do {
            w = remainder.x[j];
            j++;
        } while (w == 0);

        quotient.x[i] = (w * x) % p;
        remainder -= single_digit_multiply((w * x) % p, a_bar);

        // we have now zeroed out the first digit, so move remainder down
        remainder.x.erase(remainder.x.begin());

        // TODO: recognize repeat remainders?
    }

    return quotient;
} 

template <unsigned int p>
p_adic<p> p_adic<p>::operator+(const p_adic<p>& other) const {
    p_adic sum = p_adic();

    std::size_t a_len = this->x.size();
    std::size_t b_len = other.x.size();

    unsigned int carry_over = 0;
    unsigned int remainder = 0;
    unsigned int s;

    for (unsigned int i = 0; i < std::min(a_len, b_len); i++) {
        s = (*this)[i] + other[i] + carry_over;
        remainder = s % p;
        carry_over = (s >= p);
        sum.x.push_back(remainder);
    }
    if (a_len != b_len) {
        for (unsigned int i = std::min(a_len, b_len); i < std::max(a_len, b_len); i++) {
            s = carry_over + (a_len<b_len ? other[i] : (*this)[i]);
            remainder = s % p;
            carry_over = (s >= p);
            sum.x.push_back(remainder);
        }
    } else {
        if (carry_over) { sum.x.push_back(1); }
    }

    sum.m = std::max(this->m, other.m);
    // if we happen to get a zero in the last digit, we should trim that off so boost performance

    return sum;
}

template <unsigned int p>
p_adic<p>& p_adic<p>::operator+=(const p_adic<p>& rhs) {
    *this = *this + rhs;
    return *this;
}

template <unsigned int p>
p_adic<p> p_adic<p>::single_digit_multiply(const unsigned int a, const p_adic<p>& b) const {
    // if a == p, we just add a 0 on the end
    if (a == p) {
        p_adic<p> ret = b;
        ret.x.insert(ret.x.begin(), 0);
        return ret;
    }
    // otherwise, make sure a is in {0,1,2,...,p-1}
    assert(0 <= a && a < p);

    unsigned int carry_over = 0;
    unsigned int remainder = 0;
    unsigned int s;

    p_adic<p> ret = p_adic();
    for (unsigned int i = 0; i < b.size(); i++) {
        s = a * b[i] + carry_over;
        remainder = s % p;
        carry_over = (s - remainder) / p;
        ret.x.push_back(remainder);
    }
    if (carry_over) {
        ret.x.push_back(carry_over);
    }

    return ret;
}

template <unsigned int p>
p_adic<p> p_adic<p>::operator*(const p_adic<p>& b) const {

    // initialize sum to 0
    p_adic<p> sum = p_adic({0});

    // multiply a by each digit of b, then add p^i that to the sum
    for (unsigned int i = 0; i < this->size(); i++) {
        p_adic<p> term = single_digit_multiply((*this)[i], b);

        // multiply term by p^i
        for (unsigned int j = 0; j < i; j++) {
            term = single_digit_multiply(p, term);
        }

        sum += term;
    }

    // adjust number of decimal places
    sum.m = this->m + b.m;
    return sum;
}

template <unsigned int p>
p_adic<p>& p_adic<p>::operator*=(const p_adic<p>& rhs) {
    *this = *this * rhs;
    return *this;
}

template <unsigned int p>
p_adic<p> p_adic<p>::operator-() const {
    // construct p-adic -1 to the same number of digits of this
    p_adic<p> negative = p_adic(std::vector<unsigned>(DIGIT_ACCURACY, p - 1), 0);
    return negative * (*this);
}

template <unsigned int p>
p_adic<p> p_adic<p>::operator-(const p_adic<p>& other) const {
    // std::cout << "this=" << *this << " other = " << other << " -other = " << -other << "\nTheir sum is: this+(-other)= " << ((*this) + (-other)) << std::endl;
    // return p_adic<p>({0});
    return *this + (-other);
}

template <unsigned int p>
p_adic<p>& p_adic<p>::operator-=(const p_adic<p>& rhs) {
    p_adic<p> ret = *this - rhs;
    return ret;
}

template <unsigned int p>
p_adic<p> p_adic<p>::pow(unsigned int b) const {
    p_adic<p> ret = p_adic({1});
    for (int i = 0; i < b; i++) ret *= *this;
    return ret;
}

template <unsigned int p>
unsigned int p_adic<p>::operator[](std::size_t k) const {
    return this->x[k];
}

template <unsigned int p>
unsigned int& p_adic<p>::operator[](std::size_t k) {
    return this->x.at(k);
}

template <unsigned int p>
std::ostream& operator<<(std::ostream& out, const p_adic<p>& num) {
    out << "...00";
    
    unsigned i = num.size();

    // if this number looks like 0.000002193 then print out some leading zeros
    if (num.m > num.size()) {
        out << "0.";
        for (unsigned j = 0; j < num.m - num.size(); j++) {
            out << "0";
        }
    }

    // otherwise just print the number as usual
    do {
        if (i == num.m) out << ".";

        i--;
        out << num[i];
    } while (i > 0);
    
    return out;
}

/**
 * evaluates the power series given by $\sum_{n\geq0} a_n x^n/n!
 * a_n is a function pointer used to determine the coefficients
 * 
 * estimates up to some element of p^k Z_p
 */
template <unsigned int p>
p_adic<p> power_series(p_adic<p> x, unsigned k, p_adic<p> a(unsigned)) {
    // first figure out how many terms we will need to compute to estimate this up to coset
    /**
     * if we want to be accurate up to a term a_n x^n in p^kZ_p, we must have
     * ord(a_nx^n)= ord(a_n) - ord(x^n) >= k
     * 
     * in the special case where a_n = 1/n!, we have ord(x^n/n!) = ord(x^n) - n + s(n)
     * where s(n) denotes the digit sum of the p-adic integer n
     */

    // initialize sum to 0
    p_adic<p> D = p_adic<p>({0});
    p_adic<p> d;

    unsigned n = 0;
    do {
        d = a(n) * x.pow(n);
        D += d;
        n++;
    } while (d.ord() >= k);

    return 0;
}
