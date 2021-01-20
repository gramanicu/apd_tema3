/**
 * @file Parser.hpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief The paragraph parser declaration
 * @copyright Copyright (c) 2020
 */

#pragma once

#include <mpi.h>
#include <pthread.h>

#include <algorithm>
#include <fstream>
#include <queue>
#include <sstream>
#include <thread>

#include "Helpers.hpp"

namespace GenreParser {
    class Parser {
       private:
        int worker_rank;
        int worker_count;
        Enums::NodesType node_type;    // Equivalent to the worker rank

        std::string input_path;
        std::string output_path;
        static Enums::GenresType current_out_type;
        static pthread_barrier_t barrier;
        static std::vector<int> output_order;

        static bool output_created;

       public:
        Parser(int argc, char* argv[]);

        void run();

        /**
         * @brief Multithreaded paragraph reader. It will only process
         * paragraphs with a specific tag
         * As there as many threads as paragraph types, we can link the
         * thread_id to a GenresType
         * @param thread_id The id of the thread
         */
        void process_file(const int thread_id) const;

        /**
         * @brief Multithreaded paragraph processor. It will process only the
         * lines linked to it's thread id
         * @param thread_id The id of the thread
         */
        void process_paragraph(const int thread_id,
                               const std::vector<std::string> paragraph) const;

        void worker_communicator() const;

        /**
         * @brief Process the specified lines, according to the paragraph type
         * @param lines The lines of the paragraph
         * @param start The start index
         * @param end The end index
         * @param type The paragraph type
         */
        void process_lines(std::vector<std::string>& lines, int start, int end,
                           Enums::GenresType type) const;
    };
}    // namespace GenreParser