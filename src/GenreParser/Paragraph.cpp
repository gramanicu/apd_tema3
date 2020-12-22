/**
 * @file Paragraph.cpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief
 * @copyright Copyright (c) 2020
 */

#include "Paragraph.hpp"

namespace GenreParser {
    Paragraph::Paragraph() : data(""), line_count(0) {}

    Paragraph::Paragraph(const std::string& data) : data(data) {
        using namespace Enums;

        std::stringstream p(data);
        std::string line;
        std::vector<std::string> row;

        while (getline(p, line)) { row.push_back(line); }

        line_count = row.size() - 1;

        std::string type_s = row[0];

        if (type_s == "horror") {
            type = GenresType::Horror;
        } else if (type_s == "comedy") {
            type = GenresType::Comedy;
        } else if (type_s == "fantasy") {
            type = GenresType::Fantasy;
        } else if (type_s == "science-fiction") {
            type = GenresType::SciFi;
        }

        for (unsigned int i = 1; i < row.size(); ++i) {
            lines.push_back(row[i]);
        }
    }

    Paragraph::Paragraph(const Paragraph& other) {
        this->data = other.data;
        this->line_count = other.line_count;
        this->type = other.type;

        this->lines.clear();
        for (auto& line : other.lines) { this->lines.push_back(line); }
    }

    Paragraph& Paragraph::operator=(const Paragraph& other) {
        this->data = other.data;
        this->line_count = other.line_count;
        this->type = other.type;

        this->lines.clear();
        for (auto& line : other.lines) { this->lines.push_back(line); }

        return *this;
    }

    std::string Paragraph::to_string() const { return data; }
    Enums::GenresType Paragraph::get_type() const { return type; }
    int Paragraph::get_line_count() const { return line_count; }
    std::vector<std::string> Paragraph::get_lines() const { return lines; }

    void Paragraph::set_lines(const std::string& lines) {
        std::stringstream p(lines);
        std::string line;

        this->lines.clear();
        while (getline(p, line)) { this->lines.push_back(line); }
    }
}    // namespace GenreParser