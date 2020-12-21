/**
 * @file Helpers.hpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief
 * @copyright Copyright (c) 2020
 */

#pragma once

namespace GenreParser::Constants {
    const int MASTER = 0;
    const int WORKERS = 4;
    const int LINES_PER_THREAD = 20;
}    // namespace GenreParser::Constants

namespace GenreParser::Enums {
    enum class GenresType { Horror, Comedy, Fantasy, SciFi };
}