#ifndef EVALUATOR_H
#define EVALUATOR_H
#include <vector>
#include <string>
#include "tester.h"
#include "timer.h"

class Evaluator
{
public:
    Evaluator();
    Evaluator(Timer tim);
    Evaluator(Tester tes);
    Evaluator(Timer tim, Tester tes);
    ~Evaluator();

    Evaluator &set_timer(Timer tim);
    Evaluator &set_tester(Tester tes);

    /*  Run evaluation based on the current settings
     */
    void evaluate();

    /*  Methods that control evaluation settings
     */
    Evaluator &set_program(std::string name);
    Evaluator &add_corresponding_files(const std::vector<std::string> &inputs,
                                      const std::vector<std::string> &outputs);
    Evaluator &add_input_and_output(std::string input, std::string output);
    Evaluator &set_copy_directory(std::string name);
    Evaluator &set_copy_extension(std::string ext);
    Evaluator &save_all_outputs();
    Evaluator &save_failed_outputs();
    Evaluator &set_args(const std::vector<std::string> &args);
    Evaluator &set_args(const char *const *args);
    Evaluator &set_args(const char *const *args, unsigned num_args);
    Evaluator &add_arg(std::string new_arg);
    Evaluator &be_verbose();
    Evaluator &be_quiet();
    Evaluator &print_exit_code();

    /*  Methods that control testing settings
     */
    Evaluator &ignore_whitespace();
    Evaluator &consider_whitespace();
    Evaluator &ignore_char(char c);
    Evaluator &ignore_chars(std::string s);
    Evaluator &heed_char(char c);
    Evaluator &heed_all_chars();
    Evaluator &ignore_extra_chars();
    Evaluator &ignore_too_few_chars();
    Evaluator &expect_proper_output_length();
    Evaluator &print_on_test_success();

    /*  Methods that control timing settings
     */
    Evaluator &report_only_avg_time();
    Evaluator &dont_report_avg_time();
    Evaluator &report_all_times();
    Evaluator &set_precision_after_decimal(unsigned p);
    Evaluator &set_precision_before_decimal(unsigned p);
    Evaluator &set_line_width(unsigned w);
    Evaluator &set_spacing(unsigned n);
    Evaluator &set_header(std::string head);
    Evaluator &set_footer(std::string foot);

private:
    Tester tes;
    Timer tim;
    std::string program_name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> args;
    std::string copy_directory;
    std::string copy_ext;
};


#endif
