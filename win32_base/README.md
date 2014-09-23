# A Small win32 'Engine'

**Created for:** Spring 2014 Direct3D 11 Learning

**By:** Bernard Llanos

**Project started:** May 18, 2014

## Description
This program is intended as a platform on which DirectX experiments
can be built. It is designed to provide a number of useful services
(once fully implemented):
- Loading parameters from text files at runtime, as well as saving parameters
to files
- Management of simple windows on which to display rendered images
- Handling Windows messages, including for keyboard and mouse input
- Logging debugging messages to files and/or a console window

## Operating Instructions
Once the program is built and run for the first time, if its
configuration file has not instructed it otherwise, it will
create a text file, `.\logger_output\log.txt`, before exiting.

This output file provides guidelines on
how to configure the program, as it records the parameters
that the program can accept. (_Note: If you modify the program, please
consider outputting the absence from the input configuration files
of your new/different configuration parameters to the log files.
Also, be careful not to rely
on the presence of any configuration data._)

The program will attempt to interpret its command-line string as the location
of a configuration file. If no command-line arguments are passed,
it will try to use
`.\configuration\config.txt` as a configuration file.

## References
The following works were consulted for ideas during development
- Horton, Ivor. _Beginning Visual C++ 2008._
  Indianapolis: Wiley Publishing Inc., 2008.
- Luna, Frank D. _3D Game Programming with DirectX 11._
  Dulles: Mercury Learning and Information, 2012.
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.
- My game development group project for the course COMP2501A
  (Offered Winter 2014 at Carleton University)
  - This was, in turn, based on the course tutorials,
    which were based on the Rastertek tutorials
	(http://www.rastertek.com, which seems to no longer be available
	as discovered on August 3, 2014)

I did not copy large portions of code directly from any of the above references.
However, aspects of this project are essentially amalgamations
of code listings from these references.

## Development Environment
Visual Studio 2013 running on Windows 7, 64-bit

Note that the "Character Set" project property
(Configuration Properties > General) should be set to Unicode
for all configurations, when using Visual Studio.

## Additional Linker Dependencies
In addition to the default dependencies added by Visual Studio 2013,
I have added the following libraries as dependencies

| Filename  | Source  | Usage |
|:------|:--------|:------|
| `Shlwapi.lib` | Windows platform | File and directory existence and attribute checks, and filepath manipulation |

## Dependency on Microsoft Windows
This project makes use of the Standard Template Library
to allow for platform-independent code in most places.
The major areas in which the code is dependent on the Windows environment
are as follows:
- Error reporting makes use of the `HRESULT` data type,
  and occasionally some error codes (the 'code' portion of an `HRESULT` value)
  that are defined within Windows header files.
  - Note that it would be trivial to define macros to recreate this
    functionality on other platforms.
  - See [_Using Macros for Error Handling_ at MSDN]
    (http://msdn.microsoft.com/en-us/library/windows/desktop/ms691242%28v=vs.85%29.aspx)
	for more information.
- All GUI-related code is Windows-specific.
- Exceptions of type `std::exception` are created using a
  Microsoft-specific constructor that accepts a string argument.
- The preprocessor directive `#pragma once`
  (used to prevent multiple inclusions of header files) is Microsoft-specific.
- See above for the usage of libraries provided by Microsoft Windows.

## License and Usage
Most of the code in this project was developed with the intention
of reinventing the wheel for practice.
While I have tried to make the code robust, my aim was only to capture
the essential aspects of this type of system, noting that others
have already made far better works that I might rely on instead in the future.

I suggest that you use other, more fully-developed projects (by other authors)
that have been created to provide a superset of the functionality
of this program. For instance, there is the Hieroglyph 3 engine,
available at http://hieroglyph3.codeplex.com/ under a very flexible license.
Nevertheless, if you have comments on this project or wish to use
large portions of the code, I would appreciate if you sent me an email
at llanbe_t3mp@yahoo.ca (primarily because I value any opportunity
to learn from you in the process).