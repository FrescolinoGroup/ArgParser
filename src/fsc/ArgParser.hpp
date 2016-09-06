/** ****************************************************************************
 * 
 * \file       ArgParser.hpp
 * \brief      ArgParser header-only lib.
 * >           A command line argument parser that does not require registration.
 * >           
 * \version    2015.0818
 * Year      | Name
 * --------: | :------------
 * 2014-2016 | Mario Könz
 *    2015   | C. Frescolino
 * \copyright  See LICENCE
 */

/* TODO
 * str_to_type_ -> make -B pretty -j3
 * doc
 * 
 * implement split_clean()
 * 
 * V2:
 * contract pwd nicely!
 * struct converter {
 *   std::vector<double> apply(std::string const &);
 *   std::vector<double> apply(double const &); //check perhaps ArgParser<int_conv, double_conv, string_conv, custom_conv>
 * 
 * ap.get<converter>("x", std::vector<double>({1,2,3,4}))
 * 
 * 
 */

#ifndef FSC_ARGPARSER_HEADER
#define FSC_ARGPARSER_HEADER

#include "ArgParser/poly_type.hpp"
#include "ArgParser/boost_any.hpp"
#include "ArgParser/fsc_except.hpp"

#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <unistd.h>
#include <algorithm>

namespace fsc {
    /// @cond NEVER_DOCUMENT_THIS_ENTITY
    namespace detail {
        std::string get_progname(std::string pwd_name) {
            auto pos = pwd_name.rfind("/");
            if(pos == std::string::npos)
                return pwd_name;
            
            pwd_name.erase(0, pos+1);
            return pwd_name;
        }
        std::string get_pwd(std::string pwd_name, std::string const & cwd) {
            auto pos = pwd_name.rfind("/");
            if(pos == std::string::npos)
                return cwd;
            
            pwd_name.erase(pos, pwd_name.size());
            
            return cwd + "/" + pwd_name;
        }
        std::vector<std::string> split_clean(std::string const & str) {
            std::stringstream iss(str);
            return std::vector<std::string>{std::istream_iterator<std::string>{iss}
                                          , std::istream_iterator<std::string>{}};
        }
    }//end namespace detail
    /// @endcond
    
    /** @brief an argument parser that does not require registration.
    * 
    * ### Usage:
    * For details and more examples, see the fsc::ArgParser member function
    * documentation.
    * \include mixed_example.cpp
    */
    class ArgParser {
        //~ using poly_type = __future_impl;
        using map_type = std::map<std::string, poly_type>; ///< 
        template<typename U>
        using vec_type = std::vector<U>;
        using size_type = vec_type<poly_type>::size_type; ///< std::vector<poly_type>::size_type
    public:
        //------------------- structors -------------------
        /**@brief default constructor
         * 
         * does not generate pdw and progname.
         */
        ArgParser() noexcept: cwd_(getcwd(NULL, 0))
                            , pwd_("")
                            , progname_("") {
        }
        /**@brief construct form command line
         * @param argc length of argv
         * @param argv array of arguments given via command line
         * 
         * the only constructor that can set pwd and progname since it can be extracted from argv[0].
         * Throws a std::runtime_error if the command line is ill-formed.
         */
        ArgParser(int const & argc, char * argv[]): cwd_(getcwd(NULL, 0))
                                                  , pwd_(detail::get_pwd(argv[0], cwd_))
                                                  , progname_(detail::get_progname(argv[0])) {
            vec_type<std::string> arg;
            for(int i = 1; i < argc; ++i) // 1 since we dont need the progname
                arg.push_back(argv[i]);
            parse_(arg);
        }
        /**@brief string constructor
         * @param cline a string that simulates a command line
         * 
         * construct the ArgParser with a string instead of the command line to parse.
         * Throws a std::runtime_error if the string is ill-formed.
         * Does not generate pwd and progname.
         */
        explicit ArgParser(std::string const & cline): cwd_(getcwd(NULL, 0)) 
                                            , pwd_("")
                                            , progname_("") {
            parse_(detail::split_clean(cline));
        }
        //------------------- const getter -------------------
        /**@brief get named argument if set
         * @param key is the name of the desired named argument
         * 
         * If `key` is not found, a std::runtime_error is thrown.
         */
        poly_type const & operator[](std::string const & key) const {
            if(n_args_.find(key) == n_args_.end())
                throw std::runtime_error("ArgParser error: named argument '" + key + "' not found");
            return n_args_.at(key);
        }
        /**@brief get free argument if set
         * @param key position of the free argument
         * 
         * If `key >= freeargc()` a std::runtime_error is thrown, i.e.
         * there is no free argument at the position `key`
         */
        poly_type const & operator[](size_type const & key) const {
            if(key >= freeargc())
                throw std::runtime_error("ArgParser error: free argument '" + std::to_string(key) + "' not found");
            return f_args_[key];
        }
        /**@brief get named argument if set, else a default
         * @param key name of the named argument
         * @param def is a default that is returned if `key` is not found
         * 
         * If `key` is found but not convertibly to `decltype(def)`, 
         * a std::runtime_error is thrown.
         */
        template<typename T>
        T get(std::string const & key, T const & def) const {
            if(innamed_(key))
                return n_args_.at(key);
            else
                return def;
        }
        /**@brief special treatment of `char const *`
         * @param key name of the named argument
         * @param def is a default that is returned if `key` is not found
         * 
         * If `key` is found but not convertibly to `std::string`,
         * a std::runtime_error is thrown.
         */
        std::string get(std::string const & key, char const * def) const {
            return get<std::string>(key, def);
        }
        /**@brief get free argument if set, else a default
         * @param pos position of the free argument
         * @param def is a default that is returned if there is no free 
         * argument at position `pos`
         * 
         * If the free argument at `pos` is found but not convertibly to 
         * `decltype(def)`, a std::runtime_error is thrown.
         */
        template<typename T>
        T get(size_type const & pos, T const & def) const {
            if(is_set(pos))
                return f_args_.at(pos);
            else
                return def;
        }
        /**@brief special treatment of `char const *`
         * @param pos position of the free argument
         * @param def is a default that is returned if there is no free 
         * argument at position `pos`
         * 
         * If `pos` is found but not convertibly to `std::string`,
         * a std::runtime_error is thrown.
         */
        std::string get(size_type const & pos, char const * def) const {
            return get<std::string>(pos, def);
        }
        /**@brief returns the current working directory
         * 
         * the `cwd` should always be set
         */
        std::string const & cwd() const noexcept {
            return cwd_;
        }
        /**@brief returns the program working directory
         * 
         * Only works if the `pwd` is set, which only happens in the command 
         * line constructor. If not set, a std::runtime_error is thrown.
         */
        std::string const & pwd() const {
            if(pwd_ == "")
                throw std::runtime_error("ArgParser error: pwd not set");
            return pwd_;
        }
        /**@brief returns the program name
         * 
         * Only works if the `progname` is set, which only happens in the
         * command line constructor. If not set, a std::runtime_error is thrown.
         */
        std::string const & progname() const {
            if(progname_ == "")
                throw std::runtime_error("ArgParser error: progname not set");
            return progname_;
        }
        /**@brief returns the number of free arguments
         */
        size_type freeargc() const noexcept {
            return f_args_.size();
        }
        /**@brief check if flag or named argument is set
         * @param key is a named parameter or flag
         * 
         * returns true if `key` is a flag or named argument. Note that it 
         * is not allowed to have a flag and named argument with the same name.
         */
        bool is_set(std::string const & key) const noexcept {
            return inflag_(key) or innamed_(key);
        }
        /**@brief check if free argument is set
         * @param pos is the position of a potential free argument
         * 
         * returns true if `pos` < `freeargc()`.
         */
        bool is_set(size_type const & pos) const noexcept {
            return pos < freeargc();
        }
        //------------------- modifier -------------------
        /**@brief set a named argument if not already set
         * @param key name of the named argument
         * @param def is a default that is set if there is no named
         * argument `key`
         * 
         * If there is already a flag with name `key` a std::runtime_error 
         * is thown.
         */
        template<typename T>
        void def(std::string const & key, T const & def) {
            if(!innamed_(key))
                setnamed_(key, poly_type(def));
        }
        /**@brief set a flag
         * @param key name of the flag
         * 
         * If there is already a named argument with name `key` a
         * std::runtime_error is thown.
         */
        void def(std::string const & key) { // for O3
            setflag_(key);
        }
        
        /**@brief merge two argument parser
         * @param rhs is the argument parser that is merged into `this` one.
         * @param overwrite defines the merging-behavior on conflicts
         *  
         * The merging policy works as follows:
         * 
         * * named arguments are merged, on conflict take `rhs` if 
         * `overwrite == true` (a std::runtime_error is thrown if there is a 
         * flag - named argument collision)
         * * the flags are merged (a std::runtime_error is thrown if there is a 
         * flag - named argument collision)
         * * free arguments are NOT merged in any way, since the position is 
         * important. If `overwrite == true` the free arguments are overwritten 
         * by the free arguments of `rhs`, otherwise nothing is done.
         * * cwd, pwd and progname are also overwritten by `rhs` if 
         * `overwrite == true`, otherwise nothing is done.
         * 
         */
        void merge(ArgParser const & rhs, bool const & overwrite = true) noexcept {
            //------------------- n_args_ -------------------
            for(auto const & it: rhs.n_args_) {
                std::string key = it.first;
                poly_type val = it.second;
                setnamed_(key, val, overwrite);
            }
            //------------------- f_args_ -------------------
            if(overwrite) {
                f_args_ = rhs.f_args_;
            }
            //------------------- flags_ -------------------
            for(auto const & it: rhs.flags_) {
                setflag_(it);
            }
            //------------------- special member -------------------
            if(overwrite) {
                cwd_ = rhs.cwd_;
                pwd_ = rhs.pwd_;
                progname_ = rhs.progname_;
            }
        }
        /**@brief read a file, parse the content and merge into `this`
         * @param filename is the name of the input file
         * @param overwrite defines the merging-behavior on conflicts
         *  
         * the file is loaded an parsed in a seperate ArgParser inastance an 
         * the merged into `this`. For the mergin behavior see \link 
         * fsc::ArgParser::merge merge \endlink.
         */
        bool parse_file(std::string const & filename, bool const & overwrite = true) {
            vec_type<std::string> arg;
            std::ifstream ifs(filename, std::ios_base::in);
            std::string res = "";
            if(ifs.is_open()) {
                std::stringstream buffer;
                buffer << ifs.rdbuf();
                res = buffer.str();
                ifs.close();
            } else
                return false;
            
            merge(ArgParser(res), overwrite);
            return true;
        }
        //------------------- const fct -------------------
        /**@brief prints the argument parser in a verbose form
         * @param os an std::ostream like object
         */
        template<typename S>
        void print(S & os) const noexcept {
            os << "=======ArgParser=======" << std::endl;
            os << "free_args:" << std::endl;
            for(auto const & it: f_args_)
                os << "    " << it << std::endl;
            os << "named_args:" << std::endl;
            for(auto const & it: n_args_)
                os << "    " << it.first << " \t" << it.second << std::endl;
            os << "flags:" << std::endl;
            for(auto const & it: flags_)
                os << "    " << it << std::endl;
            os << "special member:" << std::endl;
            os << "    cwd \t" << cwd_ << std::endl;
            os << "    pwd \t" << pwd_ << std::endl;
            os << "    progname \t" << progname_;
        }
    private:
        enum arg_type {named_m_stick, named_m, named_mm, named_eq, free, flag_m, flag_mm, named_m_eq, named_mm_eq, invalid};
        inline bool inflag_(std::string const & flag) const noexcept {
            return std::find(flags_.begin(), flags_.end(), flag) != flags_.end();
        }
        inline bool innamed_(std::string const & key) const noexcept {
            return n_args_.find(key) != n_args_.end();
        }
        void setflag_(std::string const & flag) {
            if(inflag_(flag))
                std::cout << "ArgParser warning: setting flag '" << flag << "' for a second time has no effect" << std::endl;
            else {
                if(innamed_(flag))
                    throw cat_on_your_keyboard_error("ArgParser error: there cannot be a flag and named argument with the same name '" + flag + "'");
                flags_.push_back(flag);
            }
        }
        template<typename T>
        void setnamed_(std::string const & key, T const & val, bool const & overwrite = true) {
            if(innamed_(key)) {
                if(overwrite) {
                    std::cout << "ArgParser warning: named argument '" << key << "' is overwritten (" << n_args_[key] << " -> " << val << ")" << std::endl;
                    n_args_[key] = val;
                }
            } else {
                if(inflag_(key))
                    throw cat_on_your_keyboard_error("ArgParser error: there cannot be a flag and named argument with the same name '" + key + "'");
                n_args_[key] = val;
            }
        }
        arg_type find_type_(vec_type<std::string> const & arg, uint const & idx) const {
            if(idx >= arg.size())
                return invalid;
            
            if(arg[idx][0] == '-') { // flag_m, flag_mm, named_m_stick, named_m, named_mm, named_m_eq, named_mm_eq
                if(arg[idx][1] == '-') { // flag_mm, named_mm, named_mm_eq
                    if(arg[idx].find("=") != std::string::npos)
                        return named_mm_eq;
                    else if(find_type_(arg, idx+1) == free)
                        return named_mm;
                    else
                        return flag_mm;
                }
                if(arg[idx].size() == 2) { //flag_m, named_m
                    if(find_type_(arg, idx+1) == free)
                        return named_m;
                    else
                        return flag_m;
                } else { // named_m_stick, named_m_eq
                    if(arg[idx][2] == '=')
                        return named_m_eq;
                    else
                        return named_m_stick;
                }
            
            } else { // free or named_eq
                if(arg[idx].find("=") != std::string::npos) // named_eq
                    return named_eq;
                
                return free;
            }
        }
        void parse_(vec_type<std::string> const & arg) {
            for(uint i = 0; i < arg.size(); ++i) {
                auto t = find_type_(arg, i);
                
                std::string arg1 = arg[i];
                std::string help = "";
                int pos;
                
                switch(t) {
                    case(named_m_stick):
                        help.push_back(arg1[1]);
                        arg1.erase(0,2);
                        setnamed_(help, str_to_type_(poly_type(arg1)));
                        arg1 = help;
                        break;
                    case(named_mm):
                        arg1.erase(0,1);
                    case(named_m):
                        arg1.erase(0,1);
                        setnamed_(arg1, str_to_type_(poly_type(arg[i+1])));
                        ++i;
                        break;
                    case(named_mm_eq):
                        arg1.erase(0,1);
                    case(named_m_eq):
                        arg1.erase(0,1);
                    case(named_eq):
                        pos = arg1.find("=");
                        help = arg1;
                        help.erase(0, pos + 1);
                        arg1.erase(pos, arg1.size());
                        setnamed_(arg1, str_to_type_(poly_type(help)));
                        break;
                    case(free):
                        f_args_.push_back(str_to_type_(poly_type(arg1)));
                        break;
                    case(flag_mm):
                        arg1.erase(0,1);
                    case(flag_m):
                        arg1.erase(0,1);
                        setflag_(arg1);
                        break;
                    default:
                        break;
                }
            }
        }
        poly_type str_to_type_(poly_type const & a) {
            std::string val = a;
            
            size_t ipos = 0;
            int ival = 0;
            try {
                ival = std::stoi(val, & ipos);
            } catch(...) {}
            
            size_t dpos = 0;
            double dval = 0;
            try {
                dval = std::stod(val, & dpos);
                return poly_type(dval);
            } catch(...) {}
            
            if(ipos == val.size())
                return poly_type(ival);
            else if(dpos == val.size())
                return poly_type(dval);
            else
                return poly_type(val);
        }
    private:
        map_type n_args_;
        vec_type<poly_type> f_args_;
        vec_type<std::string> flags_;
        std::string cwd_;
        std::string pwd_;
        std::string progname_;
    };
    /**@brief stream operator for the argument parser
     */
    std::ostream & operator<<(std::ostream & os, ArgParser const & arg) {
        arg.print(os);
        return os;
    }
    
    /// \example basic_example.cpp
    
    /// \example mixed_example.cpp
    
    /// \example merge_example.cpp

}//end namespace fsc
#endif //FSC_ARGPARSER_HEADER
