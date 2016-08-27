// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    19.08.2015 17:49:56 CEST
// File:    fsc_except.hpp

#ifndef FSC_FSC_EXCEPT_HEADER
#define FSC_FSC_EXCEPT_HEADER

#include <stdexcept>

namespace fsc {
    class cat_on_your_keyboard_error: public std::runtime_error {
        using base = std::runtime_error;
    public:
        explicit cat_on_your_keyboard_error(const std::string& what_arg): base(what_arg) {
        }
    };
    class rethink_your_live_choices_error: public std::runtime_error {
        using base = std::runtime_error;
    public:
        explicit rethink_your_live_choices_error(const std::string& what_arg): base(what_arg) {
        }
    };
    class maybe_your_only_purpose_in_live_is_to_serve_as_a_bad_example_error: public std::runtime_error {
        using base = std::runtime_error;
    public:
        explicit maybe_your_only_purpose_in_live_is_to_serve_as_a_bad_example_error(const std::string& what_arg): base(what_arg) {
        }
    };
    class O__o: public std::runtime_error {
        using base = std::runtime_error;
    public:
        explicit O__o(const std::string& what_arg): base(what_arg) {
        }
    };
}//end namespace fsc

#endif //FSC_FSC_EXCEPT_HEADER
