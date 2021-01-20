/**
 * @file Parser.cpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief Paragraph parser implementation
 * @copyright Copyright (c) 2020
 */

#include "Parser.hpp"

namespace GenreParser {
    Enums::GenresType Parser::current_out_type;
    pthread_barrier_t Parser::barrier;
    std::vector<int> Parser::output_order;
    bool Parser::output_created;

    Parser::Parser(int argc, char* argv[]) {
        int provided;
        MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

        // Check if provided = MPI_THREAD_MULTIPLE
        Conditions::MUST(provided == MPI_THREAD_MULTIPLE,
                         "Couldn't init MPI\n");

        MPI_Comm_size(MPI_COMM_WORLD, &worker_count);
        MPI_Comm_rank(MPI_COMM_WORLD, &worker_rank);

        node_type = static_cast<Enums::NodesType>(worker_rank);

        if (node_type == Enums::NodesType::Master) {
            // Check if the input file was provided
            Conditions::MUST(argc >= 2, to_string(node_type) +
                                            ": Input file not provided\n");
            input_path = std::string(argv[1]);
            output_path =
                input_path.substr(0, input_path.find_last_of('.')) + ".out";
            output_created = false;
            pthread_barrier_init(&barrier, NULL, Constants::MASTER_THREADS);
        }
    }

    void Parser::run() {
        using namespace Enums;
        using namespace Constants;
        switch (node_type) {
            case NodesType::Master: {
                std::vector<std::thread> master_threads;

                for (int thread_id = 0; thread_id < Constants::MASTER_THREADS;
                     ++thread_id) {
                    master_threads.push_back(
                        std::thread(&Parser::process_file, this, thread_id));
                }

                for (auto& thread : master_threads) { thread.join(); }
            } break;
            case NodesType::Horror:
            case NodesType::Comedy:
            case NodesType::Fantasy:
            case NodesType::SciFi: {
                std::thread comm_thread;
                comm_thread = std::thread(&Parser::worker_communicator, this);
                comm_thread.join();
            } break;
        }
        MPI_Finalize();
    }

    void Parser::process_file(const int thread_id) const {
        int worker_id = thread_id + 1;
        Enums::GenresType p_type = static_cast<Enums::GenresType>(thread_id);
        std::string genre = Enums::to_string(p_type);
        std::queue<std::string> recv_paragraphs;

        std::ifstream input(input_path);
        Conditions::MUST(
            input.is_open(),
            to_string(node_type) + ": Input file could not be opened\n");

        // Read all the file, ignoring invalid paragraphs
        std::stringstream paragraph;
        std::string line;
        while (!input.eof()) {
            std::getline(input, line);

            // Store the initial order of the paragraphs
            if (thread_id == 0) {
                using namespace Enums;
                if (line == to_string(GenresType::Horror)) {
                    output_order.push_back((int)GenresType::Horror);
                } else if (line == to_string(GenresType::Comedy)) {
                    output_order.push_back((int)GenresType::Comedy);
                } else if (line == to_string(GenresType::Fantasy)) {
                    output_order.push_back((int)GenresType::Fantasy);
                } else if (line == to_string(GenresType::SciFi)) {
                    output_order.push_back((int)GenresType::SciFi);
                }
            }

            // Found a valid paragraph
            if (line == genre) {
                while (line != "" && !input.eof()) {
                    paragraph << line << "\n";
                    std::getline(input, line);
                }

                if (input.eof()) { paragraph << line << "\n"; }
                std::string data = paragraph.str();

                // Send the processed paragraph
                MPI_Send(data.c_str(), data.length(), MPI_CHAR, worker_id, 0,
                         MPI_COMM_WORLD);

                // Receive the processed paragraph
                int p_size;
                MPI_Status status;
                MPI_Probe(worker_id, 0, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_CHAR, &p_size);

                char* processed_par = (char*)malloc(sizeof(char) * p_size);

                MPI_Recv(processed_par, p_size, MPI_CHAR, worker_id, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                processed_par[p_size - 1] = '\0';

                // Store paragraph
                recv_paragraphs.push(std::string(processed_par));

                // Prepare next paragraph
                free(processed_par);
                paragraph.str("");
                paragraph.clear();
            }
        }
        input.close();

        // Send EOF
        MPI_Send(nullptr, 0, MPI_CHAR, worker_id, 0, MPI_COMM_WORLD);

        // Print processed paragraphs
        std::ofstream out;

        for (unsigned int i = 0; i < output_order.size(); ++i) {
            if (thread_id == 0) {
                current_out_type =
                    static_cast<Enums::GenresType>(output_order[i]);
            }

            pthread_barrier_wait(&barrier);

            if (!recv_paragraphs.empty()) {
                std::string p = recv_paragraphs.front();

                // Check type of the paragraph
                std::stringstream par(p);
                std::string type;
                par >> type;

                if (type == Enums::to_string(current_out_type)) {
                    if (!output_created) {
                        out.open(output_path);
                        output_created = true;
                    } else {
                        out.open(output_path, std::ios::app);
                    }
                    out << p;
                    if (i != output_order.size() - 1) { out << "\n"; }
                    recv_paragraphs.pop();
                    out.close();
                }
            }

            pthread_barrier_wait(&barrier);
        }
    }

    void Parser::worker_communicator() const {
        int max_threads = std::thread::hardware_concurrency() - 1;
        bool eof = false;

        while (!eof) {
            // Receive paragraph
            int p_size;
            MPI_Status status;
            MPI_Probe(Constants::MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &p_size);

            char* raw_paragraph = (char*)malloc(sizeof(char) * p_size);

            MPI_Recv(raw_paragraph, p_size, MPI_CHAR, Constants::MASTER,
                     MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            raw_paragraph[p_size - 1] = '\0';

            if (p_size == 0) {
                eof = true;
            } else {
                std::string type, line;
                std::stringstream p(raw_paragraph);
                std::vector<std::string> content;

                // Get the number of lines in the paragraph
                p >> type;
                while (!p.eof()) {
                    std::getline(p, line);
                    if (line.size() != 0) { content.push_back(line); }
                }

                // Process the paragraph in multiple threads
                int total_runs =
                    (int)content.size() / Constants::LINES_PER_THREAD + 1;
                total_runs = std::max(1, total_runs);

                int remaining_runs = total_runs;
                int start = 0, end = Constants::LINES_PER_THREAD;

                // Run the maximum allowed number of threads until all lines
                // are processed
                while (remaining_runs > 0) {
                    int t_count = std::min(remaining_runs, max_threads);
                    t_count = std::max(1, t_count);
                    std::vector<std::thread> processors(t_count);

                    // Init threads
                    for (int id = 0; id < t_count; ++id) {
                        Enums::GenresType t_type =
                            static_cast<Enums::GenresType>(worker_rank - 1);

                        start = std::min(start, (int)content.size());
                        end = std::min(end, (int)content.size());

                        processors.push_back(
                            std::thread(&Parser::process_lines, this,
                                        std::ref(content), start, end, t_type));

                        start += Constants::LINES_PER_THREAD;
                        end += Constants::LINES_PER_THREAD;
                    }

                    for (auto& thread : processors) {
                        if (thread.joinable()) { thread.join(); }
                    }

                    remaining_runs -= max_threads;
                }

                // // Single-Threaded Processing
                // Enums::GenresType p_type =
                //     static_cast<Enums::GenresType>(worker_rank - 1);
                // process_lines(content, 0, content.size(), p_type);

                // Get the processed paragraph data
                std::stringstream ss;
                ss << type << "\n";
                for (auto& line : content) { ss << line << "\n"; }

                std::string data = ss.str();

                // Send the processed paragraph
                MPI_Send(data.c_str(), data.length() + 1, MPI_CHAR,
                         Constants::MASTER, status.MPI_TAG, MPI_COMM_WORLD);

                free(raw_paragraph);
            }
        }
    }

    void Parser::process_lines(std::vector<std::string>& lines, int start,
                               int end, Enums::GenresType type) const {
        using namespace Enums;

        switch (type) {
            case GenresType::Horror: {
                for (int i = start; i < end; ++i) {
                    std::stringstream line(lines[i]);
                    std::string word;
                    std::string result = "";

                    while (std::getline(line, word, ' ')) {
                        for (unsigned int lcount = 0; lcount < word.size();
                             ++lcount) {
                            result += word.at(lcount);
                            if (Utilities::is_consonant(word.at(lcount))) {
                                result += std::tolower(word.at(lcount));
                            }
                        }
                        result += ' ';
                    }
                    result.pop_back();

                    lines[i] = result;
                }
            } break;
            case GenresType::Comedy: {
                for (int i = start; i < end; ++i) {
                    std::stringstream line(lines[i]);
                    std::string result = "";
                    std::string word;

                    while (std::getline(line, word, ' ')) {
                        for (unsigned int lcount = 0; lcount < word.size();
                             ++lcount) {
                            if (lcount % 2 == 1) {
                                result += std::toupper(word.at(lcount));
                            } else {
                                result += word.at(lcount);
                            }
                        }
                        result += ' ';
                    }

                    result.pop_back();
                    lines[i] = result;
                }
            } break;
            case GenresType::Fantasy: {
                for (int i = start; i < end; ++i) {
                    std::stringstream line(lines[i]);
                    std::string result = "";
                    std::string word;

                    while (std::getline(line, word, ' ')) {
                        word[0] = std::toupper(word[0]);
                        result += word + " ";
                    }

                    result.pop_back();
                    lines[i] = result;
                }
            } break;
            case GenresType::SciFi: {
                for (int i = start; i < end; ++i) {
                    std::stringstream line(lines[i]);
                    std::string result = "";
                    std::string word;
                    int word_counter = 0;

                    while (std::getline(line, word, ' ')) {
                        word_counter++;
                        if (word_counter == 7) {
                            word_counter = 0;
                            std::reverse(word.begin(), word.end());
                        }

                        result += word + " ";
                    }

                    result.pop_back();
                    lines[i] = result;
                }
            } break;
        }
    }    // namespace GenreParser
}    // namespace GenreParser