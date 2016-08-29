/** ****************************************************************************
 * \file    basic_example.cpp
 * \brief   Basic Example of the ArgParser
 * \author
 * Year      | Name
 * --------: | :------------
 * 2014-2015 | Mario S. Koenz
 *    2016   | C. Frescolino
 * \copyright  see LICENSE
 ******************************************************************************/

#include <iostream>
#include <fsc/ArgParser.hpp>

int main(int argc, char* argv[]) {
    
    fsc::ArgParser ap(argc, argv);
    std::cout << ap << std::endl;
    
    auto x = ap[0];
    x += "4";
    
    std::string foo("foo" + ap[0]);
    
    std::cout << foo << std::endl;
    std::cout << x << std::endl;
    
    return 0;
}
