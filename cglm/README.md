CGLM
====
[![Build Status](https://travis-ci.org/funlibs/cglm.svg?branch=master)](https://travis-ci.org/funlibs/cglm)
[![Build status](https://ci.appveyor.com/api/projects/status/j7h4kdur2cxami3f/branch/master?svg=true)](https://ci.appveyor.com/project/ssbx/cglm-f7i2h/branch/master)

Basic linear math library written in C inspired by the C++ GLM library (http://glm.g-truc.net).

Use
---
Include cglm.h after GL/gl.h or the library you are using to create your OpenGL context.

Build
-----
CGLM test suite is build with CMake for all platforms.

You can also use the fake configure script provided to set it up on unix:
```sh
./configure
make
```

Doc
---
Depends on Doxygen:
```sh
make cglm_doc
```
