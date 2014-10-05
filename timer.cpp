/*---------------------------------------------------------------------------*\
 *  timer.cpp                                                                *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This implementation depends on the string, iostream, and iomanip         *
 *    libraries.                                                             *
 *  TO DO:                                                                   *
\*---------------------------------------------------------------------------*/
#include <iostream>
#include <iomanip>
#include "timer.h"
using namespace std;


Timer::Timer()
{
    report_all();
    before_decimal = 3;
    after_decimal = 4;
    spaces = 4;
    width = 80;
    output = &cout;
}


static double get_real(ProgramInfo p)
{
    return (double) p.wall_sec
        + ((double) p.wall_usec / 1000000.0);
}

static double get_user(ProgramInfo p)
{
    return (double) p.user_sec
        + ((double) p.user_usec / 1000000.0);
}

static double get_sys(ProgramInfo p)
{
    return (double) p.sys_sec
        + ((double) p.sys_usec / 1000000.0);
}

static unsigned num_digits(unsigned x)
{
    if (x < 10) return 1;
    if (x < 100) return 2;
    if (x < 1000) return 3;
    if (x < 10000) return 4;
    if (x < 100000) return 5;
    return 5 + num_digits(x / 100000);
}


void Timer::report_run(ProgramInfo result)
{
        (*output) << "Real:   ";
        (*output) << get_real(result);
        (*output) << "s" << endl << "User:   ";
        (*output) << get_user(result);
        (*output) << "s" << endl << "System: ";
        (*output) << get_sys(result);
        (*output) << "s" << endl << endl;
}

//  Move out priting of the average.  Instead, have it return the
//    running total and then the caller can calculate the average.
double Timer::print_line(const TimeSet times, int start, int end,
                         string seperator, string final,
                         double (*get_num)(ProgramInfo))
{
    int size = times.runs.size();
    string btwn = "";
    double sum = 0.0;
    for (int i = start; i < end && i < size; ++i) {
        double t = get_num(times.runs[i]);
        if (report_all_times) {
            (*output) << btwn << setw(before_decimal + 1 + after_decimal)
                      << std::right << t;
            if (i == end - 1) (*output) << final;
            else if (i == size - 1) (*output) << seperator;
            btwn = seperator;
        }
        sum += t;
    }
    return sum;
}



string Timer::repeat_char(char c, int times)
{
    string r_val = "";
    for (; times > 0; --times) {
        r_val += c;;
    }
    return r_val;
}

void Timer::report_time(const TimeSet results)
{
    verify_dimensions(results.runs.size());
    if (report_all_times) {
        report_runs(results);
    } else if (report_avg) {
        report_avg_alone(results);
    }
}


void Timer::report_runs(const TimeSet results)
{
    unsigned size = results.runs.size();
    if (size == 0) return;
    unsigned test_width = before_decimal + 2 + after_decimal;
    double real_total = 0, user_total = 0, sys_total = 0;
    string between = "s" + repeat_char(' ', spaces);
    unsigned columns = (width + spaces - 8) / (test_width + spaces);
    if (columns == 0) return;
    unsigned current_column = 0;

    (*output) << make_header(results) << endl;
    while (current_column <= size) {
        bool on_last_line = current_column + columns > size;
        (*output) << repeat_char(' ', 8);
        if (report_all_times) {
            for (unsigned j = current_column;
                 j < current_column + columns && j < size; ++j) {
                (*output) << "TRIAL " << j;
                (*output) << repeat_char(' ',
                                         (test_width + spaces)
                                          - (6 + num_digits(j)));
            }
        }
        if (report_avg && on_last_line) (*output) << "  AVG";
        (*output) << endl;
        (*output) << "Real:   ";
        real_total += print_line(results, current_column,
                                 current_column + columns, between, "s",
                                 get_real);
        if (report_avg && on_last_line) {
            (*output) << setw(before_decimal + 1 + after_decimal)
                      << std::right << (real_total / size) << "s";
        }
        (*output) << endl << "User:   ";
        user_total += print_line(results, current_column,
                                 current_column + columns, between, "s",
                                 get_user);
        if (report_avg && on_last_line) {
            (*output) << setw(before_decimal + 1 + after_decimal)
                      << std::right << (user_total / size) << "s";
        }
        (*output) << endl << "System: ";
        sys_total += print_line(results, current_column,
                                 current_column + columns, between, "s",
                                 get_sys);
        if (report_avg && on_last_line) {
            (*output) << setw(before_decimal + 1 + after_decimal)
                      << std::right << (sys_total / size) << "s";
        }
        (*output) << endl << endl;
        current_column += columns;
    }
}


static double average(const TimeSet times, double (*get_num)(ProgramInfo))
{
    int size = times.runs.size();
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += get_num(times.runs[i]);
    }
    return sum / size;
}

void Timer::report_avg_alone(const TimeSet results)
{
    unsigned size = results.runs.size();
    if (size == 0) return;

    (*output) << make_header(results) << endl;
    (*output) << repeat_char(' ', 8);
    (*output) << "  AVG" << endl;
    (*output) << "Real:   ";
    (*output) << setw(before_decimal + 1 + after_decimal)
              << std::right << average(results, get_real) << "s";
    (*output) << endl << "User:   ";
    (*output) << setw(before_decimal + 1 + after_decimal)
              << std::right << average(results, get_user) << "s";
    (*output) << endl << "System: ";
    (*output) << setw(before_decimal + 1 + after_decimal)
              << std::right << average(results, get_sys) << "s";
    (*output) << endl << endl;
}

void Timer::report_times(const vector<TimeSet> all_results)
{
    vector<TimeSet>::const_iterator it = all_results.begin();
    vector<TimeSet>::const_iterator end = all_results.end();
    (*output).setf(ios::fixed);
    (*output).precision(after_decimal);
    while (it != end) {
        report_time(*it);
        ++it;
    }
}



Timer &Timer::report_only_avg()
{
    report_avg = true;
    report_all_times = false;
    return *this;
}


Timer &Timer::dont_report_avg()
{
    report_avg = false;
    report_all_times = true;
    return *this;
}

Timer &Timer::report_all()
{
    report_avg = true;
    report_all_times = true;
    return *this;
}


Timer &Timer::precision_after_decimal(unsigned p)
{
    after_decimal = p;
    return *this;
}

Timer &Timer::precision_before_decimal(unsigned p)
{
    before_decimal = p;
    return *this;
}

Timer &Timer::line_width(unsigned w)
{
    width = w;
    return *this;
}

Timer &Timer::spacing(unsigned n)
{
    spaces = n;
    return *this;
}


Timer &Timer::set_output(ostream *stream)
{
    output = stream;
    return *this;
}


string Timer::make_header(TimeSet test)
{
    if (test.test_name != "") return test.test_name;
    if (test.input_file != "") return test.input_file;
    if (test.output_file != "") return test.output_file;
    if (test.err_file != "") return test.err_file;
    return "Unknown test";
}

/*  First test:  Is there enough padding between numbers?
 *    A single entry has the size of its number plus its spaces.
 *    There should be enough space to fit the longest column header, which
 *    is "TRIAL " + test number.  Hence its size is "TRIAL ".length() +
 *    num_digits(last test's number).
 *  Second test:  Is the max width wide enough to fit at least one column?
 *    It must be able to fit the size of the labels ("System: ".length())
 *    plus the size of a column (the larger of the size of a number and the
 *    size of the header).
 */
void Timer::verify_dimensions(unsigned num_tests)
{
    if (num_tests == 0) return;
    unsigned num_size = before_decimal + 2 + after_decimal;
    unsigned header_size = 6 + num_digits(num_tests - 1);
    if (num_size + spaces < header_size) {
        spaces = header_size - num_size;
    }
    if (width < 8 + header_size || width < 8 + num_size) {
        width = (header_size < num_size) ? (8 + num_size) : (8 + header_size);
    }
}
