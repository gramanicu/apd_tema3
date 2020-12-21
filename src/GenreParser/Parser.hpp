/**
 * @file Parser.hpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief
 * @copyright Copyright (c) 2020
 */

#pragma once

#include <mpi.h>

#include <fstream>

namespace GenreParser {
    class Parser {
       public:
        Parser(int argc, char *argv[]);

        void run();
    };
}    // namespace GenreParser