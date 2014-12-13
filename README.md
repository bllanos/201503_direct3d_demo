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

Presently, the 'engine' project (`engine\engine.vcxproj`) has been
set up to reference `DirectXTK\DirectXTK_Desktop_2013.vcxproj`.
If you are using Visual Studio 2012 instead of Visual Studio 2013,
you may need to change the reference to `DirectXTK\DirectXTK_Desktop_2012.vcxproj`.

### Running a clone of the repository
_(Hopefully not applicable to any submitted versions of the code)_

In the event that the code is a direct clone of the repository,
you will need to obtain the DirectX Toolkit (see above) in order to build
the project.

Afterwards, the project should build properly,
but will not execute normally. The reason for the abnormal execution is the lack
of several folders which are not part of the repository, but which the program
expects to exist (depending on the configuration data passed to it).

To allow for normal execution, create the following folders:
- Empty folders
  - `logFiles`
  - `logFiles\test`
  - `configFiles\output_gitIgnored`
- Non-empty folders
  - `textures`: Insert textures for the project here.
  - `textures\test`: Insert textures needed by test code here.

Note that the above folder paths are relative to the solution directory
(the directory containing `game.sln`).

In contrast, with the current Visual Studio project settings,
when the program is executed, its working directory is the `win32_base` folder.
(All configuration data must be written with this in mind.)

## Code References

(Beyond COMP3501A course demos and assignment solutions)

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

(Beyond COMP3501A course demos and assignment solutions)

- Phong lighting
  - COMP3501A illumination demo
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011. (Chapter 11)
- Vertex skinning
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011. (Chapter 8)
- Texture setup
  - Chuck Walbourn's "DirectX SDK's Direct3D 11 tutorial series updated
    to use Visual Studio 2012 and the Windows SDK 8.0 without any dependencies
	on legacy DirectX SDK content" (_Direct3D Tutorial Win32 Sample_),
	available at https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef
- Particle systems
  - Zink, Jason, Matt Pettineo and Jack Hoxley.
    _Practical Rendering and Computation with Direct 3D 11._
    Boca Raton: CRC Press Taylor & Francis Group, 2011. (Chapter 12)

## Sources of Assets

- "Mars Rock 'Windjana' After Examination" (`textures\pia18092_mahli-sol627-windjana.jpg`)
  - Downloaded from http://www.nasa.gov/jpl/msl/pia18092/#.VFOWALHBGnw
  - Image Credit: NASA/JPL-Caltech/MSSS
  - Retrieved October 31, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

- "Clouds and Sun" (`textures\IMG_3317.JPG`)
  - Image Source: Bernard Llanos

- "Fifteen Years of NASA's Chandra X-ray Observatory" (`textures\0087_xray.jpg`)
  - Downloaded from http://www.nasa.gov/sites/default/files/thumbnails/image/0087_xray.jpg
  - Image Credit: NASA/CXC/SAO
  - Retrieved November 17, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

- "MSH 11-62 Supernova Remnant" (`textures\msh1162.jpg`)
  - Downloaded from http://www.nasa.gov/sites/default/files/msh1162.jpg
  - Image Credit: NASA/CXC/SAO/P. Slane et al.
  - Retrieved November 26, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

- "Orchid" (`textures\IMG_1931.JPG`)
  - Image Source: Bernard Llanos

### Textures used by test code only

- "King Fire in California, False-Color Infrared" (`textures\test\kingfire_oli_2014262_swir_lrg.jpg`)
  - Downloaded from http://www.nasa.gov/sites/default/files/thumbnails/image/kingfire_oli_2014262_swir_lrg.jpg
  - Image Credit: NASA Earth Observatory image by Jesse Allen, using Landsat data from the U.S. Geological Survey
  - Retrieved October 19, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

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

- Use Visual Studio's image editor to generate MIP maps for textures,
  as mip maps will not be generated at runtime.

## Camera Controls
Toggle Camera Mode:
- First/third/free Camera: P

- Read below for first/third/free camera controls.
  
## Free Camera Controls
Move Camera with Mouse:
- Pan/Tilt: CTRL + Move Mouse
    
Move Camera:
- Left:     SHIFT + LEFT ARROW
- Right:    SHIFT + Right ARROW
- Forward:  SHIFT + UP ARROW
- Backward: SHIFT + DOWN ARROW
    
Pan Camera:
- Left:  CTRL + LEFT ARROW
- Right: CTRL + RIGHT ARROW
    
Tilt Camera:
- Up:   CTRL + DOWN ARROW
- Down: CTRL + UP ARROW
    
Crane Camera:

- Up:   C + UP ARROW
- Down: C + DOWN ARROW
    
Roll Camera:
- Left:  R + LEFT ARROW
- Right: R + RIGHT ARROW
    
Zoom Camera:
- In:  Z + UP ARROW
- Out: Z + DOWN ARROW

## Player Controls (First Person / Third Person Camera)
Spin Player with Mouse:
- Pan/Tilt: There is a deadzone in the center of the screen where the mouse will not affect
			the camera orientation. Move away from this deadzone and the orientation will
			accelerate toward that direction.
			
Horizontal Movement:
- Left:  A
- Right: D

Vertical Movement:
- Up:   SHIFT
- Down: CTRL

Z Movement:
- Forward:  W
- Backward: S

## Debugging Controls

CTRL + O: When in the game state with SSSEs, switch between the shockwave SSSE
          and the motion blur SSSE.
