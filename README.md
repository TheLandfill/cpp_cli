# cpp_cli
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/7f047f6cbe31451f87096d6a64e277fa)](https://www.codacy.com/app/TheLandfill/cpp_cli?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=TheLandfill/cpp_cli&amp;utm_campaign=Badge_Grade)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

cpp_cli is a library designed to make parsing command line arguments for c++ easy, needing only one line per variable you want to set and one line to parse, and efficient, running in linear time with respect to the number of arguments passed to the command line.

Currently, this library has a stable release with all the functionality you could want from a CLI library, including subcommand handling and automatic help generation.

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
	    
    2.  [`Value`s](#command_line_values)
    
    3.  [`Command Line Vector`s](#command_line_vectors)
    
    4.  [WSpecialization](#w_specialization)
    
        1.  [WSpecialization Example](#w_specialization-example)
	
    5.  [Adding Your Own Extensions](#adding-your-own-extensions)
    
9.  [Goals](#goals)

10. [Goals Completed](#goals-completed)

11. [License](#license)

## Why I Wrote This Library
-   Every non-trivial program has to parse command line arguments, which leaves programmers often writing their own parsers for each individual project even though they are often writing the same inefficient, rigid, and unnecessarily complex algorithms.

-   In my opinion, current CLI parsers either do far too little, such as GetOpt, or far too much, such as CLI11. Programs should do one thing and do it well. This library takes the data in the command line and puts it into your variables.

-   I wanted to contribute to the open source community and familiarize myself with GitHub.

-   After actually using some of the other competitors ([CLI11](https://github.com/CLIUtils/CLI11) has a good list) to test how my library stacks up to theirs, I'm starting to remember more why I made this library.

    -   For starters, I don't include half the STL to parse the command line, which keeps executables much smaller than other CLI libraries.

    -   This library is also way more flexible than most other libraries. It understands that its only job is to organize data on the command line so that it becomes much easier for you to read (and also document itself through the automatic help generation).

## Getting Started
As it currently stands, the entire product is valid c++11 except for the makefiles for compiling the projects.

### Prerequisites
The library requires nothing but c++11. The test program already has symlinks to the required library and header file.

### Install
No installation required. Just download the current release and include "cpp_cli.h" in your cpp source code wherever you want to parse the command line.

## How to Use
1.  Download the latest release.
2.  Include "cpp_cli.h" wherever you call any function of Parser.
3.  Create a new scope (this is just so the local variables you need to create disappear, but it's not necessary).
4.  Inside that scope, create your `Var<T>`s, create your `Value<T>`s, create your `Vector<T>`s, add your subcommands, and set help information (syntax specified below).
5.  If you want to generate a help message, run the function `Parser::generate_help(argv[0])`, which will create a text file with a help message.
6.  Once you've created all the `Var`s, run the function `Parser::parse(argc, argv);`.
7.  All the variables will be set after hash finishes.
8.  Any non-flagged argument or ignored argument will be returned in a vector of "non_options" in the order in which they appear in the command line.

### Syntax of Use
```cpp
using namespace cpp_cli;
Var<T> generic_syntax(T& variable_to_set, std::vector<const char *> aliases, bool takes_args, const char * help_mess = "")
Value<T> generic_syntax(T& variable_to_set, std::vector<const char *> aliases, T value_to_set_variable, const char * help_mess = "");
Vector<T> generic_syntax(std::vector<T>& vector_to_add_to, std::vector<const char * aliases>, const char * help_mess = "");

Parser::add_subcommand(const char * name_of_subcommand, subcommand, const char * help_mess = "");
// subcommand is a function of type "void function(int argc, char ** argv, void * data)"

Parser::set_usage("[options] non-option0 non-option1");
Parser::set_header("Here is a description of what your program does and so on.");
Parser::set_footer("For more information, contact us at the.landfill.coding@gmail.com.");
Parser::set_help_file_path("/usr/local/bin/folder_you_can_access/");

// Should be called after everything else is set and before Parser::parse
Parser::generate_help(argv[0]);

// data is an object/literal with data you want to pass to the subcommand.
Parser::parse(argc, argv, &data);
```
where `T` is the type of the variable and `variable_name_var` is a stack allocated variable. The first argument to a `Var` is either the variable you want to set, its address (as long as it isn't a `Var<char>`), or a `nullptr`, the second variable is an array/vector of strings corresponding to flags that control the value of the variable, the third argument is a bool that determines whether or not the flags take arguments themselves, and the last argument is an optional help message. The only difference between a `Value` and a `Var` is that the third argument is what you want the variable to be set to if the flag is found instead of a bool that determines whether the flag takes an argument, since `Value`s do not take arguments. `Vector`s take in a vector of type `T` and only take three arguments instead of four like a `Var` or a `Value`. If you don't provide one, the flags will still show up in the help message so that you don't forget about documenting a flag. To have a set of flags ignored, pass in anything starting with a backtick (\`) for the help message.

If a `nullptr` is provided for the first argument of a Var or Value, the parser will just treat it as if it were a non-option.

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
As it currently stands, this library can handle standard types that can be converted from a `char *`, which include all numeric types, std::string, and `char *`. To extend the library to handle other types, you need to either add a template specialization, which is what I have done for the numeric types, or overload the "=" operator to take in `char *`, which is what std::string has done.

When using a `Var` that does not take subarguments, the variable will be set to whatever the subargument is. For instance, if the flag "-s" does not take arguments, it will set its corresponding variable to "s". Likewise, "--stop-early" will set its corresponding variable to "stop-early" or however many characters allocated if the variable is a `char *`. If the argument can be stacked (such as -vvvv), then it will set its variable to "vvvv".

The default template version takes the form
```cpp
virtual void set_base_variable(const char * b_v) {
    *(T *)base_variable = b_v;
}
```

#### How to Handle a `char` Array
To create a `Var`/`Value` with a `char *` variable:

1.  Use a template type of `char`.

2.  Provide a fourth argument with the size of the buffer.

```cpp
const int example_string_bf_size = 20;
char example_string[example_string_bf_size];
Var<char> example_string_var(example_string, ..., ..., example_string_bf_size);
```

Furthermore, you should allocate enough memory to store the longest argument you expect to receive. If you do not, you run the risk of a segmentation fault, and there is nothing the library can do to fix it or notify you that your buffer is too small.

If you just want to set a singular `char`, passing it in as a normal variable works and you do not need to specify a buffer size.
```cpp
char example_char = 'x';
Var<char> example_char_var(example_char, ..., ...);
```

### Using Options Whose Locations Matter
Without going into too much detail, the `-l` flag in `gcc` and `g++` has to come after other arguments in gcc, but the library destroys the order of options in most cases. To keep a flag in the list of non-options, provide `nullptr` for the first argument in the Var constructor where the address of a variable would normally go. For example:
```cpp
Var<int> example_ignored_variable(nullptr, { "l", "library"}, true);
```
Any flag that starts with `-l` or `--library` will be considered as if it were a non-option. Note that the templated type was an int in this example, but the type doesn't matter.

Having a `-l` in a group of multiple short arguments such as `-albc` will throw an exception identifying the flag.

## Exception Throwing
The library will throw exceptions (`std::invalid_argument`) when you provide a flag on the command line that you did not specify (except for the single hyphen flag for standard input), provide an argument to a flag that does not take arguments, leave out an argument to a flag that does take arguments, or try to use a short option whose location on the command line matters inside a group of short options. When the library throws an argument, it will tell you the error and which flag caused the error.

The library will also throw an `std::runtime_error` if there is a problem with writing the help files.

## Example Usage
```cpp
#include "cpp_cli.h"

void sample_subcommand(int argc, char ** argv, void * data);

int main(int argc, char ** argv){
  std::string filename = "";
  int recursion_level = 0;
  std::string show_output = "";
  bool standard_input = false;
  char c_version_of_string[20];
  char file_type = '\0';
  std::vector<int> list_of_ints;
  std::vector<const char *> non_options;

  {
    using namespace cpp_cli;
    Var<std::string> filename_var(filename, { "o", "output", "out" }, true);
    Var<int> recursion_level_var(recursion_level, { "r", "recursion", "max_depth" }, true);
    Var<std::string> show_output_var(show_output, { "show_output", "s", "no_out", "half_out" }, false);
    Var<char> c_version_of_string_var(c_version_of_string, { "v" }, false, 20);
    
    Value<bool> standard_input_var(standard_input, { "-" }, true);
    Value<char> file_type_file_var(file_type, { "file", "f" }, 'f');
    Value<char> file_type_dir_var(file_type, { "d", "dir", "directory" }, 'd');
    Value<char> file_type_link_var(file_type, { "l", "link" }, 'l');
    
    Vector<int> list_of_ints_var(list_of_ints, { "list", "L" });
    
    Parser::add_subcommand(sample_subcommand, "test");

    // The third argument argument isn't necessary to set in this simple example.
    non_options = Parser::parse(argc, argv);
  }
  
  // Other code. At this point, all variables are set.
}
```
You can't get much simpler than two lines per variable (half of which are just initializing the variable anyway), but if you want to do more complicated things, you will have to get more complicated.

## Help Message
This library can automatically generate a help message by calling `Parser::generate_help(argv[0])`, which will generate a help message and store it in a file within the directory you specify named ".X_help_message", where "X" is the name of each subcommand leading up to and including the current subcommand. For instance, if git used this library the command "git push" would produce the file ".git_push_help_message" while just "git" would produce ".git_help_message".

To print out the current help message, use `Parser::print_help()`, which will print out the last help message of the last subcommand that called `generate_help(argv[0])`. It will only generate the help message if there is no help message file corresponding to the current subcommand, meaning you should delete all help message files on compiling. This library has no automatic trigger for a help message, so you'll still need to create a `Value<bool>` for each help message display. You may need to set the filename if you want a help message from a supercommand to be displayed. If `Parser::print_help()` is called without calling `Parser::generate_help(argv[0])`, the program will throw a runtime exception detailing which subcommand needs to have the `generate_help(argv[0])` added.

The help file path can either be relative or absolute, but you should only make it relative if you can guarantee that the executable will only be run from one unchanging directory. Otherwise, running it in multiple locations will produce a new help file in each of those new locations. `Parser::set_help_file_path("")` will set the file path to the current directory.

If the help file path you specify does not exist or you do not have permission to create a file in the directory, then the program will throw a runtime exception and notify you of the error. Either rerun the command with the proper privileges, make the directory, or use another directory.

## More Complex Command Line Parsing

### Subcommands

Subcommands are commands embedded inside a single command. The classic example of a command with subcommands is `git`, which has a huge list of subcommands that do completely different things but all use the same functionality of `git`. In this library, each subcommand is treated like a different main function that allows you to pass data from a supercommand into it. Other than the data passed to the subcommand (or data shared by a class if the supercommand and the subcommand have access to the same data), each subcommand is entirely different from the rest of the program, including its supercommand, its subcommands, and any other subcommand, meaning that each subcommand can also have different flags, the same flags, the same flags but referring to different things, etc. Subcommands are declared with the syntax `Parser::add_subcommand("Command Name", subcommand_function)`, where `void subcommand_function(int argc, char ** argv, void * data)` is the style of the subcommand function. It is important to note that each subcommand will treat itself as if it were its own program and completely ignore everything before it on the command line.

#### Subcommands Example

```cpp
#include "cpp_cli.h"

void push_subcommand(int argc, char ** argv, void * data);
void pull_subcommand(int argc, char ** argv, void * data);

struct Data_From_Principle_Subcommand {
	std::string file_path;
	int level;
};


int main(int argc, char ** argv) {
    using namespace cpp_cli;
	Data_For_Push_Subcommand dfps;

	Parser::add_subcommand("push", push_subcommand);
	Parser::add_subcommand("pull", pull_subcommand);
	// Other stuff like flags
	Var<std::string> file_path_var(dfps.file_path, { "p", "path" }, true);
	Vat<int> level_var(dfps.level, { "l", "level" }, true);
	
	// A void pointer to dfps will be passed to any subcommand used.
	Parser::parse(argc, argv, &dfps);
}

void push_subcommand(int argc, char ** argv, void * data_ptr) {
	Data_For_Push_Subcommand data = *((Data_For_Push_Subcommand *)data_ptr);
	// Do whatever
}
```

### `Value`s

A `Value` has the same syntax as the `Var`, except the third argument is what you want the value to be set to when the flag appears. For instance: `Value<char> some_var(some, { "some", "not-nothing", "s", "less-than-all" }, 's')` will set `some` to `'s'` if any of the flags in the list are found. These are better suited to options that do not take args than `Var`s.

### `Vector`s

A `Vector` has a similar syntax to the `Var`, except the third argument is removed entirely because you always need to provide an argument to the flags and it has no default value. It has the syntax:

```cpp
std::vector<T> list;

Vector<T> list_var(list, { "f", "flag" }, "Help String");
```

This will take a vector of type `T` as the first argument. Whenever one of its flags are found, it will use `Var<T>::set_base_variable(const char * b_v)` to convert the argument to the type and push it back to the end of the vector unless `T = const char *`, `T = char *`, or `T = char`. If `T = const char *`, then the vector will be filled with pointers to the command line arguments themselves, which should work fine since they're `const`. If `T = char *` on the other hand, the program will throw an exception if you try to run it because you can't set a `char *` to a `const char *` and because you can't fill a vector with bare `char *`s beforehand and tell the program what the buffer size is. Finally, if `T = char`, it will push back every `char` in `b_v` in order because you should be using strings and I'm going to make you feel bad for it.

### WSpecialization

This is where things get more complicated, though not by as much as you would expect.

I have implemented a general system to handle gcc-style flags called `WSpecialization` after gcc's `-W` argument. `WSpecialization` is in its own separate header file, "w_specialization.h", which must be included for you to use. You must still include "cpp_cli.h". The syntax for `WSpecialization` is intentionally similar to the syntax for a `Var`. Note that you are not in any way required to use this type of argument if you don't want to, and can use the simpler versions above.

`WSpecialization` also works normally with other command flags and with other `WSpecializationS`.

Unlike a `Var`, `Wvalue`s and `Warg`s prevent you from providing more than one subalias, though you could if you just made multiple `Wvalue`s or `WargS`.

#### WSpecialization Example

```cpp
#include "cpp_cli.h"

int main(int argc, char ** argv){
    bool w_sign_conversion = true;
    bool w_all = false;
    int w_error_level = 1;
    char w_type = 'x';
    
    int d_debug_level = 0;
    bool d_recursion_bounds = false;
    bool d_ingore_exceptions = true;
    
    char regular_c_string[100];
    unsigned int redundancy_of_this_README = (unsigned int)-1;
    
  {
    using namespace cpp_cli;
    WSpecialization w_options(100);
    
    Wvalue<bool> w_sign_conversion_var(w_sign_conversion, w_options, "sign-conversion", true);
    Wvalue<bool> w_no_sign_conversion_var(w_sign_conversion, w_options, "no-sign-conversion" , false);
    Wvalue<bool> w_all_var(w_all, w_options, "all", true);
    
    Warg<int> w_error_level_var(w_error_level, w_options, "error-level");
    
    Wvalue<char> w_type_a_var(w_type, w_options, "file", 'f');
    Wvalue<char> w_type_b_var(w_type, w_options, "dir", 'd');
    Wvalue<char> w_type_c_var(w_type, w_options, "link", 'l');
    Wvalue<char> w_type_d_var(w_type, w_options, "any", 'a');
    
    Var<WSpecialization> w_options_var(&w_options, { "W" }, true);
    
    WSpecialization d_options(100);
    
    Warg<int> d_debug_level_var(d_debug_level, d_options, "level");
    
    Wvalue<bool> d_recursion_bounds_var(d_recursion_bounds, d_options, "recursion-bounds", true);
    Wvalue<bool> d_no_recursion_bounds_var(d_recursion_bounds, d_options, "no-recursion-bounds", false);
    Wvalue<bool> d_ignore_exceptions_var(d_ignore_exceptions, d_options, "ignore-exceptions", true);
    Wvalue<bool> d_no_ignore_exceptions_var(d_ignore_exceptions, d_options, "no-ignore-exceptions", false);
    
    Var<WSpecialization> d_options_var(d_options, { "D" }, true);
    
    Var<char> regular_c_string_var(regular_c_string, { "f", "file", "filename" }, true, 100);
    Var<unsigned int> redundancy_of_this_README_var(redundancy_of_this_README, { "r", "redundancy" }, true);
    non_options = Parser::parse(argc, argv);
  }
  
  // Other code. At this point, all variables are set.
}
```
This might look a little daunting, but bear in mind that we're linking somewhere around twenty flags to twelve variables while imposing a superstucture on the flags by using multiple `WSpecialization`s. Four of these links come from setting `w_type` alone. It's also now to around three lines per variable, which isn't that much of an increase.

### Adding Your Own Extensions
You can implement more complex parsing by defining your own class or struct and overriding the template for a `Var` and writing your own version of `set_base_variable`. Below is the template specialization for `char` which allows it to act like a `char *`:

```cpp
// args_parser_templates.h

template<>
class Var<char> : public Var_Interface {
private:
        int buffer_size;
public:
        Var(void * b_v, std::vector<const char *> a, bool ta, int b_s);
        virtual void set_base_variable(const char * b_v);
};

// cpp_cli.h
#include "args_parser_templates.h"

// Other code

inline Var<char>::Var(void * b_v, std::vector<const char *> a, bool ta, int b_s) : Var_Interface(b_v, a, ta), buffer_size(b_s) {}

inilne void Var<char>::set_base_variable(const char * b_v) {
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
-   The template specialization for `char` extends `public Var_Interface`.
-   It includes a new variable `buffer_size`, which prevents it from going beyond its buffer. Because it has more variables than a `Var_Interface`, it needs to define its own constructor. Normally, if you don't have extra variables or don't need to do anything besides setting variables in the constructor, you don't need to define a constructor.
-   `set_base_variable` is a virtual function takes in a `const char *` and returns `void`. This function must be implemented to make the template specialization behave differently.

To specialize the template, you must include the header file `args_parser_templates.h`.

## Goals
1.  Make a separate compilable library as an option for people who don't want to inline or use subcommands in multiple source files.

2.  Refactor the code so that help file generation is more flexible and its own class so it doesn't clutter up cpp_cli.h.

3.  Make Windows specific compilation.

    1.  Either convert Makefiles to CMake or roll my own Project for Visual Studio.
    
4.  Add helpful error messages.

    1.  Currently, the program will convert strings into 0 if the argument takes a numeric argument.
        For example, `--prob=test` will set prob to 0.0, because prob is a double.
        
    2.  Other examples will come up whenever I encounter more errors.

5.  See if I can't move `base_variable` from `Var_Interface` to the templated subclass of `Var`, which would really just reduce the typecast.

    1.  Not really a priority.
    
6.  Run more tests, specifically trying to simulate command line response in standard Linux tools.

    1.  `wget` in particular looks perfect for this, with the notable exception of non-standard command-line arguments, such as -nc, which the library would treat as --nc.
    
    2.  It is not a good idea for me to try to implement all the flags for `gcc`, but it does have a more complex parsing algorithm I could try to simulate at least part of.

## Goals Completed
1.  Added ability to create a vector of arguments passed to a flag by creating `Vector`s.

2.  Add autogenerated help.

3.  Add a help message for the test programs.

    1.  Using the autogenerated help, no less.
    
4.  Add subcommands, which are things like `git commit`, where `git` is the main program and `commit` is a subcommand.

5.  Add example of template class specialization as specified in the section in test program.

6.  Single `char` arguments work. You could now type something like ```Var<char> single_char_var(single_char, { "A", "a", "B", "b" }, false)``` and it will work.

7.  Fix segmentation fault from having ignored flag in list of flags.

8.  Add way to allow user to automatically move flags to non-options by default.

    1.  This is most important when dealing with flags that need to be in order, like gcc's -l library flag.
    
9.  Convert library to a single file for the user to include.

    1.  Doing so would solve the issue of Windows specific compilation, as it would automatically be taken care of by the compiler.
    
    2.  Because of the nature of the algorithm, it has a one time use so it should only be included once, meaning the hit from making the functions inline shouldn't be any worse than just having the library. However, users might not want to recompile everything everytime, so I have to provide the functionality in its own header file.
    
    3.  It makes the algorithm easier for the user to include and use.
    
10. Make sure that symlinks work on Windows.

11. Fix response to nonexistant flags. Right now, it just crashes the program with a seg fault. I can either make it ignore them or treat them as non-options.

12. Fix response to providing arguments to options that do not take arguments.

13. Fix the potential seg fault of writing outside the valid range for `char *`.

14. Add support for repeated single flags, such as `-vvvv` meaning level four verboseness.

15. Add helpful error messages for using invalid flags.

16. Add helpful error messages for providing arguments to flags that do not take arguments.

## License
This project is licensed under the MIT License - see the LICENSE.md file for details.
