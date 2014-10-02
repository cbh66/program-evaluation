#include <iostream>
#include <iomanip>
#include "timer.h"
using namespace std;


Timer::Timer()
{
    report_all();
}


void Timer::report_run(ProgramInfo result)
{

}



static void print_reals(const TimeSet times, const string seperator)
{
    int size = times.runs.size();
    string btwn = "";
    for (int i = 0; i < size; ++i) {
        double t =  (double)times.runs[i].wall_sec
                    + ((double)times.runs[i].wall_usec / 1000000.0);
        cout << btwn << t;
        btwn = seperator;
    }
}

static void print_users(const TimeSet times, const string seperator)
{
    int size = times.runs.size();
    string btwn = "";
    for (int i = 0; i < size; ++i) {
        double t =  (double)times.runs[i].user_sec
                    + ((double)times.runs[i].user_usec / 1000000.0);
        cout << btwn << t;
        btwn = seperator;
    }
}

static void print_syses(const TimeSet times, const string seperator)
{
    int size = times.runs.size();
    string btwn = "";
    for (int i = 0; i < size; ++i) {
        double t =  (double)times.runs[i].sys_sec
                    + ((double)times.runs[i].sys_usec / 1000000.0);
        cout << setw(8) << std::right << btwn << t;
        btwn = seperator;
    }
}

void Timer::report_time(const TimeSet results)
{
    int size = results.runs.size();
    if (size > 0) {
        cout << results.input_file << endl;
        for (int j = 0; j < size; ++j) {
            cout << "        TRIAL " << j;
        }
        cout << "       AVG" << endl;
        cout << "Real:   ";
        print_reals(results, "s      ");
        cout << "s" << endl << "User:   ";
        print_users(results, "s      ");
        cout << "s" << endl << "System: ";
        print_syses(results, "s      ");
        cout << "s" << endl << endl;
    }
}


void Timer::report_times(const vector<TimeSet> all_results)
{
    vector<TimeSet>::const_iterator it = all_results.begin();
    vector<TimeSet>::const_iterator end = all_results.end();
    cout.setf(ios::fixed);
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
