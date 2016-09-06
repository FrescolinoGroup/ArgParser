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

#include <fsc/ArgParser.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    
    fsc::ArgParser ap(argc, argv); // parses the command line arguments
    std::cout << ap << std::endl;  // prints the content
                                   // see mixed_example.cpp for more features
    return 0;
}
