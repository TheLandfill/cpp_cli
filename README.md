# cpp_cli
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/7f047f6cbe31451f87096d6a64e277fa)](https://www.codacy.com/app/TheLandfill/cpp_cli?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=TheLandfill/cpp_cli&amp;utm_campaign=Badge_Grade)

This repository consists of a library designed to make parsing command line arguments for c++ easy and efficient and a simple program showing you how it works. While there exist similar libraries such as [GetOpt](https://www.gnu.org/software/libc/manual/html_node/Getopt.html), [commandline](https://archive.codeplex.com/?p=commandline), [CLI11](https://github.com/CLIUtils/CLI11), and [opt](http://www.decompile.com/not_invented_here/opt/), these libraries are either designed with different goals in mind, do far too much, or use weird or complicated syntax. This repository intends to make command line parsing as simple and easy as possible for the user and for the code itself to be easy to read and improve.

Currently, this library is in the beta stage. While it is stable, there are a few improvements that could be made. I'll list these improvements in the section "Goals."

While the source code itself is standard c++11, the test program's Makefile uses the GNU/Linux tools make and g++.

## Table of Contents
1.  [Why I Wrote This Library](#why-i-wrote-this-library)

2.  [Getting Started](#getting-started)
    
    1.  [Prerequisites](#prerequisites)
    
    2.  [Install](#install)
    
3.  [How to Use](#how-to-use)

    1.  [Syntax of Use](#syntax-of-use)
    
4.  [Parsing Rules](#parsing-rules)

    1.  [How Parsing Works With Subcommands](#how-parsing-works-with-subcommands)
    
    2.  [Types the Library Can Handle](#types-the-library-can-handle)
    
        1.  [How to Handle a `char` Array](#how-to-handle-a-char-array)
	
    3.  [Using Options Whose Locations Matter](#using-options-whose-locations-matter)
    
5.  [Exception Throwing](#exception-throwing)

6.  [Example Usage](#example-usage)

7.  [Help Message](#help-message)

8.  [More Complex Command Line Parsing](#more-complex-command-line-parsing)

    1.  [Subcommands](#subcommands)
    
        1.  [Subcommands Example](#subcommands-example)
	    
    2.  [`Command_Line_Value`s](#command_line_values)
    
    3.  [W_SPECIALIZATION](#w_specialization)
    
        1.  [W_SPECIALIZATION Example](#w_specialization-example)
	
    4.  [Adding Your Own Extensions](#adding-your-own-extensions)
    
9.  [Goals](#goals)

10. [Goals Completed](#goals-completed)

11. [License](#license)

## Why I Wrote This Library
-   Every non-trivial program has to parse command line arguments, which leaves programmers often writing their own parsers for each individual project even though they are often writing the same inefficient, rigid, and unnecessarily complex algorithms.

-   In my opinion, current CLI parsers either do far too little, such as GetOpt, or far too much, such as CLI11. Programs should do one thing and do it well. This library takes the data in the command line and puts it into your variables.

-   I wanted to contribute to the open source community and familiarize myself with GitHub.

-   A little bit of [Not Invented Here](https://en.wikipedia.org/wiki/Not_invented_here), to be honest. I still think what I have written is easier to use and much more robust than other libraries, but they likely think the same thing about this library.

    -   After actually using some of the other competitors to test how my library stacks up to theirs, I'm starting to remember more why I made this library.
    -   For starters, I don't include half the STL to parse the command line, which keeps executables much smaller than other CLI libraries.	
    -   This library is also way more flexible than most other libraries. It understands that it's only job is organize data on the command line so that it becomes much easier for you to read (and also document itself through the automatic help generation).

## Getting Started
As it currently stands, the header file will work on all systems, but the test program will have to be compiled using a project on Windows.

### Prerequisites
The library requires nothing but c++11. The test program already has symlinks to the required library and header file.

### Install
No installation required. Just download the [folder](src/cpp_command_line_parser/) containing the header files "parser.h", "hash_table.h", and "args_parser_templates.h" and add it to your list of include directories.

## How to Use
1.  Add the include directory in the project settings or in the Makefile
2.  Include the header file "parser.h" in the main part of your program.
3.  Create a new scope (this is just so the local variables you need to create disappear, but it's not necessary).
4.  Inside that scope, create your `Command_Line_Var<T>`s, create your `Command_Line_Value<T>`s, add your subcommands, and set help information (syntax specified below).
5.  If you want to generate a help message, run the function `ARGS_PARSER::generate_help()`, which will create a text file with a help message.
6.  Once you've created all the `Command_Line_Var`s, run the function `ARGS_PARSER::parse(argc, argv);`.
7.  All the variables will be set after hash finishes.
8.  Any non-flagged argument or ignored argument will be returned in a vector of "non_options" in the order in which they appear in the command line.

### Syntax of Use
```cpp
// WILL WORK IN ALL CASES, INCLUDING FOR Command_Line_Var<char>.
Command_Line_Var<T> variable_name_var(variable_name, { "flag1", "flag2", "f" }, takes_args, "Optional help message");
// DEPRECATED, but will still work for everything except Command_Line_Var<char>. The ampersand doesn't really make a difference otherwise.
Command_Line_Var<T> variable_name_var(&variable_name, { "flag1", "flag2", "f" }, takes_args, "Optional help message");

// Same as the Command_Line_Var, except the third argument is the value you want the variable to be set to
// if any of the flags are found.
Command_Line_Value<T> variable_name_var(variable_name, { "flag1", "flag2", "f" }, value, "Optional help message");

// Adds a subcommand, which will run the second argument if name_of_subcommand is found. The second argument is
// a function with syntax void function_name(int argc, char ** argv, void * data);
ARGS_PARSER::add_subcommand("name_of_subcommand", subcommand, "Optional help message.");

// The help message will have the usage printed first, then the header, then the subcommands (if any), then the options, then the footer.
ARGS_PARSER::set_usage("PROGRAM_NAME [options] non-option0 non-option1");
ARGS_PARSER::set_header("Here is a description of what your program does and so on.");
ARGS_PARSER::set_footer("For more information, contact us at the.landfill.coding@gmail.com.");

// This will generate the help message and store it in a file.
ARGS_PARSER::generate_help();

ARGS_PARSER::parse(argc, argv);
```
where `T` is the type of the variable and `variable_name_var` is a stack allocated variable. The first argument is either the variable you want to set, its address (as long as it isn't a `Command_Line_Var<char>`), or a `nullptr`, the second variable is an array/vector of strings corresponding to flags that control the value of the variable, the third argument is a bool that determines whether or not the flags take arguments themselves, and the last argument is an optional help message. If you don't provide one, the flags will still show up in the help message so that you don't forget about documenting a help message. To have a set of flags ignored, pass in anything starting with a backtick (\`) for the help message.

If a `nullptr` is provided for the first argument, the parser will just treat it as if it were a non-option.

## Parsing Rules
[This answer](https://stackoverflow.com/a/14738273/6629221) on stackexchange does a good job of summarizing the standard for command line argument syntax, and the library follows these rules, which are copied below for convenience.

> -   Arguments are divided into options and non-options. Options start with a dash, non-options don't.
>
> -   Options, as the name implies, are supposed to be optional. If your program requires some command-line arguments to do anything at all useful, those arguments should be non-options (i.e. they should not start with a dash).
>
> -   Options can be further divided into short options, which are a single dash followed by a single letter (-r, -f), and long options, which are two dashes followed by one or more dash-separated words (--recursive, --frobnicate-the-gourds). Short options can be glommed together into one argument (-rf) as long as none of them takes arguments (see below).
>
>     -   Options may themselves take arguments.
>
>     -   The argument to a short option -x is either the remainder of the argv entry, or if there is no further text in that entry, the very next argv entry whether or not it starts with a dash.
>
> -   The argument to a long option is set off with an equals sign: --output=outputfile.txt.
>
> -   If at all possible, the relative ordering of distinct options (with their arguments) should have no observable effect.
>
> -   The special option -- means "do not treat anything after this point on the command line as an option, even if it looks like one." This is so, for instance, you can remove a file named '-f' by typing rm -- -f.
>
> -   The special option - means "read standard input".
>
> -   There are a number of short option letters reserved by convention: the most important are
>
>     -   \-v = be verbose
>
>     -   \-q = be quiet
>
>     -   \-h = print some help text
>
>     -   \-o file = output to file
>
>     -   \-f = force (don't prompt for confirmation of dangerous actions, just do them)

Note that this library does not force you to use any of the commonly reserved short options at the bottom of the list, nor does it treat them any differently than any other options, nor does it reserve them. It is up to the user to maintain this standard. Furthermore, the special option "-" is treated just like any other option, so it is not reserved for standard input either. Finally, the special argument "--" will turn any arguments that come after it into non-options.

### How Parsing Works With Subcommands

When any word that can be identified as a valid subcommand shows up, the parser will then call that subcommand, add a `nullptr` to the list of non_options, then add the subcommand, and then it will then run the subcommand. Each subcommand has its own totally independent set of flags, but they all share the same non_options. This functionality is modeled after the functionality of the `git` command and its subcommands.

### Types the Library Can Handle
As it currently stands, this library can handle standard types that can be converted from a char \*, which include all numeric types, std::string, and char \* (char \* has a slightly different syntax which will be discussed below). To extend the library to handle other types, you need to either add a template specialization, which is what I have done for the numeric types, or overload the "=" operator to take in char \*, which is what std::string has done.

This library, however, does not natively support bool types, as it prefers to set a variable and check if it was set or what it was set to.

When using an argument that does not take subarguments, the variable will be set to whatever the subargument is. For instance, if the flag "-s" does not take arguments, it will set its corresponding variable to "s". Likewise, "--stop-early" will set its corresponding variable to "stop-early". If the argument can be stacked (such as -vvvv), then it will set its variable to "vvvv" for up to eleven stacked characters.

The template specialization itself takes the form
```cpp
template<>
void Command_Line_Var<T>::set_base_variable(const char * b_v) {
    // Do stuff
}
```
while the default template version takes the form
```cpp
virtual void set_base_variable(const char * b_v) {
    *(T *)base_variable = b_v;
}
```

#### How to Handle a `char` Array
Instead of creating a `Command_Line_Var<char *>` with the address of the variable containing the char \*, you should create a `Command_Line_Var<char>` with the char \* as its first argument. It also has a fourth argument which sets the buffer size, and so it cannot have a segmentation fault so long as you have allocated more than the buffer size you provide.

```cpp
char example_string[20];
// INVALID: Does not accept char * as a type and the first argument should be a char *, not a char **.
Command_Line_Var<char *> example_string_var(&example_string, ..., ..., 20);
// INVALID: You have only allocated 20 bytes but say you can hold 200 in example_string.
Command_Line_Var<char> example_string_var(example_string, ..., ..., 200);
// VALID
Command_Line_Var<char> example_string_var(example_string, ..., ..., 20);
// VALID
Command_Line_Var<char> example_string_var(example_string, ..., ..., 10);
```

Furthermore, you should allocate enough memory to store the longest argument you expect to receive. If you do not, you run the risk of a segmentation fault, and there is nothing the library can do to fix it or notify you that your buffer is too small.

Note that `std::string`s do not have this problem, as they manage their own buffer sizes.

If you just want to set a singular `char`, passing it in as a normal variable works.

### Using Options Whose Locations Matter
Without going into too much detail, the `-l` flag in `gcc` and `g++` has to come after other arguments in gcc, but my library destroys the order of options in most cases. To keep a flag in the list of non-options, provide `nullptr` for the first argument in the Command_Line_Var constructor where the address of a variable would normally go. For example:
```cpp
Command_Line_Var<int> example_ignored_variable(nullptr, { "l", "library"}, true);
```
Any flag that starts with `-l` or `--library` will be considered as if it were a non-option. Note that the templated type was an int in this example, but the type doesn't matter. If you wanted to do a `char`, you would have to provide a fourth argument that would not be used. If you set the third argument to false, it will convert `--library=something` to `--library\0something`, where `\0` is the null terminator, but nothing else will happen.

Having a `-l` in a group of multiple short arguments such as `-albc` will throw an exception identifying the flag.

## Exception Throwing
The library will throw exceptions (std::invalid_argument) when you provide a flag on the command line that you did not specify (except for the single hyphen flag for standard input), provide an argument to a flag that does not take arguments, leave out an argument to a flag that does take arguments, or try to use a short option whose location on the command line matters inside a group of short options. When the library throws an argument, it will tell you the error and which flag caused the error. The exception will only print around the first 32 characters of the flag that caused the error.

## Example Usage
```cpp
#include "parser.h"

int main(int argc, char ** argv){
  std::string filename = "";
  int recursion_level = 0;
  std::string show_output = "";
  std::string standard_input = "";
  char c_version_of_string[20];
  std::vector<const char *> non_options;

  {
    Command_Line_Var<std::string> filename_var(filename, { "f", "filename", "file" }, true);
    Command_Line_Var<int> recursion_level_var(recursion_level, { "r", "recursion", "max_depth" }, true);
    Command_Line_Var<std::string> show_output_var(show_output, { "show_output", "s", "no_out", "half_out" }, false);
    
    // A single hyphen means standard input, but it doesn't have to. If you want, add the line
    Command_Line_Var<std::string> standard_input_var(standard_input, { "-" }, false);
    
    Command_Line_Var<char> c_version_of_string_var(c_version_of_string, { "v" }, false, 20);

	// The third argument and the fourth argument aren't necessary to set in this simple example.
    non_options = ARGS_PARSER::parse(argc, argv);
  }
  
  // Other code. At this point, all variables are set.
}
```
You can't get much simpler than two lines per variable (half of which are just initializing the variable anyway), but if you want to do more complicated things, you will have to get more complicated.

## Help Message
This library can automatically generate a help message by calling `ARGS_PARSER::generate_help()`, which will generate a help message and store it in a file within the same directory as the executable named ".X_help_message", where "X" is either the name of the current subcommand or "main" if no subcommand has been called. To print out the current help message, use `ARGS_PARSER::print_help()`, which will print out the last help message of the last subcommand run. It will only generate the help message if there is no help message file corresponding to the current subcommand, meaning you should delete all help message files on compiling. This library has no automatic trigger for a help message, so you'll still need to create a `Command_Line_Value<bool>` for each help message display. You may need to set the filename if you want a help message from a supercommand to be displayed. If `ARGS_PARSER::print_help()` is called without calling `ARGS_PARSER::generate_help()`, the program will print an error message to the stderr detailing which subcommand needs to have the generate_help() added.

## More Complex Command Line Parsing

### Subcommands

Subcommands are essentially commands embedded inside a single command. The classic example of a command with subcommands is `git`, which has a huge list of subcommands that do completely different things but all use the same functionality of `git`. In this library, each subcommand is treated like a different main function that allows you to pass data from a supercommand into it. Other than the data passed to the subcommand (or data shared by a class if the supercommand and the subcommand have access to the same data), each subcommand is entirely different from the rest of the program, including its supercommand, its subcommands, and any other subcommand, meaning that each subcommand can also have different flags, the same flags, the same flags but referring to different things, etc. Subcommands are declared with the syntax `ARGS_PARSER::add_subcommand("Command Name", subcommand_function)`, where `void subcommand_function(int argc, char ** argv, void * data)` is the style of the subcommand function. It is important to note that each subcommand will treat itself as if it were its own program and completely ignore everything before it on the command line.

#### Subcommands Example

```cpp
#include "parser.h"

void push_subcommand(int argc, char ** argv, void * data);
void pull_subcommand(int argc, char ** argv, void * data);

struct Data_From_Principle_Subcommand {
	std::string file_path;
	int level;
};


int main(int argc, char ** argv) {
	Data_For_Push_Subcommand dfps;

	ARGS_PARSER::add_subcommand("push", push_subcommand);
	ARGS_PARSER::add_subcommand("pull", pull_subcommand);
	// Other stuff like flags
	Command_Line_Var<std::string> file_path_var(dfps.file_path, { "p", "path" }, true);
	Command_Line_Vat<int> level_var(dfps.level, { "l", "level" }, true);
	
	// A void pointer to dfps will be passed to any subcommand used.
	ARGS_PARSER::parse(argc, argv, &dfps, 100);
}

void push_subcommand(int argc, char ** argv, void * data_ptr) {
	Data_For_Push_Subcommand data = *((Data_For_Push_Subcommand *)data_ptr);
	// Do whatever
}
```

### `Command_Line_Value`s

A `Command_Line_Value` has the same syntax as the `Command_Line_Var`, except the third argument is what you want the value to be set to when the flag appears. For instance: `Command_Line_Value<char> some_var(some, { "some", "not-nothing", "s", "less-than-all" }, 's')` will set `some` to `'s'` if any of the flags in the list are found. These are better suited to options that do not take args than `Command_Line_Var`s.

### W_SPECIALIZATION

This is where things get more complicated, though not by as much as you would expect.

I have implemented a general system to handle gcc-style flags called `W_SPECIALIZATION` after gcc's `-W` argument. `W_SPECIALIZATION` is in its own separate header file, "w_specialization.h", which must be included for you to use. You must still include "parser.h". The syntax for `W_SPECIALIZATION` is intentionally similar to the syntax for a `Command_Line_Var`. Note that you are not in any way required to use this type of argument if you don't want to, and can use the simpler versions above.

`W_SPECIALIZATION` also works normally with other command flags and with other `W_SPECIALIZATIONS`.

Unlike a `Command_Line_Var`, `W_VALUE`s and `W_ARG`s prevent you from providing more than one subalias, though you could if you just made multiple `W_VALUE`s or `W_ARGS`.

#### W_SPECIALIZATION Example

```cpp
#include "parser.h"
#include "w_specialization.h"

int main(int argc, char ** argv){
    bool w_sign_conversion = true;
    bool w_all = false;
    bool w_extra = false;
    int w_error_level = 1;
    int w_warning_level = 1;
    char w_type = 'x';
    
    int d_debug_level = 0;
    bool d_out_of_bounds_checks = false;
    bool d_recursion_bounds = false;
    bool d_ingore_exceptions = true;
    
    char regular_c_string[100];
    unsigned int redundancy_of_this_README = (unsigned int)-1;
    
  {
    W_SPECIALIZATION w_options(100);
    
    W_VALUE<bool> w_sign_conversion_var(w_sign_conversion, w_options, "sign-conversion", true);
    W_VALUE<bool> w_no_sign_conversion_var(w_sign_conversion, w_options, "no-sign-conversion" , false);
    W_VALUE<bool> w_all_var(w_all, w_options, "all", true);
    W_VALUE<bool> w_extra_var(w_extra, w_options, "extra", true);
    
    W_ARG<int> w_error_level_var(w_error_level, w_options, "error-level");
    W_ARG<int> w_warning_level_var(w_warning_level, w_options, "warning-level");
    
    W_VALUE<char> w_type_a_var(w_type, w_options, "file", 'f');
    W_VALUE<char> w_type_b_var(w_type, w_options, "dir", 'd');
    W_VALUE<char> w_type_c_var(w_type, w_options, "link", 'l');
    W_VALUE<char> w_type_d_var(w_type, w_options, "any", 'a');
    
    Command_Line_Var<W_SPECIALIZATION> w_options_var(&w_options, { "W" }, true);
    
    // Also works with other W_SPECIALIZATIONS
    W_SPECIALIZATION d_options(100);
    
    W_ARG<int> d_debug_level_var(d_debug_level, d_options, "level");
    
    W_VALUE<bool> d_out_of_bounds_checks_var(d_out_of_bounds_checks, d_options, "out-of-bounds-checks", true);
    W_VALUE<bool> d_no_out_of_bounds_checks_var(d_out_of_bounds_checks, d_options, "no-out-of-bounds-checks", false);
    W_VALUE<bool> d_recursion_bounds_var(d_recursion_bounds, d_options, "recursion-bounds", true);
    W_VALUE<bool> d_no_recursion_bounds_var(d_recursion_bounds, d_options, "no-recursion-bounds", false);
    W_VALUE<bool> d_ignore_exceptions_var(d_ignore_exceptions, d_options, "ignore-exceptions", true);
    W_VALUE<bool> d_no_ignore_exceptions_var(d_ignore_exceptions, d_options, "no-ignore-exceptions", false);
    
    Command_Line_Var<W_SPECIALIZATION> d_options_var(d_options, { "D" }, true);
    
    // Also works with regular Command_Line_Vars
    Command_Line_Var<char> regular_c_string_var(regular_c_string, { "f", "file", "filename" }, true, 100);
    Command_Line_Var<unsigned int> redundancy_of_this_README_var(redundancy_of_this_README, { "r", "redundancy" }, true);
    
    non_options = ARGS_PARSER::parse(argc, argv);
  }
  
  // Other code. At this point, all variables are set.
}
```
This might look a little daunting, but bear in mind that we're linking somewhere around twenty flags to twelve variables while imposing a superstucture on the flags by using multiple `W_SPECIALIZATION`s. Four of these links come from setting `w_type` alone. It's also now to around three lines per variable, which isn't that much of an increase.

### Adding Your Own Extensions
You can implement more complex parsing by defining your own class or struct and overriding the template for a `Command_Line_Var` and writing your own version of `set_base_variable`. Below is the template specialization for `char` which allows it to act like a `char *`:

```cpp
// args_parser_templates.h

template<>
class Command_Line_Var<char> : public Command_Line_Var_Interface {
private:
        int buffer_size;
public:
        Command_Line_Var(void * b_v, std::vector<const char *> a, bool ta, int b_s);
        virtual void set_base_variable(const char * b_v);
};

// parser.h
#include "args_parser_templates.h"

// Other code

inline Command_Line_Var<char>::Command_Line_Var(void * b_v, std::vector<const char *> a, bool ta, int b_s) : Command_Line_Var_Interface(b_v, a, ta), buffer_size(b_s) {}

inilne void Command_Line_Var<char>::set_base_variable(const char * b_v) {
        char * base_variable_string = (char *)base_variable;
        int i = 0;
        while (b_v[i] != '\0' && i < buffer_size) {
                base_variable_string[i] = b_v[i];
                i++;
        }
        base_variable_string[i] = '\0';
}
```

Note that:
-   The template specialization for `char` extends `public Command_Line_Var_Interface`.
-   It includes a new variable `buffer_size`, which prevents it from going beyond its buffer. Because it has more variables than a `Command_Line_Var_Interface`, it needs to define its own constructor. Normally, if you don't have extra variables or don't need to do anything besides setting variables in the constructor, you don't need to define a constructor.
-   `set_base_variable` is a virtual function takes in a `const char *` and returns `void`. This function must be implemented to make the template specialization behave differently.

To specialize the template, you must include the header file `args_parser_templates.h`.

## Goals
1.  Add ability to create a vector of arguments provided to a flag.

2.  Make Windows specific compilation.

    1.  Either convert Makefiles to CMake or roll my own Project for Visual Studio.
    
3.  Add helpful error messages.

    1.  Currently, the program will convert strings into 0 if the argument takes a numeric argument.
        For example, `--prob=test` will set prob to 0.0, because prob is a double.
        
    2.  Other examples will come up whenever I encounter more errors.
    
4.  Verify that this code runs on Mac.

5.  See if I can't move `base_variable` from `Command_Line_Var_Interface` to the templated subclass of `Command_Line_Var`, which would really just reduce the typecast.

    1.  Not really a priority.
    
6.  Clean up the test program, specifically by moving all the comments to better locations.

7.  Run more tests, specifically trying to simulate command line response in standard Linux tools.

    1.  `wget` in particular looks perfect for this, with the notable exception of non-standard command-line arguments, such as -nc, which the library would treat as --nc.
    
    2.  It is not a good idea for me to try to implement all the flags for `gcc`, but it does have a more complex parsing algorithm I could try to simulate at least part of.

## Goals Completed
1.  Add autogenerated help.

2.  Add a help message for the test program.

    1.  Using the autogenerated help, no less.
    
3.  Add subcommands, which are things like `git commit`, where `git` is the main program and `commit` is a subcommand.

4.  Add example of template class specialization as specified in the section in test program.

5.  Single `char` arguments work. You could now type something like ```Command_Line_Var<char> single_char_var(single_char, { "A", "a", "B", "b" }, false)``` and it will work.

6.  Fix segmentation fault from having ignored flag in list of flags.

7.  Add way to allow user to automatically move flags to non-options by default.

    1.  This is most important when dealing with flags that need to be in order, like gcc's -l library flag.
    
8.  Convert library to a single file for the user to include.

    1.  Doing so would solve the issue of Windows specific compilation, as it would automatically be taken care of by the compiler.
    
    2.  Due to the nature of the algorithm, it has a one time use so it should only be included once, meaning the hit from making the functions inline shouldn't be any worse than just having the library.
    
    3.  It makes the algorithm easier for the user to include and use.
    
9.  Make sure that symlinks work on Windows.

10. Fix response to nonexistant flags. Right now, it just crashes the program with a seg fault. I can either make it ignore them or treat them as non-options.

11. Fix response to providing arguments to options that do not take arguments.

12. Fix the potential seg fault of writing outside the valid range for `char *`.

13. Add support for repeated single flags, such as `-vvvv` meaning level four verboseness.

14. Add helpful error messages for using invalid flags.

15. Add helpful error messages for providing arguments to flags that do not take arguments.

## License
This project is licensed under the MIT License - see the LICENSE.md file for details.
