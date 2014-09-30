#ifndef TESTER_H_INCLUDED
#define TESTER_H_INCLUDED

#include <string>
#include <vector>

class Tester
{
public:
    Tester();

    bool run();
    std::string run_verbosely();

    void set_benchmark_file(std::string filename);
    void set_comparison_file(std::string filename);

    void ignore_whitespace();
    void ignore_char(char c);
    void ignore_extra_chars();
    void ignore_too_few_chars();

private:
    bool ignore_spaces;
    std::string chars_to_ignore;
    std::string benchmark_file;
    std::string comparison_file;
};

#endif
