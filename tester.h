/*---------------------------------------------------------------------------*\
 *  tester.h                                                                 *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This file contains the interface for the Tester class, a class designed  *
 *    to compare two files.  One file is considered the "benchmark" file,    *
 *    and contains the expected output.  The other is the "comparison" file, *
 *    which contains the actual output.                                      *
 *  One need only create an object of this type, call appropriate functions  *
 *    to specify how to compare the files, and then call one of the run      *
 *    methods.  Available options include ignoring differences in whitespace *
 *    (or other characters, which can be added), ignoring too-long input,    *
 *    and ignoring too-short output.                                         *
 *                                                                           *
 *  Tester::run() simply compares the two files, returning true if they      *
 *     match according to the options set, and false otherwise.              *
 *  Tester::run_verbosely() compares the two files, but returns a more       *
 *     detailed report in the form of a string.  The string will typically   *
 *     report why the test failed - too-short input, too-long input, (unless *
 *     deactivated), or the specific line and column of the comparison file  *
 *     where the first difference was found.  It will say what was expected  *
 *     and what was recieved.  If the files matched, the empty string is     *
 *     returned, unless print_on_success was set, in which case a string     *
 *     with a success message is returned.                                   *
 *                                                                           *
 *  TO DO:                                                                   *
\*---------------------------------------------------------------------------*/
#ifndef TESTER_H_INCLUDED
#define TESTER_H_INCLUDED

#include <string>
#include <vector>
#include <fstream>

class Tester
{
public:
    Tester();

    bool run();
    std::string run_verbosely();

    Tester &set_benchmark_file(std::string filename);
    Tester &set_comparison_file(std::string filename);

    Tester &ignore_whitespace();
    Tester &consider_whitespace();
    Tester &ignore_char(char c);
    Tester &heed_char(char c);
    Tester &heed_all_chars();
    Tester &ignore_extra_chars();
    Tester &ignore_too_few_chars();
    Tester &expect_proper_length();
    Tester &print_on_success();

private:
    bool ignore_spaces;
    bool ignore_extras;
    bool ignore_truncation;
    bool on_success;
    std::string chars_to_ignore;
    std::string benchmark_file;
    std::string comparison_file;

    bool is_ignore_char(char c);
    char get_next_char(std::ifstream &str);
    char get_next_char(std::ifstream &str, int *cols, int *lines);
};

#endif
