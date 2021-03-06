/** ****************************************************************************
 * \file    mixed_example.cpp
 * \brief   Mixed Feature Example of the ArgParser
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
    
    fsc::ArgParser ap(argc, argv);// creates an ArgParser and 
                                  // parses the command line
    ap.parse_file("cline.txt");   // reads cline.txt and parses the content
                                  // an already set argument will be overwritten
                                 
    ap["n"];                      // get the named argument "n" if set,
                                  // otherwise a std::runtime_error is thrown
    ap.def("n", 10);              // set named argument "n" to 10 
                                  // if "n" is not already set
    int n = ap.get("n", 10);      // get named argument "n" if set, otherwise 10
    std::cout << n << std::endl; 
     
    ap.is_set("f");               // true if "f" is a flag or named argument
    
    ap[0];                        // get free argument at position 0,
                                  // or std::runtime_error if not set
    ap.is_set(1);                 // true if there is a free argument at pos 1
    ap.freeargc();                // returns the number of free arguments
    
    ap.cwd();                     // return the cwd
    ap.pwd();                     // return the pwd
    ap.progname();                // return the name of the binary
    
    std::cout << ap << std::endl; // print the ArgParser
    
    return 0;
}
