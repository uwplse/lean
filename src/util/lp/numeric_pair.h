/*
  Copyright (c) 2013 Microsoft Corporation. All rights reserved.
  Released under Apache 2.0 license as described in the file LICENSE.

  Author: Lev Nachmanson
  The idea is that it is only one different file in Lean and z3 source inside of LP
*/
#pragma once
// #define lp_for_z3
#include <string>
#include <cmath>
#include <algorithm>
#ifdef lp_for_z3
#include "../rational.h"
#include "../sstream.h"
#include "../z3_exception.h"

#else
#include "util/numerics/mpq.h"
#include "util/numerics/numeric_traits.h"
#endif
namespace lean {
#ifdef lp_for_z3 // rename rationals
    typedef rational mpq;
#else
    typedef lean::mpq mpq;
#endif


template <typename T>
std::string T_to_string(const T & t); // forward definition
#ifdef lp_for_z3
template <typename T> class numeric_traits {};

template <>  class numeric_traits<double> {
    public:
        static bool precise() { return false; }
        static double g_zero;
        static double const &zero() { return g_zero;  }
        static double g_one;
        static double const &one() { return g_one; }
        static bool is_zero(double v) { return v == 0.0; }
        static double const & get_double(double const & d) { return d;}
        static double log(double const & d) { NOT_IMPLEMENTED_YET(); return d;}
        static double from_string(std::string const & str) { return atof(str.c_str()); }
    };

    template<>
    class numeric_traits<rational> {
    public:
        static bool precise() { return true; }
        static rational const & zero() { return rational::zero(); }
        static rational const & one() { return rational::one(); }
        static bool is_zero(const rational & v) { return v.is_zero(); }
        static double const  get_double(const rational  & d) { return d.get_double();}
        static rational log(rational const& r) { UNREACHABLE(); return r; }
        static rational from_string(std::string const & str) { return rational(str.c_str()); }
    };
#endif

template <typename X, typename Y>
struct convert_struct {
    static X convert(const Y & y){ return X(y);}
    static bool is_epsilon_small(const X & x,  const double & y) { return std::abs(numeric_traits<X>::get_double(x)) < y; }
    static bool below_bound_numeric(const X &, const X &, const Y &) { /*lean_unreachable();*/ return false;}
    static bool above_bound_numeric(const X &, const X &, const Y &) { /*lean_unreachable();*/ return false; }
};


template <>
struct convert_struct<double, mpq> {
    static double convert(const mpq & q) {return q.get_double();}
};

template <typename T>
struct numeric_pair {
    T x;
    T y;
    // empty constructor
    numeric_pair() {}
    // another constructor

    numeric_pair(T xp, T yp) : x(xp), y(yp) {}
    template <typename X, typename Y> numeric_pair(X xp, Y yp) : numeric_pair(convert_struct<T, X>::convert(xp), convert_struct<T, Y>::convert(yp)) {}

    bool operator<(const numeric_pair& a) const {
        return x < a.x || (x == a.x && y < a.y);
    }

    bool operator>(const numeric_pair& a) const {
        return x > a.x || (x == a.x && y > a.y);
    }

    bool operator==(const numeric_pair& a) const  {
        return a.x == x &&  a.y == y;
    }

    bool operator!=(const numeric_pair& a) const  {
        return !(*this == a);
    }

    bool operator<=(const numeric_pair& a) const  {
        return *this < a || *this == a;
    }

    bool operator>=(const numeric_pair& a) const  {
        return *this > a || a == *this;
    }

    numeric_pair operator*(const T & a) const {
        return numeric_pair(a * x, a * y);
    }

    numeric_pair operator/(const T & a) const {
        T a_as_T(a);
        return numeric_pair(x / a_as_T, y / a_as_T);
    }

    numeric_pair operator/(const numeric_pair &) const {
        //        lean_unreachable();
    }


    numeric_pair operator+(const numeric_pair & a) const  {
        return numeric_pair(a.x + x, a.y + y);
    }

    numeric_pair operator*(const numeric_pair & /*a*/) const  {
        //        lean_unreachable();
    }

    numeric_pair&  operator+=(const numeric_pair & a) {
        x += a.x;
        y += a.y;
        return *this;
    }

    numeric_pair&  operator-=(const numeric_pair & a) {
        x -= a.x;
        y -= a.y;
        return *this;
    }

    numeric_pair&  operator/=(const T & a) {
        x /= a;
        y /= a;
        return *this;
    }

    numeric_pair&  operator*=(const T & a) {
        x *= a;
        y *= a;
        return *this;
    }

    numeric_pair operator-(const numeric_pair & a) const {
        return numeric_pair(x - a.x, y - a.y);
    }

    numeric_pair operator-() const {
        return numeric_pair(-x, -y);
    }

    static bool precize() { return lean::numeric_traits<T>::precize();}

    std::string to_string() const { return std::string("(") + T_to_string(x) + ", "  + T_to_string(y) + ")"; }
};


template <typename T>
std::ostream& operator<<(std::ostream& os, numeric_pair<T> const & obj) {
    os << obj.to_string();
    return os;
}

template <typename T, typename X>
numeric_pair<T> operator*(const X & a, const numeric_pair<T> & r) {
    return numeric_pair<T>(a * r.x, a * r.y);
}

template <typename T, typename X>
numeric_pair<T> operator*(const numeric_pair<T> & r, const X & a) {
    return numeric_pair<T>(a * r.x, a * r.y);
}

} // close namespace lean

namespace lean {
// template <numeric_pair, typename T>  bool precise() { return numeric_traits<T>::precise();}
template <typename T> double get_double(const lean::numeric_pair<T> & ) { /* lean_unreachable(); */ return 0;}
template <typename T>
class numeric_traits<lean::numeric_pair<T>> {
  public:
    static bool precise() { return numeric_traits<T>::precise();}
    static lean::numeric_pair<T> zero() { return lean::numeric_pair<T>(numeric_traits<T>::zero(), numeric_traits<T>::zero()); }
    static bool is_zero(const lean::numeric_pair<T> & v) { return numeric_traits<T>::is_zero(v.x) && numeric_traits<T>::is_zero(v.y); }
    static double get_double(const lean::numeric_pair<T> & v){ return numeric_traits<T>::get_double(v.x); } // just return the double of the first coordinate
    static double one() { /*lean_unreachable();*/ return 0;}
};
} // close namespace lean

namespace lean {
template <>
struct convert_struct<double, numeric_pair<double>> {
    static double convert(const numeric_pair<double> & q) {return q.x;}
};

template <typename X> bool is_epsilon_small(const X & v, const double& eps);   // forward definition { return convert_struct<X, double>::is_epsilon_small(v, eps);}

template <typename T>
struct convert_struct<numeric_pair<T>, double> {
    static numeric_pair<T> convert(const double & q) {
        return numeric_pair<T>(convert_struct<T, double>::convert(q), numeric_traits<T>::zero());
    }
    static bool is_epsilon_small(const numeric_pair<T> & p, const double & eps) {
        return convert_struct<T, double>::is_epsilon_small(p.x, eps) && convert_struct<T, double>::is_epsilon_small(p.y, eps);
    }
    static bool below_bound_numeric(const numeric_pair<T> &, const numeric_pair<T> &, const double &) {
        // lean_unreachable();
        return false;
    }
    static bool above_bound_numeric(const numeric_pair<T> &, const numeric_pair<T> &, const double &) {
        // lean_unreachable();
        return false;
    }
};
template <>
struct convert_struct<numeric_pair<double>, double> {
    static numeric_pair<double> convert(const double & q) {
        return numeric_pair<double>(q, 0.0);
    }
    static bool is_epsilon_small(const numeric_pair<double> & p, const double & eps) {
        return std::abs(p.x) < eps && std::abs(p.y) < eps;
    }

    static int compare_on_coord(const double & x, const double & bound, const double eps) {
        if (bound == 0) return (x < - eps)? -1: (x > eps? 1 : 0); // it is an important special case
        double relative = (bound > 0)? - eps: eps;
        return (x < bound * (1.0 + relative) - eps)? -1 : ((x > bound * (1.0 - relative) + eps)? 1 : 0);
    }

    static bool below_bound_numeric(const numeric_pair<double> & x, const numeric_pair<double> & bound, const double & eps) {
        int r = compare_on_coord(x.x, bound.x, eps);
        if (r == 1) return false;
        if (r == -1) return true;
        // the first coordinates are almost the same
        return compare_on_coord(x.y, bound.y, eps) == -1;
    }

    static bool above_bound_numeric(const numeric_pair<double> & x, const numeric_pair<double> & bound, const double & eps) {
        int r = compare_on_coord(x.x, bound.x, eps);
        if (r == -1) return false;
        if (r ==  1) return true;
        // the first coordinates are almost the same
        return compare_on_coord(x.y, bound.y, eps) == 1;
    }
};

template <>
struct convert_struct<double, double> {
    static bool is_epsilon_small(const double& x, const double & eps) {
        return x < eps && x > -eps;
    }
    static double convert(const double & y){ return y;}
    static bool below_bound_numeric(const double & x, const double & bound, const double & eps) {
        if (bound == 0) return x < - eps;
        double relative = (bound > 0)? - eps: eps;
        return x < bound * (1.0 + relative) - eps;
    }
    static bool above_bound_numeric(const double & x, const double & bound, const double & eps) {
        if (bound == 0) return x > eps;
        double relative = (bound > 0)?  eps: - eps;
        return x > bound * (1.0 + relative) + eps;
    }
};

template <typename X> bool is_epsilon_small(const X & v, const double &eps) { return convert_struct<X, double>::is_epsilon_small(v, eps);}
template <typename X> bool below_bound_numeric(const X & x, const X & bound, const double& eps) { return convert_struct<X, double>::below_bound_numeric(x, bound, eps);}
template <typename X> bool above_bound_numeric(const X & x, const X & bound, const double& eps) { return convert_struct<X, double>::above_bound_numeric(x, bound, eps);}
}
