# Source Friendly Library

This library is Source Friendly, which means the following:

## Headers and Sources are ready to use as distributed, without a configuration step.

The headers and sources are ready to use and include in a project as they are distributed.  The requirement of a configuration step such as "CMAKE", "Boost Build", or any other "code massaging" or generation step is forbidden within the library and any external dependencies.

## Configuration is done through pre-processor defines and logic.

All configuration is done through pre-processor defines and pre-processor logic within the source code itself.  If you want to bake in a configruation, the source will look for a "Configuration.h" file, using quoted form to prefer one within the distribution, and will include it only if it is there.  This allows a configuration to be part of a distribution or branch used within an organization or project, while still allowing updates to the library source code itself to cleanly merge.

## Seperation of headers and implementation.

Header files (.h, .hpp, etc) are in seperate directories in the source distribution from source files (.c, .cpp, etc).  This makes it easy to isolate and distribute headers for use with pre-compiled builds of the library.  Note that this is not meant to guide descions on what should be implemented in headers and what should be imlemeneted in source, just to make the two easily seperable.

## Scoping of functionality to prevent conflicts with other libraries

A namespace is used to scope functionality of the library, prefixes are used to scope pre-processor directives, and a folder name within the include directory is used to scope headers.  This prevents conflicts wiih the use of multiple libraries within client source code.  Some examples:

    #define FOO_USE_SINGLE_THREADED
    #define BAR_USE_SINGLE_THREADED

    #include <Foo/Utilities.h>
    #include <Bar/Utilities.h>

    Foo::initialize();
    Bar::initialize();

## Supporting IDE Projects, makefiles, Examples, etc are seperate from library source code.

CMAKE scripts, IDE projects, examples, sources for utility applications, etc are outside and seperate from the headers and the source code of the library, and in their own folders in a distribution.  For example, making it even easier for someone to use CMAKE to generate a project for their chosen IDE or for installation is good, but mixing the CMakeLists.txt files in with the source and header files is not.  It contaminates the source and headers with toolchain specific files, and encourages the kind of pre-build Hell the Source Friendly guidelines are specifically desinged to avoid.  Having a VisualStudio directory with projects that can be directly imported into a Visual Studio solution can be good.  (Especially since Visual Studio makes it hard to just add a source and header directory to the project.)  A mess of toolchain specific files in the root directory of a source distribution is not.
