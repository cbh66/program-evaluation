
#include <iostream>
#include "evaluator.h"
using namespace std;

Evaluator::Evaluator()
{
    // Intentionally empty
}

Evaluator::Evaluator(Timer tim)
{
    Evaluator();
    set_timer(tim);
}

Evaluator::Evaluator(Tester tes)
{
    Evaluator();
    set_tester(tes);
}

Evaluator::Evaluator(Timer tim, Tester tes)
{
    Evaluator();
    set_timer(tim);
    set_tester(tes);
}

Evaluator::~Evaluator()
{
    // Intentionally empty
}

Evaluator &Evaluator::set_timer(Timer tim)
{
    this->tim = tim;
    return *this;
}

Evaluator &Evaluator::set_tester(Tester tes)
{
    this->tes = tes;
    return *this;
}


void Evaluator::evaluate()
{

}


Evaluator &Evaluator::set_program(string name)
{
    program_name = name;
    return *this;
}

Evaluator &Evaluator::add_corresponding_files(const vector<string> &inputs,
                                              const vector<string> &outputs)
{
    unsigned in_length = inputs.size();
    unsigned out_length = outputs.size();
    for (unsigned i = 0; i < in_length || i < out_length; ++i) {
        if (i < in_length) {
            this->inputs.push_back(inputs[i]);
        } else {
            this->inputs.push_back("");
        }
        if (i < out_length) {
            this->outputs.push_back(outputs[i]);
        } else {
            this->outputs.push_back("");
        }
    }
    return *this;
}

Evaluator &Evaluator::add_input_and_output(string input, string output)
{
    inputs.push_back(input);
    outputs.push_back(output);
    return *this;
}

Evaluator &Evaluator::set_copy_directory(string name)
{
    copy_directory = name;
    return *this;
}

Evaluator &Evaluator::set_copy_extension(string ext)
{
    copy_ext = ext;
    return *this;
}

Evaluator &Evaluator::save_all_outputs()
{

    return *this;
}

Evaluator &Evaluator::save_failed_outputs()
{

    return *this;
}

Evaluator &Evaluator::set_args(const vector<string> &args)
{
    this->args = args;
    return *this;
}

Evaluator &Evaluator::set_args(const char *const *args)
{
    if (args != NULL) {
        this->args.clear();
        for (unsigned i = 0; args[i] != NULL; ++i) {
            this->args.push_back(string(args[i]));
        }
    }
    return *this;
}

Evaluator &Evaluator::set_args(const char *const *args, unsigned num_args)
{
    if (args != NULL) {
        this->args.clear();
        for (unsigned i = 0; i < num_args; ++i) {
            string next_arg = args[i];
            if (args[i] == NULL) next_arg = "";
            this->args.push_back(next_arg);
        }
    }
    return *this;
}


Evaluator &Evaluator::add_arg(string new_arg)
{
    args.push_back(new_arg);
    return *this;
}

Evaluator &Evaluator::be_verbose()
{

    return *this;
}

Evaluator &Evaluator::be_quiet()
{

    return *this;
}

Evaluator &Evaluator::print_exit_code()
{

    return *this;
}


/*  Methods that control testing settings
 */
Evaluator &Evaluator::ignore_whitespace()
{
    tes.ignore_whitespace();
    return *this;
}

Evaluator &Evaluator::consider_whitespace()
{
    tes.consider_whitespace();
    return *this;
}

Evaluator &Evaluator::ignore_char(char c)
{
    tes.ignore_char(c);
    return *this;
}

Evaluator &Evaluator::ignore_chars(string s)
{
    tes.ignore_chars(s);
    return *this;
}

Evaluator &Evaluator::heed_char(char c)
{
    tes.heed_char(c);
    return *this;
}

Evaluator &Evaluator::heed_all_chars()
{
    tes.heed_all_chars();
    return *this;
}

Evaluator &Evaluator::ignore_extra_chars()
{
    tes.ignore_extra_chars();
    return *this;
}

Evaluator &Evaluator::ignore_too_few_chars()
{
    tes.ignore_too_few_chars();
    return *this;
}

Evaluator &Evaluator::expect_proper_output_length()
{
    tes.expect_proper_length();
    return *this;
}

Evaluator &Evaluator::print_on_test_success()
{
    tes.print_on_success();
    return *this;
}


Evaluator &Evaluator::report_only_avg_time()
{
    tim.report_only_avg();
    return *this;
}

Evaluator &Evaluator::dont_report_avg_time()
{
    tim.dont_report_avg();
    return *this;
}

Evaluator &Evaluator::report_all_times()
{
    tim.report_all();
    return *this;
}

Evaluator &Evaluator::set_precision_after_decimal(unsigned p)
{
    tim.precision_after_decimal(p);
    return *this;
}

Evaluator &Evaluator::set_precision_before_decimal(unsigned p)
{
    tim.precision_before_decimal(p);
    return *this;
}

Evaluator &Evaluator::set_line_width(unsigned w)
{
    tim.line_width(w);
    return *this;
}

Evaluator &Evaluator::set_spacing(unsigned n)
{
    tim.spacing(n);
    return *this;
}

Evaluator &Evaluator::set_header(string head)
{
    tim.set_header(head);
    return *this;
}

Evaluator &Evaluator::set_footer(string foot)
{
    tim.set_footer(foot);
    return *this;
}

