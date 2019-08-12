# OBJViewer

Another OBJ/MTL 3D models viewer.

Checkout the [msvc] branch for the Visual Studio 2019 project.


## Features

- [x] OBJ/MTL 3D models loading
  - [x] Interactive position, scale and rotation
  - [x] Interactive material attributes
  - [x] Reload materials in runtime
- [x] Shaders loading in runtime
- [x] 2D and cube maps textures loading in runtime
- [x] Interactive camera
  - [x] Mouse and keyboard control
  - [x] Orthogonal and perspective projection
  - [x] Variable speed
- [x] Lighting techniques
  - [x] Blinn-Phong
  - [x] Oren-Nayar
  - [x] Cook-Torrance
- [x] Arbitrary number of lights
- [x] Type of lights
  - [x] Directional
  - [x] Point
  - [x] Spotlight
- [ ] Shading techniques
  - [x] Normal mapping
  - [x] Occlusion parallax mapping
  - [ ] Shadow mapping
  - [x] Deferred shading


# Dependencies

- [GLFW]\: Library for OpenGL
- [Glad]\: OpenGL extension loader library
- [GLM]\: OpenGL mathematics
- [stb_image.h]\: Image loader
- [Dear ImGui]\: Immediate mode graphical user interface
- [ImGuizmo]\: Immediate mode 3D controls


[//]: # "Links references"

[msvc]: https://github.com/Rebaya17/objviewer/tree/msvc/

[GLFW]: https://www.glfw.org/
[Glad]: https://github.com/Dav1dde/glad
[GLM]: http://glm.g-truc.net/
[stb_image.h]: https://github.com/nothings/stb
[Dear ImGui]: https://github.com/ocornut/imgui
[ImGuizmo]: https://github.com/CedricGuillemet/ImGuizmo