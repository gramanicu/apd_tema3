# Paragraph Parser

This is the 3rd homework for the Parallel and Distributed Algorithms Course. ([the problem statement](./problem_statement.pdf), in romanian).

## Table of Contents

- [Paragraph Parser](#paragraph-parser)
  - [Table of Contents](#table-of-contents)
  - [Project Structure](#project-structure)
  - [Application overview](#application-overview)
    - [Master Node](#master-node)
    - [Worker Node](#worker-node)
  - [Performance](#performance)
  - [Build and Run](#build-and-run)

## Project Structure

- Parser - the logic of the paragraph parser
- Helpers - helper functions, error checking and constants

## Application overview

When the `Parser` object is constructed, MPI will be initialized. The master node will check that an input file was provided.

By calling the `run` method of the `Parser`, two things can happen, depending on the _Node Type_:

- **master** nodes - we initialize the threads that will read, send the paragraphs to the workers and write the processed data to the output
- **worker** nodes - we start a thread that runs the `worker_communicator` function (the code executed by each worker)

### Master Node

The master node has 4 threads, one for each paragraph type. After the thread is started, it will open the input file, and start to read the data, paragraph by paragraph. If the paragraph type is not the one that specific thread should read, it will simply ignore it. One exception is _thread 0_, which will store the order of the paragraphs (all of them).

After a thread found a valid paragraph, it will send to it to the worker "specialized" in that specific paragraph type, and then it will at the processed paragraph to a queue (so it can write it in the output file later).

When a thread reaches `eof`, it will stop the associated worker and wait for all the other threads to finish. After that, by respecting the initial paragraph order, the threads will write to the output file the processed data, from their queues.

### Worker Node

Each worker has 1 thread reserved for data exchange with the `Master`, and multiple threads for paragraph processing. They will run until they receive the `eof` message from the `Master`

Each thread can only process `LINES_PER_THREAD` (20) lines at a time, and the number of threads is limited. The lines are processed in multiple iterations (if we have too many lines). In each iteration, `LINES_PER_THREAD` * `max_threads` lines will be processed.

After we finished parsing the paragraph, the data is sent back to the master node.

## Performance

Below, there is a table comparing a serial solution for this problem and my implementation. The tests were done on a machine with the following specifications:

- OS: WSL, Ubuntu 20.04
- Processor: 8 cores, 4.70 Ghz
- Memory: 32GB, 3200 Mhz

The results are:

| Input Size | Reference | This program |
|------------|-----------|--------------|
| 1KB        | 0.01s     | 0.41s        |
| 2.4MB      | 0.16s     | 0.76s        |
| 7.4MB      | 0.45s     | 0.96s        |
| 138.7MB    | 7.86s     | 5.77s        |
| ~200MB     | 10.65s    | 6.92s        |

As we can observe, on small files, the overhead created by MPI and multithreading exceeds its benefits. However, for big tests, the differences are noticeable.

## Build and Run

The `Makefile` defines different rules used for compilation, debugging, running the code, etc.:

- build - compiles the program
- run - executes the program. It uses 2 variables, `PCOUNT` (the number of mpi processes) & `SFILE` (path to the file, the one with the paragraphs to be parsed)
- clean - removes the binary, object files and some other unnecessary files
- beauty - code-styling for the program
- gitignore - creates/adds rules to the .gitignore files
- archive - creates the homework archive

© 2021 Grama Nicolae, 332CA
