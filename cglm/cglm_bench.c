
#ifdef __APPLE__
#include <OpenGL/gl.h>
#elif _WIN32
#include <windows.h>
#include <GL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cglm.h>
#include <stdlib.h>

int
main(int argc, char** argv) {
    int iterate_count, i;
    CGLMmat4 view, proj, model, MVP;
    CGLMvec3 position, direction, up;

    if (argc < 2)
        return 1;

    iterate_count = atoi(argv[1]);

    model = cglmMat4(0.0f);
    proj = cglmPerspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    position = (CGLMvec3){3.0f, 0.0f, 10.0f};
    direction = (CGLMvec3){0.0f, 0.0f, 0.0f};
    up = (CGLMvec3){0.0f, 1.0f, 0.0f};

    for (i = 0; i < iterate_count; i++) {
        position.z -= 1;
        view = cglmLookAt(position, direction, up);
        MVP = cglmMultMat4(cglmMultMat4(proj, view), model);
    }

    return 0;
}
