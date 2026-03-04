# Sokol Triangle

A minimal OpenGL 3.3 triangle using the [Sokol](https://github.com/floooh/sokol) single-file headers.

## Files

- `test.cpp` — source code
- `vertex.glsl` — vertex shader
- `fragment.glsl` — fragment shader
- `sokol_app.h`, `sokol_gfx.h`, `sokol_glue.h`, `sokol_log.h` — required Sokol headers

## Compile

### Windows (MinGW g++)

```
g++ test.cpp -o triangle.exe -lgdi32 -lopengl32
```

Then run:

```
.\triangle.exe
```

### macOS (clang++)

```
clang++ -x objective-c++ -framework Cocoa -framework OpenGL test.cpp -o triangle
```

Then run:

```
./triangle
```

Note, if thi above command does not compile properly on MacOS, you can try a more verbose version:

```
clang++ -x objective-c++ -std=c++17 -framework Cocoa -framework OpenGL -framework QuartzCore test.cpp -o triangle
```
