/*
 * NumberConversion.h
 *
 *  Created on: Aug 14, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <exception>
#include <utility>

namespace nmea {

    class NumberConversionError : public std::exception {
      public:
        explicit NumberConversionError(std::string msg):message(std::move(msg)){};
        ~NumberConversionError() override = default;

      public:
        std::string message;
        std::string what() {
          return message;
        }
    };

    double parseDouble(const std::string& s);
    int64_t parseInt(const std::string& s, int radix = 10);

//void NumberConversion_test();

}



