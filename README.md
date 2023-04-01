# LodGenerator Library
LodGenerator is a C++ library that provides functionality to create level of details (LOD) for meshes. The library allows for the generation of simplified versions of complex meshes that can be used for rendering at different distances, resulting in significant performance improvements.
# Installation

To use LodGenerator, you can include the library in your C++ project as a static. To build the library, you will need a C++ compiler that supports C++11 or later.
# Usage

To use the LodGenerator library, you will need to follow these steps:

* Include the "LodGenerator.h" header file in your C++ project.
* Create an instance of the LodGenerator class.
* Call the generateLod() method with the original mesh data and desired LOD levels to generate simplified meshes.
* Use the generated simplified meshes for rendering at different distances.

```cpp
#include "LodGenerator.h"
// Your code here
mesh src_mesh;
mesh dst_mesh;
// Code for reading source file vertexes and indices
auto instance = lod_generator::lod_core::get_instance();
instance->generate_lod(src_mesh, dst_mesh, lod_generator::HYBRID_QEM);
```

# How to use LodTerminal
LodTerminal is an example of a program using the LodGenerator library. Currently it only works with the obj format.
Commands: 
+ help - display all commands
+ optimize - requests a geometry file for optimization and a file to save to
+ tests - calls tests (for example, a calculation test on a video card)