//
//  StringHelpers.hpp
//  ClashStats
//
//  Created by Atlas on 11/20/15.
//  Copyright © 2015 JTJ. All rights reserved.
//

#ifndef StringHelpers_hpp
#define StringHelpers_hpp

#include <stdio.h>
#include <string>

/**
 * Removes leading and trailing whitespace from a string.
 */
std::string trim(const std::string &str, const std::string &whitespace = " \t");

/**
 * Removes extra whitespace (leading, trailing, in between words) in a string
 */
std::string reduce(const std::string &str, const std::string &fill = " ", const std::string &whitespace = " \t");

#endif /* StringHelpers_hpp */
