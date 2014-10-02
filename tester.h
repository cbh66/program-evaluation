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
    Tester &ignore_char(char c);
    Tester &ignore_extra_chars();
    Tester &ignore_too_few_chars();
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
