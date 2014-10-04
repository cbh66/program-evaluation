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


void Timer::print_line(const TimeSet times, const string seperator,
                       double (*get_num)(ProgramInfo))
{
    int size = times.runs.size();
    string btwn = "";
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        double t = get_num(times.runs[i]);
        if (report_all_times) {
            (*output) << btwn << setw(before_decimal + 1 + after_decimal)
                      << std::right << t;
            btwn = seperator;
        }
        sum += t;
    }
    if (report_avg) {
        (*output) << btwn << setw(before_decimal + 1 + after_decimal)
                  << std::right << (sum / size);
    }
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
    int size = results.runs.size();
    if (size > 0) {
        (*output) << make_header(results) << endl;
        (*output) << repeat_char(' ', 8);
        if (report_all_times) {
            for (int j = 0; j < size; ++j) {
                (*output) << "TRIAL " << j;
                (*output) << repeat_char(' ',
                            (before_decimal + 2 + after_decimal + spaces)
                                - (6 + num_digits(j)));
            }
        }
        if (report_avg) (*output) << "  AVG";
        (*output) << endl;
        (*output) << "Real:   ";
        print_line(results, "s" + repeat_char(' ', spaces), get_real);
        (*output) << "s" << endl << "User:   ";
        print_line(results, "s" + repeat_char(' ', spaces), get_user);
        (*output) << "s" << endl << "System: ";
        print_line(results, "s" + repeat_char(' ', spaces), get_sys);
        (*output) << "s" << endl << endl;
    }
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
