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
std::vector<unsigned> p_adic<p>::get_tuple() {
    return this->x;
}

template <unsigned int p>
unsigned int p_adic<p>::size() const {
    return this->x.size();
}

template <unsigned int p>
int p_adic<p>::ord() const {
    int ord = -this->m;
    auto itr = this->x.begin();
    while (itr != this->x.end() && ord < ORD_MAX) {
        if (*itr != 0) return ord;
        ord++;
        itr++;
    }
    return ORD_MAX;
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
    while (a_bar.x[0] == 0) {
        a_bar.x.erase(a_bar.x.begin());
        c++;
    }

    // compute x in 0,1,2,... p-1 so a_bar * x == 1 mod p
    // use FLT, then reduce mod p (get 0th index)
    unsigned x = a_bar.pow(p - 2)[0];
    // first (right-most) digit of remainder
    unsigned w;

    // pre-allocate a vector of size k
    p_adic<p> quotient = p_adic<p>(std::vector<unsigned>(k, 0U));
    // initialize remainder to 1
    p_adic<p> remainder = p_adic<p>(std::vector<unsigned>(k, 0U));
    remainder.x[0] = 1;

    for (unsigned i = 0; i < k; i++) {
        // std::cout << "\nLoop iteration: " << i << "\nRemainder = " << remainder << "\nQuotient = " << quotient << std::endl;

        w = remainder.x[0];
        
        quotient.x[i] = (w * x) % p;

        // std::cout << "Adding " << quotient.x[i] << " to quotient\nSubtracting " << single_digit_multiply(quotient.x[i], a_bar) << " from remainder" << std::endl;

        remainder -= single_digit_multiply(quotient.x[i], a_bar);

        // we have now zeroed out the right-most digit, so move remainder down by factor of p
        remainder.x.erase(remainder.x.begin());

        // TODO: recognize repeat remainders?
    }

    quotient.m = c;
    return quotient;
}

template <unsigned int p>
p_adic<p> p_adic<p>::inv(void) const {
    return this->inv(DIGIT_ACCURACY);
}

template <unsigned int p>
p_adic<p> p_adic<p>::operator+(const p_adic<p>& other) const {
    p_adic sum = p_adic();

    std::size_t a_len = this->x.size();
    std::size_t b_len = other.x.size();

    unsigned int carry_over = 0;
    unsigned int remainder = 0;
    unsigned int s;

    // regular place-value addition
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
    // TODO: if we happen to get a zero in the last digit, we should trim that off so boost performance

    return sum;
}

template <unsigned int p>
p_adic<p>& p_adic<p>::operator+=(const p_adic<p>& rhs) {
    *this = *this + rhs;
    return *this;
}

template <unsigned int p>
p_adic<p> p_adic<p>::single_digit_multiply(const unsigned int a, const p_adic<p>& b) {
    // if a == p, we just add a 0 on the end
    if (a == p) {
        p_adic<p> ret = b;
        ret.x.insert(ret.x.begin(), 0);
        return ret;
    }
    if (a == 0) { return p_adic<p>({0}); }
    
    // otherwise, make sure a is in {1,2,...,p-1}
    assert(0 < a && a < p);

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
    sum.trim_zeros();
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

    return (*this) * negative;
}

template <unsigned int p>
p_adic<p> p_adic<p>::operator-(const p_adic<p>& other) const {
    return *this + (-other);
}

template <unsigned int p>
p_adic<p>& p_adic<p>::operator-=(const p_adic<p>& rhs) {
    *this = *this - rhs;
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

    // aux line to return a_0 if x==0
    if (x.ord() == ORD_MAX) {
        return a(0);
    }

    // initialize sum to 0
    p_adic<p> D = p_adic<p>(std::vector<unsigned>(DIGIT_ACCURACY, 0));
    p_adic<p> d;

    // p_adic<p> one = p_adic<p>({1});

    unsigned n = 0;
    do {
        do {
            d = a(n) * x.pow(n);
            // std::cout << "d=" << d << std::endl;
            n++;
        } while (d.ord() == ORD_MAX);

        
        // if (d.ord() == ORD_MAX /* d == 0 */) {
        //     if (n > MAX_ITER) break;
        //     d += one; // so d will pass the while condition
        // }
        
        D += d;

    } while (d.ord() <= k && n < MAX_ITER);

    return D;
}

template <unsigned int p>
p_adic<p> factorial_inv(unsigned n) {
    // first get n!
    p_adic<p> n_fact = p_adic<p>({1});
    p_adic<p> one = p_adic<p>({1});
    p_adic<p> multiplier = p_adic<p>({2});

    for (unsigned k = 2; k <= n; k++) {
        n_fact *= multiplier;
        multiplier += one;
    }
    // return the inverse
    return n_fact.inv();
}

template <unsigned int p>
p_adic<p> exp(p_adic<p> x, unsigned k) {
    return power_series(x, k, factorial_inv);
}

template <unsigned int p>
p_adic<p> sin(p_adic<p> x, unsigned k) {
    return power_series(x, k, sin_coef);
}

template <unsigned int p>
p_adic<p> cos(p_adic<p> x, unsigned k) {
    return power_series(x, k, cos_coef);
}

template <unsigned int p>
p_adic<p> p_adic<p>::trim_zeros() {
    while (this->x[0] == 0 && this->m > 0) {
        this->x.erase(this->x.begin());
        this->m--;
    }

    return *this;
}

template <unsigned int p>
p_adic<p> p_adic<p>::increment_back() {
    for (int i = this->size() - 1; i >= 0; i--) {
        this->x[i]++;
        if (this->x[i] == p) {
            this->x[i] = 0;
        }
        else break;
    }

    return *this;
}

template <unsigned int p>
std::string p_adic<p>::to_string() {
    std::string out = "";
    for (int i = this->size() - 1; i >= 0; i--) {
        // std::cout << "i=" << i << std::endl;
        out += std::to_string(this->x[i]);
    }
    return out;
}

template <unsigned int p>
p_adic<p> sin_coef(unsigned n) {
    if (n % 2 == 0) return p_adic<p>(std::vector<unsigned>(DIGIT_ACCURACY, 0));
    
    p_adic<p> ret = factorial_inv<p>(n);
    if ((n-1)/2 % 2 == 1) { return -ret; }
    return ret;
}

template <unsigned int p>
p_adic<p> cos_coef(unsigned n) {
    if (n % 2 == 1) return p_adic<p>(std::vector<unsigned>(DIGIT_ACCURACY, 0));
    
    p_adic<p> ret = factorial_inv<p>(n);
    if (n/2 % 2 == 1) { return -ret; }
    return ret;
}