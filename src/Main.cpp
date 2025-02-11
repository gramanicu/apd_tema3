/**
 * @file Main.cpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief Main file, runs the parser
 * @copyright Copyright (c) 2020
 */

#include "./GenreParser/Parser.hpp"

int main(int argc, char *argv[]) {
    GenreParser::Parser parser(argc, argv);
    parser.run();
}