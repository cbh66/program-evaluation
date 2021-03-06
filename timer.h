/*---------------------------------------------------------------------------*\
 *  timer.h                                                                  *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This file contains the interface for the Timer class, a class designed   *
 *    to translate a ProgramInfo struct into well-formatted output.          *
 *    It also contains the definition of a TimeSet struct, which can be      *
 *    useful for storing the results of a single test, run several times.    *
 *  One need only create an object of this type, call some methods, if       *
 *    desired, to determine how to format the output and what to report,     *
 *    and finally call one of the report methods.                            *
 *                                                                           *
 *  Timer::report_run() prints to cout the information stored in a single    *
 *    ProgramInfo struct.                                                    *
 *  Timer::report_time() prints to cout the information stored in a single   *
 *    TimeSet.  It precedes the report with the name of the test, input file *
 *    output file, and/or err file.  It then prints each run of the test as  *
 *    a column in a table, where the rows are wall, user, and system.  It    *
 *    can then output another column with the average time.                  *
 *  Timer::report_times() prints to cout the information stored in many      *
 *    TimeSet structs.  It simply calls Timer::report_time() on each.        *
 *                                                                           *
 *  TO DO:                                                                   *
\*---------------------------------------------------------------------------*/
#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED
#include <ostream>
#include <string>
#include <vector>
#include "execute-process.h"

struct TimeSet {
    std::vector<ProgramInfo> runs;
    std::string input_file;
    std::string output_file;
    std::string err_file;
    std::string test_name;
};

class Timer
{
public:
    Timer();

    void report_run(const ProgramInfo result);
    void report_time(const TimeSet results);
    void report_times(const std::vector<TimeSet> all_results);

    Timer &report_only_avg();
    Timer &dont_report_avg();
    Timer &report_all();
    Timer &precision_after_decimal(unsigned p);
    Timer &precision_before_decimal(unsigned p);
    Timer &line_width(unsigned w);
    Timer &spacing(unsigned n);
    Timer &set_output(std::ostream *stream);
    Timer &set_header(std::string head);
    Timer &set_footer(std::string foot);

private:
    std::string before;
    std::string after;
    bool report_avg;
    bool report_all_times;
    unsigned before_decimal;
    unsigned after_decimal;
    unsigned spaces;
    unsigned width;
    std::ostream *output;
    std::string make_header(TimeSet test);
    double print_line(const TimeSet times, int start, int end,
                      std::string seperator, std::string final,
                      double (*get_num)(ProgramInfo));
    void report_runs(const TimeSet results);
    void report_avg_alone(const TimeSet results);
    void verify_dimensions(unsigned num_tests);
    std::string repeat_char(char c, int times);
};

#endif
