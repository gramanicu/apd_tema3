/**
 * @file Parser.cpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief
 * @copyright Copyright (c) 2020
 */

#include "Parser.hpp"

namespace GenreParser {
    Parser::Parser(int argc, char *argv[]) {
        int provided;

        MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

        // Check if provided = MPI_THREAD_MULTIPLE
    }

    void Parser::run() {
        std::cout << "Hello World\n";
        MPI_Finalize();
    }
}    // namespace GenreParser