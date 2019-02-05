# cpp_args_parser
This repository consists of a library designed to make parsing command line arguments for c++ easy and efficient and a simple program showing you how it works. While there exist similar libraries such as [GetOpt](https://www.gnu.org/software/libc/manual/html_node/Getopt.html), [commandline](https://archive.codeplex.com/?p=commandline), [CLI11](https://github.com/CLIUtils/CLI11), and [opt](http://www.decompile.com/not_invented_here/opt/), these libraries are either designed with different goals in mind, do far too much, or use weird or complicated syntax. This repository intends to make command line parsing as simple and easy as possible for the user and for the code itself to be easy to read and improve.

Currently, this library is in the beta stage. While it is stable, there are a few improvements that could be made. I'll list these improvements in the section "Goals."

While the source code itself is standard c++11, both Makefiles use the GNU/Linux tools make and g++.

## Table of Contents
1. [Why I Wrote This Library](#why-i-wrote-this-library)
1. [Getting Started](#getting-started)
    1. [Prerequisites](#prerequisites)
    1. [Install for Linux/Mac](#install-for-linux-or-mac)
        1. [Make the Library](#make-the-library)
        1. [Make the Test](#make-the-test)
1. [How to Use](#how-to-use)
    1. [Syntax of Use](#syntax-of-use)
1. [Parsing Rules](#parsing-rules)
    1. [Types the Library Can Handle](#types-the-library-can-handle)
        1. [How to Handle a `char` Array](#how-to-handle-a-char-array)
1. [Exception Throwing](#exception-throwing)
1. [Example Usage](#example-usage)
1. [Extensibility to More Complex Command Line Arguments](#extensibility-to-more-complex-command-line-arguments)
1. [Goals](#goals)
1. [Goals Completed](#goals-completed)
1. [License](#license)

## Why I Wrote This Library
- Every non-trivial program has to parse command line arguments, which leaves programmers often writing their own parsers for each individual project even though they are often writing the same inefficient, rigid, and unnecessarily complex algorithms.
- Current CLI parsers either do far too little, such as GetOpt, or far too much, such as CLI11. Programs should do one thing and do it well. This library takes the data in the command line and puts it into your variables.
- I wanted to contribute to the open source community and familiarize myself with GitHub.

## Getting Started

As it currently stands, this repository will not work on Windows systems as it relies on GNU/Linux tools to compile.

### Prerequisites
The library requires nothing but c++11, but the test program itself needs the library to be compiled first. The test program already has symlinks to the required library and header file.

To use this library in other projects, it must first be compiled (it comes with its own Makefile which uses g++) and linked to the program that wants to use it, then you can include the header file "parser.h" and use all the necessary functionality.

### Install For Linux or Mac
#### Make the Library
1. Download the source code.
1. Move to the directory cpp_command_line_parser/bin/
1. Type "make".
#### Make the Test
1. Make the library first.
1. Move to the directory test_cpp_command_line_parser/bin/
1. Type "make".

## How to Use
1. Link the library in the project settings or in the Makefile
1. Include the header file "parser.h" in the main part of your program.
1. Create a new scope (this is just so the local variables you need to create disappear).
1. Inside that scope, create a `Command_Line_Var<T>` (syntax specified below).
1. Once you've created all the `Command_Line_Var`s, run the function `hash(argc, argv);`.
1. All the variables will be set after hash finishes.
1. Any non-flagged argument or subargument will be returned in a vector of "non_options" in the order in which they appear in the command line.

### Syntax of Use
```
Command_Line_Var<T> variable_name_var(&variable_name, { "flag1", "flag2", "f" }, takes_args);
```
where `T` is the type of the variable and `variable_name_var` is a stack allocated variable. The first argument is the address of the variable you want to set, the second variable is an array/vector of strings corresponding to flags that control the value of the variable, and the third argument is a bool that determines whether or not the flags take arguments themselves.

## Parsing Rules
[This answer](https://stackoverflow.com/a/14738273/6629221) on stackexchange does a good job of summarizing the standard for command line argument syntax, and the library follows these rules, which are copied below for convenience.
> - Arguments are divided into options and non-options. Options start with a dash, non-options don't.
> - Options, as the name implies, are supposed to be optional. If your program requires some command-line arguments to do anything at all useful, those arguments should be non-options (i.e. they should not start with a dash).
> - Options can be further divided into short options, which are a single dash followed by a single letter (-r, -f), and long options, which are two dashes followed by one or more dash-separated words (--recursive, --frobnicate-the-gourds). Short options can be glommed together into one argument (-rf) as long as none of them takes arguments (see below).
>   - Options may themselves take arguments.
>   - The argument to a short option -x is either the remainder of the argv entry, or if there is no further text in that entry, the very next argv entry whether or not it starts with a dash.
> - The argument to a long option is set off with an equals sign: --output=outputfile.txt.
> - If at all possible, the relative ordering of distinct options (with their arguments) should have no observable effect.
> - The special option -- means "do not treat anything after this point on the command line as an option, even if it looks like one." This is so, for instance, you can remove a file named '-f' by typing rm -- -f.
> - The special option - means "read standard input".
> - There are a number of short option letters reserved by convention: the most important are
>   - -v = be verbose
>   - -q = be quiet
>   - -h = print some help text
>   - -o file = output to file
>   - -f = force (don't prompt for confirmation of dangerous actions, just do them)

Note that cpp_args_parser does not force you to use any of the commonly reserved short options at the bottom of the list, nor does it treat them any differently than any other options, nor does it reserve them. It is up to the user to maintain this standard. Furthermore, the special option "-" is treated just like any other option, so it is not reserved for standard input either. Finally, the special argument "--" will turn any arguments that come after it into non-options.

### Types the Library Can Handle
As it currently stands, this library can handle standard types that can be converted from a char \*, which include all numeric types, std::string, and char \* (char \* has a slightly different syntax which will be discussed below). To extend the library to handle other types, you need to either add a template specialization, which is what I have done for the numeric types, or overload the "=" operator to take in char \*, which is what std::string has done.

This library, however, does not natively support bool types, as it prefers to set a variable and check if it was set or what it was set to.

When using an argument that does not take subarguments, the variable will be set to whatever the subargument is. For instance, if the flag "-s" does not take arguments, it will set its corresponding variable to "s". Likewise, "--stop-early" will set its corresponding variable to "stop-early". If the argument can be stacked (such as -vvvv), then it will set its variable to "vvvv" for up to eleven stacked characters.

The template specialization itself takes the form
```
template<>
void Command_Line_Var<T>::set_base_variable(const char * b_v) {
    // Do stuff
}
```
while the default template version takes the form
```
virtual void set_base_variable(const char * b_v) {
    *(T *)base_variable = b_v;
}
```
#### How to Handle a `char` Array
Instead of creating a `Command_Line_Var<char *>` with the address of the variable containing the char \*, you should create a `Command_Line_Var<char>` with the char \* as its first argument. It also has a fourth argument which sets the buffer size, and so it cannot have a segmentation fault so long as you have allocated more than the buffer size you provide.

```
char example_string[20];
// INVALID: Does not accept char * as a type and the first argument should be a char *, not a char **.
Command_Line_Var<char *> example_string_var(&example_string, ..., ..., 20);
// INVALID: You have only allocated 20 bytes but say you can hold 200 in example_string.
Command_Line_Var<char> example_string_var(example_string, ..., ..., 200);
// VALID
Command_Line_Var<char> example_string_var(example_string, ..., ..., 20);
```

Furthermore, you should allocate enough memory to store the longest argument you expect to receive. If you do not, you run the risk of a segmentation fault, and there is nothing the library can do to fix it or notify you that your buffer is too small.

## Exception Throwing
The library will throw exceptions (std::invalid_argument) when you provide a flag you did not specify (except for the single hyphen flag for standard input) or provide an argument to a flag that does not take arguments. When the library throws an argument, it will tell you the error and which flag caused the error. The exception will only print around the first 32 characters of the flag. It will also throw an exception (std::length_error) if you try to use more flags than the hash table can handle, which is around 5,000.

## Example Usage
```
#include "parser.h"

int main(int argc, char ** argv){
  std::string filename = "";
  int recursion_level = 0;
  std::string show_output = "";
  std::string standard_input = "";
  char c_version_of_string[20];

  {
    // Both filename and recursion_level take args, so their third argument is true
    Command_Line_Var<std::string> filename_var(&filename, { "f", "filename", "file" }, true);
    Command_Line_Var<int> recursion_level_var(&recursion_level, { "r", "recursion", "max_depth" }, true);
    
    // show_output doesn't take arguments, so its third argument is false
    Command_Line_Var<std::string> show_output_var(&show_output, { "show_output", "s", "no_out", "half_out" }, false);
    
    // A single hyphen means standard input, but it doesn't have to. If you want, add the line
    Command_Line_Var<std::string> standard_input_var(&standard_input, { "-" }, false);
    
    // A char * has type char in the template and doesn't take the address of the variable.
    Command_Line_Var<char> c_version_of_string_var(c_version_of_string, { "v" }, false);
    hash(argc, argv);
  }
  
  // Other code. At this point, filename, recursion_level, and show_output are set.
}
```

## Extensibility to More Complex Command Line Arguments

If you look at gcc's documentation, you'll find what looks like 2000 different options, but there are really only less than twenty. However, each argument takes multiple subarguments. In the vanilla application of this library, `-Wall -Wno-sign-compare` would set the `std::string` or `char *` that the flag `-W` refers to to `no-sign-compare` with no mention of `-Wall`. You can fix this issue by defining your own class or struct and overriding the template for a `Command_Line_Var` and writing your own version of `set_base_variable`. Below is the template specialization for `char` which allows it to act like a `char *`:

```
// parser.h
template<>
class Command_Line_Var<char> : public Command_Line_Var_Interface {
private:
        int buffer_size;
public:
        Command_Line_Var(void * b_v, std::vector<const char *> a, bool ta, int b_s);
        virtual void set_base_variable(const char * b_v);
};

// parser.cpp
Command_Line_Var<char>::Command_Line_Var(void * b_v, std::vector<const char *> a, bool ta, int b_s) : Command_Line_Var_Interface(b_v, a, ta), buffer_size(b_s) {}

void Command_Line_Var<char>::set_base_variable(const char * b_v) {
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
- The template specialization for `char` extends `public Command_Line_Var_Interface`.
- It includes a new variable `buffer_size`, which prevents it from going beyond its buffer. Because it has more variables than a `Command_Line_Var_Interface`, it needs to define its own constructor. Normally, if you don't have extra variables or don't need to do anything besides setting variables in the constructor, you don't need to define a constructor.
- `set_base_variable` is a virtual function takes in a `const char *` and returns `void`. This function must be implemented to make the template specialization behave differently.

## Goals
1. Make Windows specific compilation.
    1. Either convert Makefiles to CMake or roll my own Project for Visual Studio.
1. Add helpful error messages.
    1. Currently, the program will convert strings into 0 if the argument takes a numeric argument.
       For example, `--prob=test` will set prob to 0.0, because prob is a double.
    1. Other examples will come up whenever I encounter more errors.
1. Add example of template class specialization as specified in the section  in test program.
1. Verify that this code runs on Mac.
1. Add a help message for the test program.
1. Add some sort of configuration file for the program to use in the case of a large number of arguments.
1. Clean up the test program, specifically by moving all the comments to better locations.
1. Refine README
1. Run more tests, specifically trying to simulate command line response in standard Linux tools.
    1. `wget` in particular looks perfect for this, with the notable exception of non-standard command-line arguments, such as -nc, which the library would treat as --nc.
    1. It is not a good idea for me to try to implement all the flags for `gcc`, but I made sure that cpp_args_parser can handle up to 5000 aliases, which is more than enough for gcc (which has around 2000) to handle.
    
## Goals Completed
1. Make sure that symlinks work on Windows.
1. Fix response to nonexistant flags. Right now, it just crashes the program with a seg fault. I can either make it ignore them or treat them as non-options.
1. Fix response to providing arguments to options that do not take arguments.
1. Fix the potential seg fault of writing outside the valid range for `char *`.
1. Add support for repeated single flags, such as `-vvvv` meaning level four verboseness.
1. Add helpful error messages for using invalid flags.
1. Add helpful error messages for providing arguments to flags that do not take arguments.

## License
This project is licensed under the MIT License - see the LICENSE.md file for details.
