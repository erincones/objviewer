# OBJViewer

Another OBJ/MTL 3D models viewer for GCC.

An example for MSVC is available in <https://github.com/Rebaya17/objviewer>


## Features

- [ ] OBJ/MTL 3D models loading
  - [ ] Intercative position, scale and rotation
  - [ ] Interactive material attributes
- [x] Shaders loading in runtime
- [x] 2D and cube maps textures loading in runtime
- [ ] Interactive camera
  - [ ] Mouse and keyboard control
  - [ ] Orthogonal and perspective projection
  - [ ] Variable speed
- [ ] Lighting techniques
  - [ ] Blinn-Phong
  - [ ] Oren-Nayar
  - [ ] Cook-Torrance
- [ ] Type of lights
  - [ ] Directional
  - [ ] Point
  - [ ] Spotlight
- [ ] Shading techniques
  - [ ] Normal mapping
  - [ ] Occlusion parallax mapping
  - [ ] Shadow mapping
  - [ ] Deferred shading


# Dependencies

- [GLFW]\: Library for OpenGL
- [Glad]\: OpenGL extension loader library
- [GLM]\: OpenGL mathematics
- [stb_image.h]\: Image loader
- [Dear ImGui]\: Immediate mode graphical user interface
- [ImGuizmo]\: Immediate mode 3D controls


[//]: # "Links references"

[GLFW]: https://www.glfw.org/
[Glad]: https://github.com/Dav1dde/glad
[GLM]: http://glm.g-truc.net/
[stb_image.h]: https://github.com/nothings/stb
[Dear ImGui]: https://github.com/ocornut/imgui
[ImGuizmo]: https://github.com/CedricGuillemet/ImGuizmo