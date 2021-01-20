/**
 * @file Helpers.cpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief Helpers implementations
 * @copyright Copyright (c) 2020
 */

#include "Helpers.hpp"

namespace GenreParser {
    namespace Enums {
        std::string to_string(const GenresType& gt) {
            switch (gt) {
                case GenresType::Horror: {
                    return "horror";
                } break;
                case GenresType::Comedy: {
                    return "comedy";
                } break;
                case GenresType::Fantasy: {
                    return "fantasy";
                } break;
                case GenresType::SciFi: {
                    return "science-fiction";
                } break;
            }
            return "";
        }

        std::string to_string(const NodesType& nt) {
            switch (nt) {
                case NodesType::Master: {
                    return "Master";
                } break;
                case NodesType::Horror: {
                    return "Horror";
                } break;
                case NodesType::Comedy: {
                    return "Comedy";
                } break;
                case NodesType::Fantasy: {
                    return "Fantasy";
                } break;
                case NodesType::SciFi: {
                    return "SciFi";
                } break;
            }
            return "";
        }

        std::ostream& operator<<(std::ostream& os, const GenresType& gt) {
            switch (gt) {
                case GenresType::Horror: {
                    os << "horror";
                } break;
                case GenresType::Comedy: {
                    os << "comedy";
                } break;
                case GenresType::Fantasy: {
                    os << "fantasy";
                } break;
                case GenresType::SciFi: {
                    os << "science-fiction";
                } break;
            }
            return os;
        }

        std::ostream& operator<<(std::ostream& os, const NodesType& nt) {
            switch (nt) {
                case NodesType::Master: {
                    os << "Master";
                } break;
                case NodesType::Horror: {
                    os << "Horror";
                } break;
                case NodesType::Comedy: {
                    os << "Comedy";
                } break;
                case NodesType::Fantasy: {
                    os << "Fantasy";
                } break;
                case NodesType::SciFi: {
                    os << "SciFi";
                } break;
            }
            return os;
        }
    }    // namespace Enums
}    // namespace GenreParser