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
 *    and relies on functions from the execute-process, timer, and tester    *
 *    modules.                                                               *
 *                                                                           *
 *  TO DO:                                                                   *
 *   - Add options to be more specific about the headers for timing.         *
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
    "Evaluate v1.3.2\n"
    "Copyright (C) 2014 Colin B Hamilton\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law.";

const string DESCRIPTION =
    "Evaluate:  A program for testing and timing other programs";

const string USAGE_INFORMATION =
    "Usage: evaluate [options] executable\n"
    "Or:    evaluate [options] -- executable arguments";

struct ProgramOptions {
    string program_name;
    vector<string> args;
    vector<string> inputs;
    vector<string> outputs;
    string input_dir;
    string output_dir;
    string copy_dir;
    string copy_suffix;
    string input_suffix;
    string output_suffix;
    string timer_header;
    string ignore_chars;
    bool just_test;
    bool just_time;
    bool be_quiet;
    bool be_verbose;
    bool print_ec;
    bool all_tests;
    bool all_times;
    bool avg_time;
    bool ignore_space;
    bool cp_fail;
    bool cp_all;
    unsigned times;
    unsigned max_cpu;
    unsigned max_real;
    unsigned time_precision;
    unsigned spacing;
    unsigned max_width;
};

void parse_command_line_args(int argc, char *argv[], ProgramOptions *opts);
void verify_args(ProgramOptions *opts);
void get_io(vector<string> &inputs, vector<string> &outputs, string input_dir,
            string output_dir, string input_suffix, string output_suffix);

char **vector_to_argv(string name, vector<string> *vect);
void evaluate(string name, vector<string> args,
              vector<string> inputs, vector<string> outputs,
              ProgramOptions *opts,
              Timer &tim, Tester &tes);
string make_temp_file();
void copy_output(string source, string dest_dir, string dest_ext,
                 string dest_name, bool print);
void run_one_test(string name, char **argv, string in, string temp_in,
                  string out, string temp_out,
                  vector<ProgramInfo> &result_list, ProgramOptions *opts,
                  bool print_test, unsigned &successful, Tester &tes);
bool report_test_results(Tester &tes, string input_name, int exit_code,
                         ProgramOptions *opts, unsigned &successful);

int main(int argc, char *argv[])
{
    Timer tim;
    Tester tes;
    vector<string> inputs, outputs;
    ProgramOptions opts;
    parse_command_line_args(argc, argv, &opts);
    if (opts.ignore_space) tes.ignore_whitespace();
    tes.ignore_chars(opts.ignore_chars);
    if (opts.all_times) {
        if (opts.avg_time) tim.report_all();
        else tim.dont_report_avg();
    } else if (opts.avg_time) {
        tim.report_only_avg();
    }
    tim.precision_after_decimal(opts.time_precision).spacing(opts.spacing)
       .line_width(opts.max_width);

    get_io(opts.inputs, opts.outputs, opts.input_dir, opts.output_dir,
           opts.input_suffix, opts.output_suffix);
    evaluate(opts.program_name, opts.args, opts.inputs, opts.outputs,
             &opts, tim, tes);

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
    po::options_description program("Program options");
    po::options_description general("General options");
    po::options_description testing("Testing options");
    po::options_description timing("Timing options");
    po::options_description all("All options");
    po::positional_options_description p_desc;
    program.add_options()
        ("help,h", po::value<string>()->implicit_value(""),
             "Produce help message")
        ("version,v", "Produce version, origin, and legal information")
        ("program", po::value<string>(&(opts->program_name))->required(),
            "Specify executable to run")
        ("arg", po::value< vector<string> >(&(opts->args)),
            "Specify arguments to pass to the executable")
    ;
    general.add_options()
        ("input-file,f", po::value< vector<string> >(&(opts->inputs)),
            "Specify one or more input files")
        ("output-file,F", po::value< vector<string> >(&(opts->outputs)),
            "Specify one or more output files")
        ("input-dir,d", po::value<string>(&(opts->input_dir)),
            "Specify the directory containing input files")
        ("output-dir,D", po::value<string>(&(opts->output_dir)),
            "Specify the directory containing output files")
        ("input-ext,e", po::value<string>(&(opts->input_suffix)),
            "Specify the suffix (eg. an extension) identifying input files")
        ("output-ext,E", po::value<string>(&(opts->output_suffix)),
            "Specify the suffix (eg. an extension) identifying output files")
        ("test,s", po::bool_switch(&(opts->just_test)),
            "Only run tests, do not time")
        ("time,m", po::bool_switch(&(opts->just_time)),
            "Only time, do not evaluate tests")
        ("times,t", po::value<unsigned>(&(opts->times))
                            ->default_value(1),
            "Specify number of times to run each test")
    ;
    testing.add_options()
        ("quiet,q", po::bool_switch(&(opts->be_quiet)),
            "Run quietly, only reporting failures")
        ("loud,l", po::bool_switch(&(opts->be_verbose)),
            "Run verbosely, reporting detailed results of all tests")
        ("all-tests,a", po::bool_switch(&(opts->all_tests)),
            "Report results of all tests, not just the first")
        ("ignore-space,i", po::bool_switch(&(opts->ignore_space)),
            "Ignore whitespace when testing program's output")
        ("ignore,I", po::value<string>(&(opts->ignore_chars)),
            "Specify a string containing characters to ignore "
            "when testing program's output")
        ("exit-code,C", po::bool_switch(&(opts->print_ec)),
            "Report the exit code for processes")
        ("copy-failed,y", po::bool_switch(&(opts->cp_fail)),
            "Save outputs of failed tests")
        ("copy-all,Y", po::bool_switch(&(opts->cp_all)),
            "Save outputs of all tests")
        ("copy-dir,p", po::value<string>(&(opts->copy_dir)),
            "Specify a directory in which to save copies of test output")
        ("copy-ext,P", po::value<string>(&(opts->copy_suffix)),
            "Specify a suffix (eg. an extension) to identify output copies")
    ;
    timing.add_options()
        ("max-cpu,c", po::value<unsigned>(&(opts->max_cpu))
                            ->default_value(0),
            "Set a CPU time limit for each test in seconds (0 for no limit)")
        ("max-real,r", po::value<unsigned>(&(opts->max_real))
                            ->default_value(0),
            "Set a real-time limit for each test in seconds (0 for no limit)")
        ("precision,p", po::value<unsigned>(&(opts->time_precision))
                            ->default_value(4),
            "Set decimal precision for timing output")
        ("spacing,S", po::value<unsigned>(&(opts->spacing))
                            ->default_value(2),
            "Specify spacing between columns in timing report")
        ("all-times,A", po::bool_switch(&(opts->all_times)),
            "Report results of all timing tests")
        ("avg-time,g", po::bool_switch(&(opts->avg_time)),
            "Report the average results for each timing test")
        ("max-width,w", po::value<unsigned>(&(opts->max_width))
                            ->default_value(80),
            "Specify the maximum width of a line for a better"
            " formatted timing report")
    ;
    all.add(program).add(general).add(testing).add(timing);
    p_desc.add("program", 1);
    p_desc.add("arg", -1);
    try {
        po::store(po::command_line_parser(argc, argv).
                  options(all).positional(p_desc).run(), args);
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
        string type = args["help"].as<string>();
        if (type == "general") {
            cout << general;
        } else if (type == "program") {
            cout << program;
        } else if (type == "testing") {
            cout << testing;
        } else if (type == "timing") {
            cout << timing;
        } else if (type == "all") {
            cout << all;
        } else {
            cout << DESCRIPTION << endl;
            cout << program;
            cout << "For more information on available options, "
                 << "run help with an argument:" << endl
                 << "    --help [program/general/testing/timing/all]" << endl;
        }
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
    if (!opts->all_times && !opts->avg_time) {
        opts->all_times = opts->avg_time = true;
    }
    if (opts->be_quiet && opts->be_verbose) {
        cerr << "Error in arguments: cannot be both quiet and loud!" << endl;
        exit(1);
    }
    if (opts->cp_fail || opts->cp_all &&
        (opts->copy_dir == "" || opts->copy_suffix == "")) {
        cerr << "To copy test outputs, please specify a directory or "
             << "extension for the copied files" << endl;
        exit(1);
    }
    if (!(opts->cp_fail) && !(opts->cp_all) &&
        (opts->copy_dir != "" || opts->copy_suffix != "")) {
        opts->cp_fail = true;
    }
    unsigned in_size = opts->inputs.size();
    unsigned out_size = opts->outputs.size();
    while (in_size < out_size) {
        opts->inputs.push_back("");
        ++in_size;
    }
    while (out_size < in_size) {
        opts->outputs.push_back("");
        ++out_size;
    }
    for (unsigned i = 0; i < out_size; ++i) {
        if (opts->outputs[i] == "--") {
            cerr << "Error: -- is not valid for output comparison" << endl;
            exit(1);
        }
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
    if (input_dir != "" || input_suffix != "") {
        get_files_by_suffix(temp_in, input_dir, input_suffix);
    }
    if (output_dir != "" || output_suffix != "") {
        get_files_by_suffix(temp_out, output_dir, output_suffix);
    }
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
        } else if (cur_in.filename() < cur_out.filename()
                   && cur_in.filename() != "") {
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


bool report_test_results(Tester &tes, string input_name, int exit_code,
                         ProgramOptions *opts, unsigned &successful)
{
    bool passed = false;
    if (opts->be_quiet) {
        if (!tes.run()) {
            cout << "Failed on input " << input_name;
            if (opts->print_ec) {
                cout << " with exit code " << exit_code;
            }
            cout << endl;
        } else {
            passed = true;
        }
    } else {
        string result = tes.run_verbosely();
        if (result != "") {
            cout << ">> " << result;
            if (opts->print_ec) {
                cout << endl << ">> exit code: " << exit_code;
            }
            if (result == "Passed") passed = true;
        } else {
            if (opts->be_verbose) {
                cout << "> Passed";
                if (opts->print_ec) {
                    cout << " with exit code " << exit_code;
                }
            }
            passed = true;
        }
        cout << endl;
    }
    successful += passed;
    return passed;
}


string make_temp_file()
{
    namespace fs = boost::filesystem;
    string r_val = fs::temp_directory_path().native()
                 + fs::unique_path().native() + ".eval";
    fclose(fopen(r_val.c_str(), "w"));
    return r_val;
}


void copy_output(string source, string dest_dir, string dest_ext,
                 string dest_name, bool print)
{
    namespace fs = boost::filesystem;
    if ((dest_dir == "" && dest_ext == "") || dest_name == "") return;
    fs::path destination = (dest_dir == "") ? fs::current_path() : dest_dir;
    destination /= dest_name + dest_ext;
    fs::copy_file(source, destination, fs::copy_option::overwrite_if_exists);
    if (print) {
        cout << "Output copied to " << destination.native() << endl;
    }
}


void run_one_test(string name, char **argv, string in, string temp_in,
                  string out, string temp_out,
                  vector<ProgramInfo> &result_list, ProgramOptions *opts,
                  bool print_test, unsigned &successful, Tester &tes)
{
    namespace fs = boost::filesystem;
    ProgramInfo result;
    string input_name, input_file;
    if (in == "") {
        input_name = "/no input/";
        input_file = temp_in;
    } else if (in == "--") {
        input_name = "/stdin/";
        input_file = "";
    } else {
        input_name = fs::path(in).filename().native();
        input_file = in;
    }
    if (!opts->be_quiet) {
        cout << "On input " << input_name << endl;
        cout.flush();
    }
    try {
        result = execute_process(name, argv, input_file, temp_out, "",
                                 opts->max_cpu, opts->max_real);
        result_list.push_back(result);
        if (print_test) {
            tes.set_benchmark_file(out).set_comparison_file(temp_out);
            bool good = report_test_results(tes, input_name, result.exit_code,
                                            opts, successful);
            if (opts->cp_all || (!good && opts->cp_fail)) {
                string output_name = (input_name == "/stdin/"
                                      || input_name == "")
                                   ? fs::path(out).filename().native()
                                   : input_name;
                copy_output(temp_out, opts->copy_dir, opts->copy_suffix,
                            output_name, opts->be_verbose);
            }
        }
    } catch (string err) {
        if (opts->be_quiet) cout << "On input " << input_name;
        cout << ">>> Error: " << err << endl;
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
    if (len != outputs.size()) {
        throw string("differing amounts of inputs and outputs");
    }
    char **argv = vector_to_argv(name, &args);
    unsigned successful = 0;
    string temp_input = make_temp_file();
    string temp_output = make_temp_file();
    for (unsigned i = 0; i < len; ++i) {
        TimeSet these_tests;
        these_tests.input_file = inputs[i];
        these_tests.output_file = outputs[i];
        these_tests.test_name = fs::path(inputs[i]).filename().native();
        for (unsigned j = 0; j < opts->times; ++j) {
            run_one_test(name, argv, inputs[i], temp_input,
                         outputs[i], temp_output, these_tests.runs, opts,
                         opts->just_test && (opts->all_tests || j == 0),
                         successful, tes);
        }
        results.push_back(these_tests);
    }
    if (opts->just_test) {
        cout << "Final: Passed (" << successful << "/" << len << ")" << endl;
    }
    if (opts->just_time) {
        tim.report_times(results);
    }
    fs::remove(temp_input);
    fs::remove(temp_output);
}

