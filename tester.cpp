/*---------------------------------------------------------------------------*\
 *  timer.cpp                                                                *
 *  Written By: Colin Hamilton, Tufts University                             *
 *  This implementation depends on the iostream, fstream, vector, and        *
 *    boost/lexical_cast.hpp libraries.                                      *
 *  Currently it largely compares files character-by-character.  To take the *
 *    "ignore characters" and/or whitespace into account, it utilizes the    *
 *    Tester::get_next_char() function, which reads characters until it      *
 *    finds one that is _not_ an ignore character, which it then returns.    *
 *                                                                           *
 *  TO DO:                                                                   *
 *   - Allow for more detailed error reporting by a more sophisticated       *
 *     file comparison algorithm.  It could be useful to report, e.g., a     *
 *     range of lines that failed, if the entire file didn't.  Or perhaps    *
 *     calculating something like what percentage matched.                   *\
\*---------------------------------------------------------------------------*/
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "tester.h"
using namespace std;

Tester::Tester()
{
    ignore_spaces = false;
    ignore_extras = false;
    ignore_truncation = false;
    on_success = false;
    chars_to_ignore = "";
    benchmark_file = "";
    comparison_file = "";
}


bool Tester::run()
{
    ifstream bench(benchmark_file.c_str());
    ifstream comp(comparison_file.c_str());
    char b_char = get_next_char(bench);
    char c_char = get_next_char(comp);
    while (!bench.eof() && !comp.eof()) {
        if (b_char != c_char) {
            return false;
        }
        b_char = get_next_char(bench);
        c_char = get_next_char(comp);
    }
    if (!ignore_extras && bench.eof() && !comp.eof()) {
        return false;
    }
    if (!ignore_truncation && comp.eof() && !bench.eof()) {
        return false;
    }
    return true;
}


static string readable_char(char c)
{
    string s = "'";
    switch (c) {
        case '\a':  s += "\\a";  break;
        case '\b':  s += "\\b";  break;
        case '\f':  s += "\\f";  break;
        case '\n':  s += "\\n";  break;
        case '\r':  s += "\\r";  break;
        case '\t':  s += "\\t";  break;
        case '\v':  s += "\\v";  break;
        case '\0':  s += "\\0";  break;
        default:    s += c;
    }
    s += "'";
    return s;
}

string Tester::run_verbosely()
{
    string succ = on_success ? "Passed\n" : "";
    ifstream bench(benchmark_file.c_str());
    ifstream comp(comparison_file.c_str());
    int col = 1;
    int line = 1;
    char b_char = bench.peek();
    char c_char = comp.peek();
    if (!bench.is_open() || bench.eof()) {
        if (!comp.is_open() || comp.eof()) {
            return succ;
        } else {
            c_char = get_next_char(comp, &col, &line);
            if (!comp.eof()) {
                return "Expected no output, got "
                    + readable_char(c_char)
                    + " on line " + boost::lexical_cast<string>(line)
                    + " col " + boost::lexical_cast<string>(col) + "\n";
            } else {
                return succ;
            }
        }
    }
    if (!comp.is_open() || comp.eof()) {
        b_char = get_next_char(bench);
        if (!bench.eof()) {
            return "Expected " + readable_char(b_char)
                + ", got no output\n";
        } else {
            return succ;
        }
    }

    b_char = get_next_char(bench);
    c_char = get_next_char(comp, &col, &line);
    while (!bench.eof() && !comp.eof()) {
        ++col;
        if (b_char != c_char) {
            return "Expected " + readable_char(b_char)
                + ", got " + readable_char(c_char)
                + " on line " + boost::lexical_cast<string>(line)
                + " col " + boost::lexical_cast<string>(col) + "\n";
        }
        if (b_char == '\n') {
            ++line;
            col = 1;
        }
        b_char = get_next_char(bench);
        c_char = get_next_char(comp, &col, &line);
    }

    if (bench.eof() && !comp.eof()) {
        return "Got more output than expected: did not expect "
            + readable_char(c_char) + " on line "
            + boost::lexical_cast<string>(line) + " col "
            + boost::lexical_cast<string>(col) + "\n";
    }
    if (comp.eof() && !bench.eof()) {
        return "Got less output than expected: expected additional "
            + readable_char(b_char) + " on line "
            + boost::lexical_cast<string>(line) + " col "
            + boost::lexical_cast<string>(col) + "\n";
    }
    return succ;
}

/* Function under consideration.
 *   If implemented, it would work very much like a diff of the files.
 * Of course, it's not a huge priority, as users can always call diff
 *   themselves once they see what failed, if they want to know the details.
 * Also, this function could just be implemented with a fork-exec of diff.
 * Granted, that assumes diff is installed on the machine.
string Tester::run_detailed()
{
      Current ideas:
         Read both files in as two strings.
         Compare them char-by-char until you find a difference.
         Then, find the longest common substring of the two strings.
         Recurse on the elements before and after this substring,
           terminating when no elements are in common.
         Once you find these, return the set of substrings that
           differ, and translate into an error report.
}
*/

Tester &Tester::set_benchmark_file(string filename)
{
    benchmark_file = filename;
    return *this;
}


Tester &Tester::set_comparison_file(string filename)
{
    comparison_file = filename;
    return *this;
}


Tester &Tester::ignore_whitespace()
{
    ignore_spaces = true;
    return *this;
}


Tester &Tester::consider_whitespace()
{
    ignore_spaces = false;
    return *this;
}


Tester &Tester::ignore_char(char c)
{
    chars_to_ignore += c;
    return *this;
}


Tester &Tester::ignore_chars(string s)
{
    chars_to_ignore += s;
    return *this;
}


Tester &Tester::heed_char(char c)
{
    string result = "";
    unsigned len = chars_to_ignore.length();
    for (unsigned i = 0; i < len; ++i) {
        if (chars_to_ignore[i] != c) result += chars_to_ignore[i];
    }
    chars_to_ignore = result;
    return *this;
}


Tester &Tester::heed_all_chars()
{
    chars_to_ignore = "";
    ignore_spaces = false;
    return *this;
}


Tester &Tester::ignore_extra_chars()
{
    ignore_extras = true;
    return *this;
}


Tester &Tester::ignore_too_few_chars()
{
    ignore_truncation = true;
    return *this;
}


Tester &Tester::expect_proper_length()
{
    ignore_truncation = false;
    ignore_extras = false;
    return *this;
}


Tester &Tester::print_on_success()
{
    on_success = true;
    return *this;
}


bool Tester::is_ignore_char(char c)
{
    if (ignore_spaces && !isgraph(c)) return true;
    if (chars_to_ignore.find(c) != string::npos) return true;
    return false;
}


char Tester::get_next_char(ifstream &str)
{
    int x = 0;
    return get_next_char(str, &x, &x);
}


char Tester::get_next_char(ifstream &str, int *cols, int *lines)
{
    char c = str.get();
    while (!str.eof() && c != EOF && is_ignore_char(c)) {
        ++(*cols);
        if (c == '\n') {
            ++(*lines);
            *cols = 0;
        }
        c = str.get();
    }
    if (c == EOF) {
        str.clear(ios::eofbit);
    }
    return c;
}
