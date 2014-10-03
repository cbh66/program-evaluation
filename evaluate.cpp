/*---------------------------------------------------------------------------*\
 *  evaluate.cpp                                                             *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This file should be supplied with a Makefile.  The "make" command should *
 *    compile it.  If the Makefile is not there, you can run                 *
 *    g++ evaluate.cpp execute-process.cpp timer.cpp tester.cpp \            *
 *        -lboost_program_options -lboost_filesystem -lboost_system \        *
 *        -o evaluate                                                        *
 *                                                                           *
 *  As that command indicates, this program relies on three boost libraries: *
 *    boost::program_options, boost::filesystem, and boost::system.          *
 *    The most recent version of the libraries can be downloaded from        *
 *    http://www.boost.org                                                   *
 *                                                                           *
 *  This main file primarily evaluates program options and finds specified   *
 *    files.  The evaluate() function contains the core of the program,      *
 *    and relies on functions for the execute-process, timer, and tester     *
 *    modules.                                                               *
 *                                                                           *
 *  TO DO:                                                                   *
 *   - Add options to add individual input and output files                  *
 *   - Add options for outputting individual times, avg times, or both.      *
\*---------------------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "execute-process.h"
#include "timer.h"
#include "tester.h"
using namespace std;


const string VERSION_INFORMATION =
    "Evaluate v1.2.0\n"
    "Copyright (C) 2014 Colin B Hamilton\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.";

const string USAGE_INFORMATION =
    "Usage: get-timing-stats [options] executable";

struct ProgramOptions {
    string program_name;
    vector<string> args;
    string input_dir;
    string output_dir;
    string input_suffix;
    string output_suffix;
    bool just_test;
    bool just_time;
    bool be_quiet;
    bool be_verbose;
    bool report_all;
    unsigned times;
    unsigned max_time;
    unsigned time_precision;
    unsigned spacing;
};

void parse_command_line_args(int argc, char *argv[], ProgramOptions *opts);
void verify_args(ProgramOptions *opts);

char **vector_to_argv(string name, vector<string> *vect);
void evaluate(string name, vector<string> args,
              vector<string> inputs, vector<string> outputs,
              ProgramOptions *opts,
              Timer &tim, Tester &tes);
void get_io(vector<string> &inputs, vector<string> &outputs, string input_dir,
            string output_dir, string input_suffix, string output_suffix);

int main(int argc, char *argv[])
{
    Timer tim;
    Tester tes;
    vector<string> inputs, outputs;
    ProgramOptions opts;
    parse_command_line_args(argc, argv, &opts);
    tes.ignore_whitespace();
    if (opts.be_verbose) tes.print_on_success(); 
    tim.precision_after_decimal(opts.time_precision).spacing(opts.spacing);

    get_io(inputs, outputs, opts.input_dir, opts.output_dir,
           opts.input_suffix, opts.output_suffix);
    evaluate(opts.program_name, opts.args, inputs, outputs, &opts, tim, tes);

    return 0;
}

/*  parse_command_line_args()
 *  Goes through the array of arguments and sets appropriate values in the
 *    structure pointed to by opts.
 */
void parse_command_line_args(int argc, char *argv[], ProgramOptions *opts)
{
    namespace po = boost::program_options;
    po::variables_map args;
    po::options_description generic("Generic options");
    po::positional_options_description p_desc;
    generic.add_options()
        ("help,h", "Produce help message")
        ("version,v", "Produce version, origin, and legal information")
        ("program", po::value<string>(&(opts->program_name))->required(),
            "Specify executable to run")
        ("arg", po::value< vector<string> >(&(opts->args)),
            "Specify arguments to pass to the executable")
        ("input-dir,d", po::value<string>(&(opts->input_dir)),
            "Specify the directory containing input files")
        ("output-dir,D", po::value<string>(&(opts->output_dir)),
            "Specify the directory containing input files")
        ("input-ext,e", po::value<string>(&(opts->input_suffix)),
            "Specify the suffix (eg. an extension) identifying input files")
        ("output-ext,E", po::value<string>(&(opts->output_suffix)),
            "Specify the suffix (eg. an extension) identifying output files")
        ("test,s", po::bool_switch(&(opts->just_test)),
            "Only run tests, do not time")
        ("time,m", po::bool_switch(&(opts->just_time)),
            "Only time, do not evaluate tests")
        ("quiet,q", po::bool_switch(&(opts->be_quiet)),
            "Run quietly, only reporting failures")
        ("loud,l", po::bool_switch(&(opts->be_verbose)),
            "Run verbosely, reporting detailed results of all tests")
        ("times,t", po::value<unsigned>(&(opts->times))
                            ->default_value(1),
            "Specify number of times to run each test")
        ("report-all,a", po::bool_switch(&(opts->report_all)),
            "Report results of all tests, not just the first")
        ("max-time,m", po::value<unsigned>(&(opts->max_time))
                            ->default_value(0),
            "Set a time limit for each test in seconds (0 for no limit)")
        ("precision,p", po::value<unsigned>(&(opts->time_precision))
                            ->default_value(4),
            "Set decimal precision for output of timing")
        ("spacing,S", po::value<unsigned>(&(opts->spacing))
                            ->default_value(2),
            "Specify spacing between columns in timing report")
    ;
    p_desc.add("program", 1);
    p_desc.add("arg", -1);
    try {
        po::store(po::command_line_parser(argc, argv).
                  options(generic).positional(p_desc).run(), args);
        po::notify(args);
    } catch (exception &err) {
        if (!args.count("help") && !args.count("version")) {
            cerr << "Error in arguments: " << err.what() << endl;
            cerr << "(For help, use the --help or -h options)" << endl;
            exit(1);
        }
    }
    if (args.count("help")) {
        cout << USAGE_INFORMATION << endl;
        cout << generic;
        exit(0);
    }
    if (args.count("version")) {
        cout << VERSION_INFORMATION << endl;
        exit(0);
    }
    verify_args(opts);
}


void verify_args(ProgramOptions *opts)
{
    if (!opts->just_test && !opts->just_time) {
        opts->just_test = opts->just_time = true;
    }
    if (opts->be_quiet && opts->be_verbose) {
        cerr << "Error in arguments: cannot be both quiet and loud!" << endl;
        exit(1);
    }
}


/*  vector_to_argv()
 *  Takes the name of a program and a vector of its arguments, as strings,
 *    and converts them into standard argv format (that expected by the exec
 *    family of functions).  That is, it is a NULL-terminated array of
 *    null-terminated character arrays, where the first contains the name
 *    of the program, and the rest contain the program's arguments.
 */
char **vector_to_argv(string name, vector<string> *vect)
{
    vector<char *> temp_vect;
    char *copy = new char[name.size()];
    temp_vect.push_back(strcpy(copy, name.c_str()));
    unsigned end = vect->size();
    for (unsigned i = 0; i < end; ++i) {
        copy = new char[vect->at(i).size()];
        temp_vect.push_back(strcpy(copy, vect->at(i).c_str()));
    }
    temp_vect.push_back(NULL);
    char **argv = new char *[temp_vect.size()];
    end = temp_vect.size();
    for (unsigned i = 0; i < end; ++i) {
        argv[i] = temp_vect[i];
    }
    return argv;
}

void print_results(ProgramInfo results)
{
    double wall =  (double)results.wall_sec
                    + ((double)results.wall_usec / 1000000.0);
    double user = (double) results.user_sec
                    + ((double)results.user_usec / 1000000.0);
    double sys  = (double) results.sys_sec
                    + ((double)results.sys_usec  / 1000000.0);
    cout.precision(4);
    cout.setf( std::ios::fixed, std:: ios::floatfield );
    cout << "Wall: " << wall << endl;
    cout << "User: " << user << endl;
    cout << "Sys:  " << sys  << endl;
    cout << "Exit code: " << results.exit_code << endl;
}


void get_files_by_suffix(vector<boost::filesystem::path> &output,
                         string dir, string suffix)
{
    namespace fs = boost::filesystem;
    fs::directory_iterator end;
    fs::directory_iterator i;
    if (dir != "") {
        fs::path d(dir);
        if (d.is_absolute()) i = fs::directory_iterator(d);
        else i = fs::directory_iterator(fs::current_path() / d);
    }
    unsigned suffix_len = suffix.length();
    for ( ; i != end; ++i) {
        string path = i->path().generic_string();
        unsigned path_len = path.length();
        if (path_len >= suffix_len) {
            if (path.substr(path_len - suffix_len) == suffix) {
                output.push_back(path.substr(0, path_len - suffix_len));
            }
        }
    }
}


void get_io(vector<string> &inputs, vector<string> &outputs, string input_dir,
            string output_dir, string input_suffix, string output_suffix)
{
    namespace fs = boost::filesystem;
    vector<fs::path> temp_in, temp_out;
    get_files_by_suffix(temp_in, input_dir, input_suffix);
    get_files_by_suffix(temp_out, output_dir, output_suffix);
    unsigned i_len = temp_in.size();
    unsigned o_len = temp_out.size();
    unsigned i = 0, j = 0;
    /*  Look for file names that correspond.  Add names in parallel to the
     *  vectors.  If names are found without a match, push the empty string
     *  to the other vector.
     */
    while (i < i_len || j < o_len) {
        fs::path cur_in = (i < i_len) ? temp_in[i] : "";
        fs::path cur_out = (j < o_len) ? temp_out[j] : "";
        if (cur_in.filename() == cur_out.filename()) {
            inputs.push_back(cur_in.generic_string() + input_suffix);
            outputs.push_back(cur_out.generic_string() + output_suffix);
            ++i, ++j;
        } else if (cur_in.filename() < cur_out.filename() && cur_in.filename() != "") {
            inputs.push_back(cur_in.generic_string() + input_suffix);
            outputs.push_back("");
            ++i;
        } else {
            outputs.push_back(cur_out.generic_string() + output_suffix);
            inputs.push_back("");
            ++j;
        }
    }
}


void evaluate(string name, vector<string> args,
              vector<string> inputs, vector<string> outputs,
              ProgramOptions *opts,
              Timer &tim, Tester &tes)
{
    namespace fs = boost::filesystem;
    vector<TimeSet> results;
    unsigned len = inputs.size();
    string temp_file = fs::temp_directory_path().native()
                            + fs::unique_path().native() + ".eval";
    if (len != outputs.size()) {
        throw string("differing amounts of inputs and outputs");
    }
    for (unsigned i = 0; i < len; ++i) {
        TimeSet these_tests;
        for (unsigned j = 0; j < opts->times; ++j) {
            try {
                these_tests.runs.push_back(execute_process(name,
                                                vector_to_argv(name, &args),
                                                inputs[i], temp_file, "",
                                                opts->max_time));
                if (opts->just_test && (opts->report_all || j == 0)) {
                    tes.set_benchmark_file(outputs[i])
                       .set_comparison_file(temp_file);
                    if (opts->be_quiet) {
                        if (!tes.run()) {
                            cout << "Failed on input "
                                 << fs::path(inputs[i]).filename().native()
                                 << endl;
                        }
                    } else {
                        string result = tes.run_verbosely();
                        if (result != "") {
                            cout << "On input "
                                 << fs::path(inputs[i]).filename().native()
                                 << ": " << endl << result << endl;
                        }

                    }
                }
            } catch (string err) {
                cerr << "Error on input " << fs::path(inputs[i]).filename().native()
                     << ": " << err << endl;
            }
        }
        results.push_back(these_tests);
        results.back().input_file = inputs[i];
        results.back().output_file = outputs[i];
    }
    if (opts->just_time) {
        tim.report_times(results);
    }
}

