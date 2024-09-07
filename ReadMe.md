# Ray Tracer
Ray tracers are a way to take scene and to generate a realistic rendering of that scene from a specific angle. This ray tracer project explores different modelling techniques such as L-System trees, fractal mountains, and constructive solid geometry amongst others.

The ray tracer uses lua as a scene descriptor. More information on the project as a whole can be found under this [report](https://drive.google.com/drive/u/0/folders/1Y_tYlbzSwEBm77zSKQuGIzMGtRGNHbfX).

## Building Steps
1. First install `premake` that can be found [here](https://premake.github.io/).
2. To ease the setup, important shared libraries are already included under the `share/` folder. To build these, run the premake binary (probably `premake5`):
```
premake5 gmake
```
followed by `make`.
3. Then go under `ray-tracer` to build the final ray tracer. Run
```
premake5 gmake
```
followed by `make`. The resulting binary `Ray-Tracer` is the ray tracer.
4. Render a specific lua file scene by looking at examples under `Assets`. More information on different lua commands and the scene descriptors can be found in the [report](https://drive.google.com/drive/u/0/folders/1Y_tYlbzSwEBm77zSKQuGIzMGtRGNHbfX).
