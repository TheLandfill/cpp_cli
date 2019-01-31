# cpp_args_parser
This repository consists of a library designed to make parsing command line arguments for c++ easy and efficient and a simple program showing you how it works. While there exist similar libraries such as [GetOpt](https://www.gnu.org/software/libc/manual/html_node/Getopt.html), [commandline](https://archive.codeplex.com/?p=commandline), [CLI11](https://github.com/CLIUtils/CLI11), and [opt](http://www.decompile.com/not_invented_here/opt/), these libraries are either designed with different goals in mind, do far too much for most cases, use weird or complicated syntax, or just outright suck to use. This repository intends to make command line parsing as simple and easy as possible for the user and for the code itself to be easy to read and improve.

Currently, this library is in the beta stage. While it is stable, there are a few improvements that could be made. I'll list these improvements in the section "Goals."

While the source code itself is standard c++11, both Makefiles use the GNU/Linux tools make and g++ and the test program uses Linux symlinks, which may be invalid on Windows systems.

## Getting Started

As it currently stands, this repository will not work on Windows systems as it relies on GNU/Linux tools to compile.

### Prerequisites
The library requires nothing but c++11, but the test program itself needs the library to be compiled first. The test program already has symlinks to the required library and header file.

To use this library in other projects, it must first be compiled (it comes with its own Makefile which uses g++) and linked to the program that wants to use it, then you can include the header file "parser.h" and use all the necessary functionality.

### Install For Linux/Mac
#### Make the Library
1. Download the source code.
1. Move to the directory cpp_command_line_parser/bin/
1. Type "make".
#### Make the Test
1. Make the library first.
1. Move to the directory test_cpp_command_line_parser/bin/
1. Type "make".

### How to Use
1. Link the library in the project settings or in the Makefile
1. Include the header file "parser.h" in the main part of your program.
1. Create a new scope (this is just so the local variables you need to create disappear).
1. Inside that scope, create a `Command_Line_Var<T>` (syntax specified below).
1. Once you've created all the `Command_Line_Var`s, run the function `hash(argc, argv);`.
1. All the variables will be set after hash finishes.
1. Any non-flagged argument or subargument will be returned in a vector of "non_options" in the order in which they appear in the command line.

#### Syntax of Use
```
Command_Line_Var<T> variable_name_var(&variable_name, { "flag1", "flag2", "f" }, takes_args);
```
where `T` is the type of the variable and `variable_name_var` is a stack allocated variable. The first argument is the address of the variable you want to set, the second variable is an array/vector of strings corresponding to flags that control the value of the variable, and the third argument is a bool that determines whether or not the flags take arguments themselves.

### Parsing Rules
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
As it currently stands, this library can handle standard types that can be converted from a char \*, which include all numeric types, std::string, and char \*. To extend the library to handle other types, you need to either add a template specialization, which is what I have done for the numeric types, or overload the "=" operator to take in char \*, which is what std::string has done.

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
### Example Usage
```
#include "parser.h"

int main(int argc, char ** argv){
  std::string filename = "";
  int recursion_level = 0;
  std::string show_output = "";
  std::string standard_input = "";

  {
    // Both filename and recursion_level take args, so their third argument is true
    Command_Line_Var<std::string> filename_var(&filename, { "f", "filename", "file" }, true);
    Command_Line_Var<int> recursion_level_var(&recursion_level, { "r", "recursion", "max_depth" }, true);
    
    // show_output doesn't take arguments, so its third argument is false
    Command_Line_Var<std::string> show_output_var(&show_output, { "show_output", "s", "no_out", "half_out" }, false);
    
    // A single hyphen means standard input, but it doesn't have to. If you want, add the line
    Command_Line_Var<std::string> standard_input_var(&standard_input, { "-" }, false);
    hash(argc, argv);
  }
  
  // Other code. At this point, filename, recursion_level, and show_output are set.
}
```

## License
This project is licensed under the MIT License - see the LICENSE.md file for details

## Goals
1. Make Windows specific compilation.
    1. ~~Make sure that symlinks work on Windows.~~
    1. Either convert Makefiles to CMake or roll my own Project for Visual Studio.
1. Add helpful error messages.
    1. Currently, the program will just crash if you provide the wrong argument.
1. Verify that this code runs on Mac.
1. Add a help message for the test program.
1. Clean up the test program, specifically by moving all the comments to better locations.
1. Add autogenerated help.
1. Refine README
1. Run more tests, specifically trying to simulate command line response in standard Linux tools.
