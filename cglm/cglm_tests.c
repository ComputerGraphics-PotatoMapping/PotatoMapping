/*
 * MIT License
 *
 * CGLM Copyright (c) 2016 Sebastien Serre <ssbx@sysmo.io>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#elif _WIN32
#include <windows.h>
#include <GL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cglm.h>
#include <cglm_debug.h>


/*
 * vesult, mresult and fresult values have been computed the C++ glm library.
 */

// Minunit include BEGIN
/* Copyright (C) 2002 John Brewer */
#define mu_assert(message, test) do { \
    if (!(test)) return message; \
} while (0)
#define mu_run_test(test) do { \
    char *message = test(); \
    tests_run++; \
    if (message) return message; \
} while (0)
int tests_run = 0;
// Minunit include END


/*
 * TODO get results from GLM and compare to FLT_EPSILON
 */
static const float LARGE_ROUND = FLT_EPSILON * 1000;

CGLMvec3 vcompare;
CGLMvec3 vresult;

static char*
compare_vec3() {
    printf("\nExpected vec3: \n");
    debugvec3(vresult);
    printf("Compared vec3: \n");
    debugvec3(vcompare);

    mu_assert("x not match", fabs(vcompare.x - vresult.x) < LARGE_ROUND);
    mu_assert("y not match", fabs(vcompare.y - vresult.y) < LARGE_ROUND);
    mu_assert("z not match", fabs(vcompare.z - vresult.z) < LARGE_ROUND);
    return NULL;
}

CGLMmat4 mcompare;
CGLMmat4 mresult;

static char*
compare_matrix() {
    printf("\nExpected mat4: \n");
    debugmat4(mresult);
    printf("Compared mat4: \n");
    debugmat4(mcompare);

    mu_assert("compare error on a0",
            fabs(mcompare.a0 - mresult.a0) < LARGE_ROUND);
    mu_assert("compare error on a1",
            fabs(mcompare.a1 - mresult.a1) < LARGE_ROUND);
    mu_assert("compare error on a2",
            fabs(mcompare.a2 - mresult.a2) < LARGE_ROUND);
    mu_assert("compare error on a3",
            fabs(mcompare.a3 - mresult.a3) < LARGE_ROUND);
    mu_assert("compare error on b0",
            fabs(mcompare.b0 - mresult.b0) < LARGE_ROUND);
    mu_assert("compare error on b1",
            fabs(mcompare.b1 - mresult.b1) < LARGE_ROUND);
    mu_assert("compare error on b2",
            fabs(mcompare.b2 - mresult.b2) < LARGE_ROUND);
    mu_assert("compare error on b3",
            fabs(mcompare.b3 - mresult.b3) < LARGE_ROUND);
    mu_assert("compare error on c0",
            fabs(mcompare.c0 - mresult.c0) < LARGE_ROUND);
    mu_assert("compare error on c1",
            fabs(mcompare.c1 - mresult.c1) < LARGE_ROUND);
    mu_assert("compare error on c2",
            fabs(mcompare.c2 - mresult.c2) < LARGE_ROUND);
    mu_assert("compare error on c3",
            fabs(mcompare.c3 - mresult.c3) < LARGE_ROUND);
    mu_assert("compare error on d0",
            fabs(mcompare.d0 - mresult.d0) < LARGE_ROUND);
    mu_assert("compare error on d1",
            fabs(mcompare.d1 - mresult.d1) < LARGE_ROUND);
    mu_assert("compare error on d2",
            fabs(mcompare.d2 - mresult.d2) < LARGE_ROUND);
    mu_assert("compare error on d3",
            fabs(mcompare.d3 - mresult.d3) < LARGE_ROUND);
    return NULL;
}

static char*
test_perspective() {
    mresult = (CGLMmat4){
        1.344400f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 1.792600f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f, -1.002000f, -1.000000f,
        0.000000f, 0.000000f, -0.200200f, 0.000000f
    };
    mcompare = cglmPerspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    mu_run_test(compare_matrix);

    return NULL;
}

static char*
test_ortho() {
    mresult = (CGLMmat4){
        (float) 0.100000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.100000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) - 0.020000, (float) 0.000000,
        (float) - 0.000000, (float) - 0.000000, (float) - 1.000000, (float) 1.000000
    };
    mcompare = cglmOrtho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    mu_run_test(compare_matrix);

    return NULL;
}

static char*
test_scalarMultVec3() {
    CGLMvec3 a;
    a = (CGLMvec3){(float) 1.0, (float) 1.0, (float) - 1.0};

    vresult = (CGLMvec3){(float) 2.0, (float) 2.0, (float) - 2.0};
    vcompare = cglmScalarMultVec3(a, (float) 2.0);
    mu_run_test(compare_vec3);

    vresult = (CGLMvec3){(float) - 2.0, (float) - 2.0, (float) 2.0};
    vcompare = cglmScalarMultVec3(a, (float) - 2.0);
    mu_run_test(compare_vec3);

    return NULL;
}

static char*
test_scalarMultMat4() {
    CGLMmat4 a;

    mresult = (CGLMmat4){
        (float) 0.000000, (float) 2.000000, (float) - 2.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000
    };

    a = (CGLMmat4){
        (float) 0.000000, (float) 1.000000, (float) - 1.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000
    };

    mcompare = cglmScalarMultMat4(a, (float) 2.0);
    mu_run_test(compare_matrix);

    mresult = (CGLMmat4){
        (float) 0.000000, (float) - 2.000000, (float) 2.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000
    };

    a = (CGLMmat4){
        (float) 0.000000, (float) 1.000000, (float) - 1.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000
    };

    mcompare = cglmScalarMultMat4(a, (float) - 2.0);
    mu_run_test(compare_matrix);

    return NULL;
}

static char*
test_frustum() {
    mresult = (CGLMmat4){
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) - 1.000000, (float) - 1.000000,
        (float) 0.000000, (float) 0.000000, (float) - 0.000000, (float) 0.000000
    };
    mcompare = cglmFrustum(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    mu_run_test(compare_matrix);

    return NULL;
}

static char*
test_mat4() {
    float diag;
    diag = (float) 1;
    mresult = (CGLMmat4){
        diag, 0, 0, 0,
        0, diag, 0, 0,
        0, 0, diag, 0,
        0, 0, 0, diag
    };

    mcompare = cglmMat4(diag);
    mu_run_test(compare_matrix);

    diag = (float) 0;
    mresult = (CGLMmat4){
        diag, 0, 0, 0,
        0, diag, 0, 0,
        0, 0, diag, 0,
        0, 0, 0, diag
    };

    mcompare = cglmMat4(diag);
    mu_run_test(compare_matrix);

    diag = (float) 0.4;
    mresult = (CGLMmat4){
        diag, 0, 0, 0,
        0, diag, 0, 0,
        0, 0, diag, 0,
        0, 0, 0, diag
    };

    mcompare = cglmMat4(diag);
    mu_run_test(compare_matrix);

    return NULL;
}

static char*
test_normalize() {
    CGLMvec3 a;

    //
    vresult = (CGLMvec3){1, 0, 0};
    a = (CGLMvec3){1, 0, 0};
    vcompare = cglmNormalize(a);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){0, 0, 1};

    a = (CGLMvec3){0, 0, 2};
    vcompare = cglmNormalize(a);
    mu_run_test(compare_vec3);


    //
    vresult = (CGLMvec3){0, 0, -1};

    a = (CGLMvec3){0, 0, -2};
    vcompare = cglmNormalize(a);
    mu_run_test(compare_vec3);

    return NULL;
}

static char*
test_cross() {
    CGLMvec3 a;
    CGLMvec3 b;

    //
    vresult = (CGLMvec3){0, 0, 0};
    a = (CGLMvec3){1, 1, 1};
    b = (CGLMvec3){1, 1, 1};
    vcompare = cglmCross(a, b);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){0, 0, 0};
    a = (CGLMvec3){0, 0, 0};
    b = (CGLMvec3){1, 1, 1};
    vcompare = cglmCross(a, b);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){0, 0, 0};
    a = (CGLMvec3){0, 0, 0};
    b = (CGLMvec3){1, 1, 1};
    vcompare = cglmCross(a, b);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){-2.5, -3, -3.5};
    a = (CGLMvec3){-1, 2, -1};
    b = (CGLMvec3){1, 1.5, -2};
    vcompare = cglmCross(a, b);
    mu_run_test(compare_vec3);

    return NULL;
}

static char*
test_subsVec3() {
    CGLMvec3 a;
    CGLMvec3 b;

    //
    vresult = (CGLMvec3){0, 0, 0};
    a = (CGLMvec3){1, 1, 1};
    b = (CGLMvec3){1, 1, 1};
    vcompare = cglmSubsVec3(a, b);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){-1, -1, -1};
    a = (CGLMvec3){0, 0, 0};
    b = (CGLMvec3){1, 1, 1};
    vcompare = cglmSubsVec3(a, b);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){1, 1, 1};
    a = (CGLMvec3){1, 1, 1};
    b = (CGLMvec3){0, 0, 0};
    vcompare = cglmSubsVec3(a, b);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){-2, 0.5, 1};
    a = (CGLMvec3){-1, 2, -1};
    b = (CGLMvec3){1, 1.5, -2};
    vcompare = cglmSubsVec3(a, b);
    mu_run_test(compare_vec3);

    return NULL;
}

static char*
test_addVec3() {
    CGLMvec3 a;
    CGLMvec3 b;

    //
    vresult = (CGLMvec3){(float) 0, (float) 0, (float) 0};
    a = (CGLMvec3){(float) 0, (float) 0, (float) 0};
    b = (CGLMvec3){(float) 0, (float) 0, (float) 0};
    vcompare = cglmAddVec3(a, b);
    mu_run_test(compare_vec3);


    //
    vresult = (CGLMvec3){(float) 0, (float) 0, (float) 0};
    a = (CGLMvec3){(float) 1, (float) 1, (float) 1};
    b = (CGLMvec3){(float) - 1, (float) - 1, (float) - 1};
    vcompare = cglmAddVec3(a, b);
    mu_run_test(compare_vec3);

    //
    vresult = (CGLMvec3){(float) 0.5, (float) - 0.82, (float) 0.12};
    a = (CGLMvec3){(float) 1.5, (float) - 1.32, (float) 1.32};
    b = (CGLMvec3){(float) - 1, (float) 0.5, (float) - 1.2};
    vcompare = cglmAddVec3(a, b);
    mu_run_test(compare_vec3);

    return NULL;
}

static char*
test_dot() {
    CGLMvec3 a;
    CGLMvec3 b;
    float fresult;
    float fcompare;

    //
    fresult = (float) - 3.744;
    a = (CGLMvec3){(float) 1.5, (float) - 1.32, (float) 1.32};
    b = (CGLMvec3){(float) - 1, (float) 0.5, (float) - 1.2};
    fcompare = cglmDot(a, b);
    mu_assert("compare on dot float", fabs(fcompare - fresult) < LARGE_ROUND);

    //
    fresult = (float) 3;
    a = (CGLMvec3){(float) 1, (float) 1, (float) 1};
    b = (CGLMvec3){(float) 1, (float) 1, (float) 1};
    fcompare = cglmDot(a, b);
    mu_assert("compare on dot float", fabs(fcompare - fresult) < LARGE_ROUND);


    //
    fresult = (float) 0;
    a = (CGLMvec3){(float) 0, (float) 0, (float) 0};
    b = (CGLMvec3){(float) 1, (float) 1, (float) 1};
    fcompare = cglmDot(a, b);
    mu_assert("compare on dot float", fabs(fcompare - fresult) < LARGE_ROUND);


    //
    fresult = (float) - 0.5;
    a = (CGLMvec3){(float) - 1, (float) 1.5, (float) - 1};
    b = (CGLMvec3){(float) 1, (float) 1, (float) 1};
    fcompare = cglmDot(a, b);
    mu_assert("compare on dot float", fabs(fcompare - fresult) < LARGE_ROUND);

    //
    fresult = (float) - 3.2;
    a = (CGLMvec3){(float) - 1, (float) 1.5, (float) - 1};
    b = (CGLMvec3){(float) 1, (float) - 0.8, (float) 1};
    fcompare = cglmDot(a, b);
    mu_assert("compare on dot float", fabs(fcompare - fresult) < LARGE_ROUND);

    return NULL;
}

static char*
test_multMat4() {
    CGLMmat4 mult1, mult2, mult3;

    mresult = (CGLMmat4){
        (float) 1.446000, (float) - 0.991900, (float) - 0.924100, (float) - 0.922300,
        (float) 0.000000, (float) 2.755500, (float) - 0.924100, (float) - 0.922300,
        (float) 1.436900, (float) 0.554500, (float) - 5.125900, (float) - 5.315500,
        (float) 0.287100, (float) 0.110800, (float) 0.103200, (float) 0.103000
    };

    mult1 = (CGLMmat4){
        (float) 1.792600, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 1.792600, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) - 1.002000, (float) - 1.000000,
        (float) 0.000000, (float) 0.000000, (float) - 0.200200, (float) 0.000000
    };
    mult2 = (CGLMmat4){
        (float) 0.600000, (float) - 0.411600, (float) 0.686000, (float) 0.000000,
        (float) 0.000000, (float) 0.857500, (float) 0.514500, (float) 0.000000,
        (float) - 0.800000, (float) - 0.308700, (float) 0.514500, (float) 0.000000,
        (float) - 0.000000, (float) - 0.000000, (float) - 5.831000, (float) 1.000000
    };
    mult3 = (CGLMmat4){
        (float) 1.344400, (float) 0.000000, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 1.792600, (float) 0.000000, (float) 0.000000,
        (float) 0.000000, (float) 0.000000, (float) - 1.002000, (float) - 1.000000,
        (float) 0.000000, (float) 0.000000, (float) - 0.200200, (float) 0.000000
    };

    mcompare = cglmMultMat4(cglmMultMat4(mult1, mult2), mult3);
    mu_run_test(compare_matrix);

    return NULL;
}

static char*
test_lookAt() {
    CGLMvec3 a;
    CGLMvec3 b;
    CGLMvec3 c;

    mresult = (CGLMmat4){
        (float) 0.600000, (float) - 0.411600, (float) 0.686000, (float) 0.000000,
        (float) 0.000000, (float) 0.857500, (float) 0.514500, (float) 0.000000,
        (float) - 0.800000, (float) - 0.308700, (float) 0.514500, (float) 0.000000,
        (float) - 0.000000, (float) - 0.000000, (float) - 5.831000, (float) 1.000000
    };
    a = (CGLMvec3){(float) 4, (float) 3, (float) 3};
    b = (CGLMvec3){(float) 0, (float) 0, (float) 0};
    c = (CGLMvec3){(float) 0, (float) 1, (float) 0};
    mcompare = cglmLookAt(a, b, c);
    mu_run_test(compare_matrix);

    return NULL;
}

static char*
cglm_test(char* test_name) {

    if (!test_name) return "no shuch test";

    if (strcmp(test_name, "Perspective") == 0)
        mu_run_test(test_perspective);
    else if (strcmp(test_name, "Mat4") == 0)
        mu_run_test(test_mat4);
    else if (strcmp(test_name, "Normalize") == 0)
        mu_run_test(test_normalize);
    else if (strcmp(test_name, "Cross") == 0)
        mu_run_test(test_cross);
    else if (strcmp(test_name, "SubsVec3") == 0)
        mu_run_test(test_subsVec3);
    else if (strcmp(test_name, "AddVec3") == 0)
        mu_run_test(test_addVec3);
    else if (strcmp(test_name, "Dot") == 0)
        mu_run_test(test_dot);
    else if (strcmp(test_name, "LookAt") == 0)
        mu_run_test(test_lookAt);
    else if (strcmp(test_name, "MultMat4") == 0)
        mu_run_test(test_multMat4);
    else if (strcmp(test_name, "Ortho") == 0)
        mu_run_test(test_ortho);
    else if (strcmp(test_name, "Frustum") == 0)
        mu_run_test(test_frustum);
    else if (strcmp(test_name, "ScalarMultMat4") == 0)
        mu_run_test(test_scalarMultMat4);
    else if (strcmp(test_name, "ScalarMultVec3") == 0)
        mu_run_test(test_scalarMultVec3);
    else
        return "no such test";

    return NULL;
}

int
main(int argc, char* argv[]) {
    char* result = cglm_test(argv[1]);

    if (result != 0)
        printf("%s\n", result);

    return result != NULL;
}

