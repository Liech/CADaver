![image](https://github.com/user-attachments/assets/baa67416-4072-4ff2-ab30-78724f8d7c56)


# CADaver - Now

CADaver is currently a STEP file viewer using Godot & Open Cascade.

# CADaver - Future

CADaver should be a basic & easy to use CAD tool, capable of replacing commerical CAD tools when its nothing too complex.

Due to godots accesibility it will be easy to improve UX/UI for ~everyone.

I get easily distracted by new projects, so lets see how it goes.

Goals for the next version:

* Extrusion
* Sketches (line only)
* Remember Settings
* Linux releasebuild


# Controls

| Move | Description |
|---|---|
| Left Click Drag | Rotate camera around center |
| Double Click | Change center to picked position |
| Right Click Draw | Panning |
| Mouse Wheel | Zoom |

# How to Build

It is writen Cross Platform compatible, so it should work on your machine.

You have to use VCPKG as your CMAKE toolchain to compile the GDExtension. The library should be put into the bin file of the project by a postbuildstep. Use godot-4.3-stable for the godotproject or change the branch in the root CMakelists.txt.
