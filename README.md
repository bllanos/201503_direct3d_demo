# COMP3501A Game Project

**Fall 2014 (September - December)**

### Authors
- Brandon Keyes, ID: 100897196
- Bernard Llanos, ID: 100793648
- Mark Wilkes, ID: 100884169

## Description
An outer space-themed game. (Refer to project proposal for more details.)

## Building and Operating Instructions
The program should build and run with Visual Studio 2012 or higher,
on Windows 7 or higher. Development was undertaken primarily on Windows 7
with Visual Studio 2013.

For Visual Studio 2012, it will be necessary to 'downgrade' the projects
in the solution by changing their 'Platform Toolset' values (`Configuration Properties > General`).

Any broken links, “File not found”-type errors, (e.g. “Cannot open…”) can normally be resolved
by removing the files in question from the project using the 'Solution Explorer' pane and then
using the `Project > Add Existing Item` wizard to reinsert them into the project under the
correct file paths.

One group member uses the WARP software driver due to hardware limitations.
This setting should not be pushed to the repository, but in the event
that it makes its way in, it can be changed on Line 292 of d3dclass.cpp
(in the call to `D3D11CreateDeviceAndSwapChain()`).

### Including the DirectX Toolkit

The DirectX Toolkit may not be included with the project code
(and is certainly not in the repository). If it is absent,
download it from https://directxtk.codeplex.com/ and extract it
to the following location: `DirectXTK`.

Next, build the appropriate version of the toolkit for your
platform by opening and building the appropriate Visual Studio
solution file in the `DirectXTK` folder. Specifically,
`DirectXTK_Desktop_2013.sln` and `DirectXTK_Desktop_2012.sln`
are the usable files (for Visual Studio 2013 or 2012, respectively).

The toolkit needs to be built before the rest of the project
can be built.

### Running a clone of the repository
_(Hopefully not applicable to any submitted versions of the code)_

In the event that the code is a direct clone of the repository, it should build properly,
but will not execute normally. The reason for the abnormal execution is the lack
of several folders which are not part of the repository, but which the program
expects to exist (depending on the configuration data passed to it).

To allow for normal execution, create the following (empty) folders:
- `logFiles`
- `logFiles\test`
- `configFiles\output_gitIgnored`

Note that the above folder paths are relative to the solution directory
(the directory containing `game.sln`).

In contrast, with the current Visual Studio project settings,
when the program is executed, its working directory is the `win32_base` folder.
(All configuration data must be written with this in mind.)

## Code References
The main project code, the Visual Studio 'engine' project,
was adapted from the following sources:
- Bernard Llanos, Alec McGrail, Benjamin Smith,
  group project for the course COMP2501A (Winter 2014)
  - This was, in turn, based on the course tutorials,
    which were based on the Rastertek tutorials.
	(http://www.rastertek.com, currently unavailable)

The game engine relies on framework code, the Visual Studio 'win32_base' project.
This code was written by Bernard Llanos from May to September 2014 and is further
described in `win32_base\README.md`.
Presently, it is also available at https://github.com/bllanos/win32_base

The following works were consulted for ideas during development:
- Luna, Frank D. _3D Game Programming with DirectX 11._
  Dulles: Mercury Learning and Information, 2012.
- Zink, Jason, Matt Pettineo and Jack Hoxley.
  _Practical Rendering and Computation with Direct 3D 11._
  Boca Raton: CRC Press Taylor & Francis Group, 2011.

### References for Specific Features
- Phong lighting
  - COMP3501A illumination demo
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011. (Chapter 11)

## Sources of Assets
There are no assets, currently.

## Development Policies
- Please add each new file to the system under a path that is relative to the
  containing Visual Studio project's folder. File paths used by Visual Studio
  can be adjusted manually by modifying `*.vcxproj` and `*.vcxproj.filters`
  with a text editor.

- Please push work frequently to the repository so that others can see
  progress made between project milestones. However, do not
  alter the 'master' branch except to merge in code from working
  branches that has been tested at least superficially.
  (i.e. Keep features not yet fully developed on branches
  other than 'master'.) The 'master' branch should ideally be demo-worthy at
  all times.

- Before merging code into the 'master' branch, merge the 'master' branch
  into your working branch and ensure that the merged code works properly.

- All shaders will be compiled for shader model 4.0 (as determined by the
  rendering configuration files). The Direct3D device will be created
  with a feature level of 10.0, and will be set to use a hardware driver.
