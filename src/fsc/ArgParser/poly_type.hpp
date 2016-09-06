// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    19.08.2015 09:44:17 CEST
// File:    poly_type.cpp

#ifndef FSC_POLY_TYPE_HEADER
#define FSC_POLY_TYPE_HEADER

#include "boost_any.hpp"
#include "fsc_except.hpp"

#include <map>
#include <iomanip>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#include <type_traits>

/* TODO:
 * default ctor in reject conv
 */

namespace fsc {
    /// @cond NEVER_DOCUMENT_THIS_ENTITY
    namespace detail {
        template<typename T>
        struct convert {
            template<typename U>
            static typename std::enable_if<std::is_convertible<U, T>::value, T>::type from(U const & u) {
                return u;
            }
            template<typename U>
            static typename std::enable_if<!std::is_convertible<U, T>::value, T>::type from(U const &) {
                throw fsc::O__o("poly_type error: type '" + std::string(typeid(U).name()) + "' is not convertible to type '" + std::string(typeid(T).name()) + "'");
                return T(); //TODO, remove default ctor req.
            }
        };
    }//end namespace detail
    /// @endcond
    
    /// \cond IMPLEMENTATION_DETAIL_DOC
    struct poly_type {
        private:
            // This class has a templated cast operator that works nicely, but
            // has ambiguity problems with std::string since it has so many 
            // ctors... hence we drop all cast operators that trouble std::string 
            template<typename T, typename U>
            struct drop_casts {
                static constexpr bool value = true;
            };
            //~ template<typename U>
            //~ struct drop_casts<size_t, U> {
                //~ static constexpr bool value = false;
            //~ };
            template<typename U>
            struct drop_casts<std::initializer_list<char>, U> {
                static constexpr bool value = false;
            };
            template<typename U>
            struct drop_casts<std::allocator<char>, U> {
                static constexpr bool value = false;
            };
            template<typename U>
            struct drop_casts<const char*, U> {
                static constexpr bool value = false;
            };
        public:
        //------------------- structors -------------------
        poly_type() {}
        template<typename T>
        poly_type(T const & t): any(t) {}
        poly_type(char const * t): any(std::string(t)) {}
        //------------------- cast -------------------
        template<typename T, typename enable = std::enable_if_t<drop_casts<T, void>::value>>
        operator T() const {
            if(type() == typeid(int)) {
                return detail::convert<T>::from(boost::any_cast<int>(any));
            }
            if(type() == typeid(double)) {
                return detail::convert<T>::from(boost::any_cast<double>(any));
            }
            if(type() == typeid(bool)) {
                return detail::convert<T>::from(boost::any_cast<bool>(any));
            }
            if(type() == typeid(std::string)) {
                return detail::convert<T>::from(boost::any_cast<std::string>(any));
            }
            return T();
        }
        //------------------- assign -------------------
        template<typename T>
        void operator=(T const & t){
            any = t;
        }
        void operator=(const char * t) { // handle spaecial case since "bla" is not a string
            any = std::string(t);
        }
        void operator=(poly_type const & t){
            any = t.any;
        }
        
        //------------------- += -= *= /= operator -------------------
        template<typename T>
        poly_type & operator+=(T const & t) {
            return (*this) += poly_type(t);
        }
        poly_type & operator+=(char const * t) {
            return (*this)+=std::string(t);
        }
        poly_type & operator+=(poly_type const & t) {
            if(type() == typeid(std::string)) {
                std::string as = (*this);
                std::string bs = t;
                any = as+bs;
            } else if(type() == typeid(double) or t.type() == typeid(double)) {
                double as = (*this);
                double bs = t;
                any = as+bs;
            } else {
                int as = (*this);
                int bs = t;
                any = as+bs;
            }
            return (*this);
        }
        
        template<typename T>
        poly_type & operator-=(T const & t) {
            return (*this) -= poly_type(t);
        }
        poly_type & operator-=(poly_type const & t) {
            if(type() == typeid(double) or t.type() == typeid(double)) {
                double as = (*this);
                double bs = t;
                any = as-bs;
            } else {
                int as = (*this);
                int bs = t;
                any = as-bs;
            }
            return (*this);
        }
        
        template<typename T>
        poly_type & operator*=(T const & t) {
            return (*this) *= poly_type(t);
        }
        poly_type & operator*=(poly_type const & t) {
            if(type() == typeid(double) or t.type() == typeid(double)) {
                double as = (*this);
                double bs = t;
                any = as*bs;
            } else {
                int as = (*this);
                int bs = t;
                any = as*bs;
            }
            return (*this);
        }
        
        template<typename T>
        poly_type & operator/=(T const & t) {
            return (*this) /= poly_type(t);
        }
        poly_type & operator/=(poly_type const & t) {
            double as = (*this);
            double bs = t;
            any = as/bs;
            return (*this);
        }
        
        //------------------- const fct -------------------
        std::type_info const & type() const {
            return any.type();
        }
        template<typename S>
        void print(S & os) const {
            if(type() == typeid(bool)) {
                os << boost::any_cast<bool>(any);
            }
            if(type() == typeid(int)) {
                os << boost::any_cast<int>(any);
            }
            if(type() == typeid(double)) {
                os << boost::any_cast<double>(any);
            }
            if(type() == typeid(std::string)) {
                os << boost::any_cast<std::string>(any);
            }
        }
    private:
        boost::any any;
    };

    std::ostream & operator<<(std::ostream & os, poly_type const & arg) {
        arg.print(os);
        return os;
    }

    #define FSC_POLY_TYPE_OP_SUPPORT(OP)                                \
    poly_type operator OP(poly_type const & a, poly_type const & b) {   \
        poly_type res(a);                                               \
        res OP##= b;                                                    \
        return res;                                                     \
    }                                                                   \
    template<typename T>                                                \
    poly_type operator OP(poly_type const & p, T const & t) {           \
        return p OP poly_type(t);                                       \
    }                                                                   \
    template<typename T>                                                \
    poly_type operator OP(T const & t, poly_type const & p) {           \
        return poly_type(t) OP p;                                       \
    }                                                                   //
    
    
    #define FSC_POLY_TYPE_OPEQ_SUPPORT(OP, TYPE)                        \
    TYPE & operator OP(TYPE & a, fsc::poly_type const & b) {            \
        TYPE t = b;                                                     \
        return a OP t;                                                  \
    }                                                                   //
    
    FSC_POLY_TYPE_OP_SUPPORT(+)
    FSC_POLY_TYPE_OP_SUPPORT(-)
    FSC_POLY_TYPE_OP_SUPPORT(/)
    FSC_POLY_TYPE_OP_SUPPORT(*)
    FSC_POLY_TYPE_OPEQ_SUPPORT(+=, std::string)
    FSC_POLY_TYPE_OPEQ_SUPPORT(+=, double)
    FSC_POLY_TYPE_OPEQ_SUPPORT(-=, double)
    FSC_POLY_TYPE_OPEQ_SUPPORT(*=, double)
    FSC_POLY_TYPE_OPEQ_SUPPORT(/=, double)
    FSC_POLY_TYPE_OPEQ_SUPPORT(+=, int)
    FSC_POLY_TYPE_OPEQ_SUPPORT(-=, int)
    FSC_POLY_TYPE_OPEQ_SUPPORT(*=, int)
    FSC_POLY_TYPE_OPEQ_SUPPORT(/=, int)

    #undef FSC_POLY_TYPE_OP_SUPPORT
    #undef FSC_POLY_TYPE_OPEQ_SUPPORT
    
    /// \endcond
}//end namespace fsc


/*
int main(int argc, char* argv[]) {
    std::map<std::string, fsc::poly_type> f;
    
    f["cwd"] = "./";
    f["cwd"] += "home";
    f["b"] = true;
    
    f["i"] = 1;
    f["i"] -= .1;
    
    f["d"] = 1.3;
    f["d"] = f["d"] + 1;
    
    f["i*d"] = f["i"]*f["d"];
    
    f["i*d/(2*b)"] = f["i*d"]/(2*f["b"]);
    
    f["x"] = "foo";
    f["y"] = f["x"] + "/bar";
    f["x"] = "fo1";
    f["bar"] = f["x"];
    f["x"] = 1.2;
    f["x"] = "d";
    
    
    std::string str("f");
    fsc::poly_type temp("foo");
    
    str+=temp;
    
    for(auto const & p: f)
        std::cout << p.first << " = " << p.second << std::endl;
    
    
    return 0;
}
*/
#endif //FSC_POLY_TYPE_HEADER

