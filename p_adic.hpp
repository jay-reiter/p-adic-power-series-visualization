
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
p_adic<p> p_adic<p>::operator*(const p_adic<p>& other) const {
    return p_adic();
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