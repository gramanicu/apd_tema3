/**
 * @file Helpers.hpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief
 * @copyright Copyright (c) 2020
 */

#pragma once

#include <iostream>
#include <sstream>

namespace GenreParser::Constants {
    const int MASTER = 0;
    const int WORKERS = 4;
    const int MASTER_THREADS = WORKERS;
    const int LINES_PER_THREAD = 20;
}    // namespace GenreParser::Constants

namespace GenreParser::Enums {
    enum class GenresType { Horror, Comedy, Fantasy, SciFi };
    enum class NodesType { Master, Horror, Comedy, Fantasy, SciFi };

    std::string to_string(const GenresType& gt);
    std::string to_string(const NodesType& nt);
    std::ostream& operator<<(std::ostream& os, const GenresType& gt);
    std::ostream& operator<<(std::ostream& os, const NodesType& nt);
}    // namespace GenreParser::Enums

namespace GenreParser::Conditions {
    /**
     * @brief Check if the condition is triggered. If it is not, print the
     * error message and exit the program
     * @param condition The condition that must happen
     * @param error The error message
     * @param code The exit code
     */
    inline void MUST(bool condition, std::string error, int code = -1) {
        if (!condition) {
            std::cerr << error;
            exit(code);
        }
    }

    /**
     * @brief Check if the condition is triggered. If it is, print the
     * error message and exit the program
     * @param condition The condition that must happen
     * @param error The error message
     * @param code The exit code
     */
    inline void MUST_NOT(bool condition, std::string error, int code = -1) {
        if (condition) {
            std::cerr << error;
            exit(code);
        }
    }
}    // namespace GenreParser::Conditions