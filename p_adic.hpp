
template <unsigned int p>
p_adic<p>::p_adic() {
    this->x = std::vector<unsigned int>({});
}

template <unsigned int p>
p_adic<p>::p_adic(std::vector<unsigned int> x) {
    this->x = x;
}

template <unsigned int p>
p_adic<p>::p_adic(const p_adic<p>& other) {
    this->x = other.x;
}

template <unsigned int p>
unsigned int p_adic<p>::size() const {
    return this->x.size();
}

template <unsigned int p>
unsigned int p_adic<p>::ord() const {
    unsigned int ord = 0;
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
    
    std::size_t b_len = b.size();

    // initialize sum to 0
    p_adic<p> sum = p_adic({0});

    // multiply a by each digit of b, then add p^i that to the sum
    for (unsigned int i = 0; i < b_len; i++) {
        p_adic<p> term = single_digit_multiply((*this)[i], b);

        // multiply term by p^i
        for (unsigned int j = 0; j < i; j++) {
            term = single_digit_multiply(p, term);
        }

        sum += term;
    }
    return sum;
}

template <unsigned int p>
p_adic<p>& p_adic<p>::operator*=(const p_adic<p>& rhs) {
    *this = *this * rhs;
    return *this;
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
    do {
        i--;
        out << num[i];
    } while (i > 0);
    
    return out;
}

<<<<<<< HEAD









=======
/**
 * evaluates the power series given by $\sum_{n\geq0} a_n x^n/n!
 * a_n is a function pointer used to determine the coefficients
 * 
 * estimates up to some element of p^k Z_p
 */
template <unsigned int p>
double power_series(p_adic<p> x, unsigned k, double a(unsigned)) {
    // first figure out how many terms we will need to compute to estimate this up to coset
    

    return 0;
}
>>>>>>> 36824ce92aceae4f129a619abef6c66395b724f9
