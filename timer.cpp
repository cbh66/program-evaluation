/*---------------------------------------------------------------------------*\
 *  timer.cpp                                                                *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This implementation depends on the string, iostream, and iomanip         *
 *    libraries.                                                             *
 *  TO DO:                                                                   *
 *   - Fix formatting of report_time() - instead of printing literal         *
 *     strings, determine dynamically how long they ought to be, and how     *
 *     many spaces to print.  Especially when the user can specify the       *
 *     amount of precision in the decimals.                                  *
 *   - Report and/or calculare average time for each test.  Maybe have each  *
 *     print_ function also take in a bool for whether to do so.             *
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
}


void Timer::report_run(ProgramInfo result)
{

}



void Timer::print_reals(const TimeSet times, const string seperator)
{
    int size = times.runs.size();
    string btwn = "";
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        double t =  (double)times.runs[i].wall_sec
                    + ((double)times.runs[i].wall_usec / 1000000.0);
        cout << btwn << setw(before_decimal + 1 + after_decimal) << std::right
             << t;
        sum += t;
        btwn = seperator;
    }
    cout << btwn << setw(before_decimal + 1 + after_decimal) << std::right
         << (sum / size);
}

void Timer::print_users(const TimeSet times, const string seperator)
{
    int size = times.runs.size();
    string btwn = "";
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        double t =  (double)times.runs[i].user_sec
                    + ((double)times.runs[i].user_usec / 1000000.0);
        cout << btwn << setw(before_decimal + 1 + after_decimal)
             << t;
        sum += t;
        btwn = seperator;
    }
    cout << btwn << setw(before_decimal + 1 + after_decimal) << std::right
         << (sum / size);
}

void Timer::print_syses(const TimeSet times, const string seperator)
{
    int size = times.runs.size();
    string btwn = "";
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        double t =  (double)times.runs[i].sys_sec
                    + ((double)times.runs[i].sys_usec / 1000000.0);
        cout << btwn << setw(before_decimal + 1 + after_decimal) << std::right
            << t;
        sum += t;
        btwn = seperator;
    }
    cout << btwn << setw(before_decimal + 1 + after_decimal) << std::right
         << (sum / size);
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
        cout << results.input_file << endl;
        cout << repeat_char(' ', 8);
        if (report_all_times) {
            for (int j = 0; j < size; ++j) {
                cout << "TRIAL " << j;
                cout << repeat_char(' ',
                            (before_decimal + 2 + after_decimal + spaces)
                                - 7);
            }                               // Or minus (6 + #digits in j)
        }
        if (report_avg) cout << "  AVG" << endl;
        cout << "Real:   ";
        print_reals(results, "s" + repeat_char(' ', spaces));
        cout << "s" << endl << "User:   ";
        print_users(results, "s" + repeat_char(' ', spaces));
        cout << "s" << endl << "System: ";
        print_syses(results, "s" + repeat_char(' ', spaces));
        cout << "s" << endl << endl;
    }
}


void Timer::report_times(const vector<TimeSet> all_results)
{
    vector<TimeSet>::const_iterator it = all_results.begin();
    vector<TimeSet>::const_iterator end = all_results.end();
    cout.setf(ios::fixed);
    cout.precision(after_decimal);
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
