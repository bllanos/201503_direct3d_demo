# A Demo of Elementary Hardware-Accelerated Graphics Techniques

**Using Direct3D 11 and Microsoft Visual Studio**

**Bernard Llanos**

**March 2015**

## What You Should See

A walkthrough of the demo (with screenshots) is provided in the repository's
[Wiki pages](https://github.com/bllanos/201503_direct3d_demo/wiki).

## About

### Project History and Sources

This demo is based on a final project for the Carleton University course
_COMP3501A: Foundations of Game Programming and Computer Graphics_ (Fall 2014 offering).

The team behind the COMP3501A project consisted of:
- Brandon Keyes
- Bernard Llanos
- Mark Wilkes

The main component of the projet, the Visual Studio 'engine' project (`engine/`),
was adapted from a final project for the Carleton University course
_COMP2501A: Computer Game Design and Development_ (Winter 2014 offering).

The team behind the COMP2501A project consisted of:
- Bernard Llanos
- Alec McGrail
- Benjamin Smith,

The COMP2501A project was based on the course tutorials, which in turn
were based on the [Rastertek tutorials](http://www.rastertek.com).

The rendering engine relies on framework code, the Visual Studio 'win32_base' project
(`win32_base/`), which was written by Bernard Llanos from May to September 2014 and is further
described in `win32_base/README.md`. Presently, it is also available at https://github.com/bllanos/win32_base

### Additional Sources

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

### Sources of Assets

- "Mars Rock 'Windjana' After Examination" (`textures/pia18092_mahli_sol627_windjana_500x537.dds`)
  - Downloaded from http://www.nasa.gov/jpl/msl/pia18092/#.VFOWALHBGnw
  - Image Credit: NASA/JPL-Caltech/MSSS
  - Retrieved October 31, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

- "Clouds and Sun" (`textures/IMG_3317_512x313.dds`)
  - Image Source: Bernard Llanos

- "Fifteen Years of NASA's Chandra X-ray Observatory" (`textures/0087_xray_512x595.dds`)
  - Downloaded from http://www.nasa.gov/sites/default/files/thumbnails/image/0087_xray.jpg
  - Image Credit: NASA/CXC/SAO
  - Retrieved November 17, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

- "MSH 11-62 Supernova Remnant" (`textures/msh1162_512x445.dds`)
  - Downloaded from http://www.nasa.gov/sites/default/files/msh1162.jpg
  - Image Credit: NASA/CXC/SAO/P. Slane et al.
  - Retrieved November 26, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

- "Orchid" (`textures/IMG_1931.dds`)
  - Image Source: Bernard Llanos

#### Assets used by test code only

- "King Fire in California, False-Color Infrared" (`textures/test/kingfire_oli_2014262_swir_lrg_512x512.dds`)
  - Downloaded from http://www.nasa.gov/sites/default/files/thumbnails/image/kingfire_oli_2014262_swir_lrg.jpg
  - Image Credit: NASA Earth Observatory image by Jesse Allen, using Landsat data from the U.S. Geological Survey
  - Retrieved October 19, 2014
  - Usage terms: Permissive (http://www.nasa.gov/audience/formedia/features/MP_Photo_Guidelines.html#.VEQBixaDbfc)

## How to Run a Clone of the Repository

### Including the DirectX Toolkit

Download the DirectX Toolkit from https://directxtk.codeplex.com/ and extract it
to the following location: `DirectXTK/` (create this folder in the root folder of the repository).

The 'engine' project (`engine/engine.vcxproj`) has been
set up to reference `DirectXTK/DirectXTK_Desktop_2013.vcxproj`.
If you are using Visual Studio 2012 instead of Visual Studio 2013,
you may need to change the reference to `DirectXTK/DirectXTK_Desktop_2012.vcxproj`.
(Similar steps will need to be followed for versions of Visual Studio other than 2012.)

### Building and Executing the Project

Open the Visual Studio solution file `game.sln` and examine the 'Solution Explorer' pane
to check that all projects have been loaded properly.

The solution should build and run with Visual Studio 2012 or higher on a system running
Windows 7 or higher. Development was undertaken primarily on Windows 7
with Visual Studio 2013 and DirectX 11-compatible graphics hardware.

To allow the program to run on a system with graphics hardware supporting up to DirectX 10, a software
driver for DirectX 11 may be used by changing "D3D_DRIVER_TYPE_HARDWARE" to "D3D_DRIVER_TYPE_WARP"
on Line 295 of `engine/cpp/d3dclass.cpp` (in the call to `D3D11CreateDeviceAndSwapChain()`).

For Visual Studio 2012, it will be necessary to 'downgrade' the projects
in the solution by changing their 'Platform Toolset' values (`Configuration Properties > General`).

Any broken links, "File not found"-type errors, (e.g. "Cannot open...") can normally be resolved
by removing the files in question from the project using the 'Solution Explorer' pane and then
using the `Project > Add Existing Item` wizard to reinsert them into the project under the
correct file paths.

#### Note concerning build configurations

The Visual Studio projects have different include directories defined for the "Debug" and "Release"
build configurations. The Debug configuration is active by default.
Currently, the Release configuration cannot be used to build the project because `#include` directives
for test code will not be resolved successfully. (I never had a need to run the project
outside of Visual Studio and therefore ignored this issue.)

### User Interaction

#### Quitting the program

Press `ESCAPE` or click the window's close button.

#### Free Camera Controls

| Camera Action | Corresponding User Input |
| :------------ |:---------------|
| Fine control over camera panning and tilting motion | CTRL + Move Mouse |
| Move left | Left Arrow Key |
| Move right | Right Arrow Key |
| Move forward | Up Arrow Key |
| Move backwards | Down Arrow Key |
| Pan left | CTRL + Left Arrow Key |
| Pan right | CTRL + Right Arrow Key |
| Tilt up | CTRL + Down Arrow Key |
| Tilt down | CTRL + Up Arrow Key |
| Move up | C + Up Arrow Key |
| Move down | C + Down Arrow Key |
| Roll left | R + Left Arrow Key |
| Roll right | R + Right Arrow Key |
| Zoom in | Z + Up Arrow Key |
| Zoom out | Z + Down Arrow Key |

#### Screen-Space Special Effects Selection

Press the `o` key to cycle between the motion blur effect (dark red background),
the shockwave effect (dark blue background) and no effect (black background).

The shockwave effect is centered around the current mouse position.
Move the mouse around inside the window to move the shockwave ring.

#### Configuration Files

While not an in-game control, all files in the `configFiles` folder can be modified to influence the
behaviour of the project when it is next run. The most useful customization for running a demo
of the project is window size, which can be adjusted on Lines 20 and 21 of
`configFiles/globalConfig.txt`.

## Development Tips

- In contrast, with the current Visual Studio project settings,
  when the program is executed, its working directory is the `win32_base` folder.
  (All configuration data must be written with this in mind.)
- All shaders are currently compiled for shader model 4.0 (as determined by the
  rendering configuration files).
- The Direct3D device is presently created
  with a feature level of 10.0, and set to use a hardware driver.
- Use Visual Studio's image editor to generate MIP maps for textures,
  as mip maps will not be generated at runtime.