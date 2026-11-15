![image](https://github.com/user-attachments/assets/baa67416-4072-4ff2-ab30-78724f8d7c56)

# CADaver - Now

CADaver is currently a STL/STEP/voxel file viewer with conversions between everything. The STL->CAD conversion is very unfinisheed.
It has a relativly solid foundation to support all features i can think of.

# CADaver - Future

CADaver should be a basic & easy to use Spatial Workbench:
 * capable of replacing commerical CAD tools for simple tasks (e.g. modify CAD without timeline or parameteric features).
 * flawless conversion in different spatial representations and data like Triangles, Brep, SDF, Voxel, skeletal animation.
 * Algorithms that leverage the strength of the corresponding spatial representation to achieve complex things easier
 * a kitchen sink approach of developing: since a lot of my temporal obsessions are spatial stuff i would like explore that obsession by adding features to this workbench
 * packing features into general purpose libraries that can be used by other projects.

# CADaver - Past

CADaver was built originally on Godot and OCCT. It was a bit tedious to cycle through the debug cycle and i currently find joy in collaborating with ai agents, so i changed it to vulkan + dearimgui + smlib. smlib allowed to ditch OCCT and use the MIT license. If you are interested in the old version check out v0.1.

# How to Build

It is writen Cross Platform compatible, so it should work on your machine.
Use VCPKG and CMAKE.
There are other public projects of mine this depends on, put them together in a project. Later id like to make it easier with my own vcpkg registry.
