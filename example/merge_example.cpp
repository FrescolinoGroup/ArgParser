/** ****************************************************************************
 * \file    merge_example.cpp
 * \brief   Merge ArgParser Example
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
    
    fsc::ArgParser ap("--mcs 20 --type=sim --slow -n10 free0");
    fsc::ArgParser rhs("--mcs 10 --E=10 --fast -n5 free1 free2");

    ap.merge(rhs, false);
    // ap has now the named arguments: mcs = 20, type = sim, n = 10, E = 10
    //                 free arguments: free0
    //                          flags: slow fast
    ap.merge(rhs);
    // ap has now the named arguments: mcs = 10,  type = sim, n = 5, E = 10
    //                 free arguments: free1 free2
    //                          flags: slow fast

    std::cout << ap << std::endl;
        
    return 0;
}
