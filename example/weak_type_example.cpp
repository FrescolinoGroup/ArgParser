/** ****************************************************************************
 * \file    weak_type_example.cpp
 * \brief   Shows the automatic casting ArgParser
 * \author
 * Year      | Name
 * --------: | :------------
 * 2014-2015 | Mario S. Koenz
 *    2016   | C. Frescolino
 * \copyright  see LICENSE
 ******************************************************************************/

#include <fsc/ArgParser.hpp>
#include <iostream>

int main() {
    
    fsc::ArgParser ap("--mcs 20 --type=sim --slow -n0.2 free0");
    
    std::string type = ap["type"];  // will be cast to a string
    
    auto mcs = ap["mcs"];           // mcs is a weak type
    
    int mcs_int = mcs;              // automatic cast via context
    
    mcs = " now I'm a string";      // can change type on the fly
    
    std::string mcs_string(mcs);    // automatic correct cast
    
    // int mcs_int2(mcs);           // this would now fail with a runtime error
    
    double x = ap["n"] * ap["mcs"]; // basic operations (+ - * /) are supported
                                    // where it makes sense

    std::string free = "";
    free += ap[0];
    free += mcs_string;

    std::cout << type << std::endl;
    std::cout << mcs_int << std::endl;
    std::cout << x << std::endl;
    std::cout << free << std::endl;
    
    return 0;
}
