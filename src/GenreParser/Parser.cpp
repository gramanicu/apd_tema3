/**
 * @file Parser.cpp
 * @author Grama Nicolae (gramanicu@gmail.com)
 * @brief
 * @copyright Copyright (c) 2020
 */

#include "Parser.hpp"

namespace GenreParser {
    Enums::GenresType Parser::current_out_type;
    pthread_barrier_t Parser::barrier;
    std::vector<int> Parser::output_order;

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
        std::queue<Paragraph> recv_paragraphs;

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

                // Store the unprocessed paragraph
                Paragraph p(paragraph.str());

                // Send Paragraph
                MPI_Send(p.to_string().c_str(), p.to_string().size(), MPI_CHAR,
                         worker_id, 0, MPI_COMM_WORLD);

                // Receive the processed paragraph
                int p_size;
                MPI_Status status;
                MPI_Probe(worker_id, 0, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_CHAR, &p_size);

                char* processed_par = (char*)malloc(sizeof(char) * p_size);

                MPI_Recv(processed_par, p_size, MPI_CHAR, worker_id, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Store paragraph
                p.set_lines(std::string(processed_par));
                recv_paragraphs.push(p);

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
        for (unsigned int i = 0; i < output_order.size(); ++i) {
            if (thread_id == 0) {
                current_out_type =
                    static_cast<Enums::GenresType>(output_order[i]);
            }

            pthread_barrier_wait(&barrier);

            if (!recv_paragraphs.empty()) {
                Paragraph p = recv_paragraphs.front();
                if (p.get_type() == current_out_type) {
                    std::cout << p.to_string();
                    if (i != output_order.size() - 1) { std::cout << "\n"; }
                    recv_paragraphs.pop();
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

            if (p_size == 0) {
                eof = true;
            } else {
                // Process the paragraph in multiple threads - TODO
                int lines = 10;
                int t_count = std::min(lines / 20 + 1, max_threads);
                std::vector<std::thread> processors(t_count);

                // Send the processed paragraph
                MPI_Send(raw_paragraph, p_size, MPI_CHAR, Constants::MASTER,
                         status.MPI_TAG, MPI_COMM_WORLD);

                free(raw_paragraph);
            }
        }
    }
}    // namespace GenreParser