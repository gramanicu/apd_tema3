/**
 * @file Paragraph.hpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief
 * @copyright Copyright (c) 2020
 */

#pragma once

#include <vector>

#include "Helpers.hpp"
#include "string"

namespace GenreParser {
    class Paragraph {
       private:
        std::string data;
        int line_count;
        Enums::GenresType type;
        std::vector<std::string> lines;

       public:
        /**
         * @brief Default constructor
         */
        Paragraph();

        /**
         * @brief Construct a new Paragraph object from a string (full paragraph
         * + title) It will parse the lines to get all data it requires
         * @param data The paragraph in string form
         */
        Paragraph(const std::string& data);

        /**
         * @brief Copy-Construct a new Paragraph object
         * @param other The other paragraph
         */
        Paragraph(const Paragraph& other);

        /**
         * @brief Copy-Assignment
         * @param other The other paragraph
         * @return Paragraph& The new paragraph
         */
        Paragraph& operator=(const Paragraph& other);

        /**
         * @brief Return the data contained in the paragraph object, in string
         * form
         * @return std::string The data
         */
        std::string to_string() const;

        Enums::GenresType get_type() const;
        int get_line_count() const;
        std::vector<std::string> get_lines() const;

        /**
         * @brief Set the information contained in the lines (the paragraph,
         * without the title)
         * @param lines The new information
         */
        void set_lines(const std::string& lines);
    };
}    // namespace GenreParser