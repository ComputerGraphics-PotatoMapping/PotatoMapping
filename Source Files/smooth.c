/*  
    smooth.c
    Nate Robins, 1998

    Model viewer program.  Exercises the glm library.
*/

#include <GL/glew.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <GL/freeglut.h>
#include "gltb.h"
#include "glm.h"
#include "dirent32.h"

//#include "MathHelper.h"

#pragma comment( linker, "/entry:\"mainCRTStartup\"" )  // set the entry point to be main()

#define DATA_DIR "data/"

#define BUFFER_OFFSET(i) ((char *) NULL + (i))

char*      model_file = NULL;		/* name of the obect file */
GLuint     model_list = 0;		    /* display list for object */
GLMmodel*  model;			        /* glm model data structure */
GLfloat    scale;			        /* original scale factor */
GLfloat    smoothing_angle = 90.0;	/* smoothing angle */
GLfloat    weld_distance = 0.00001;	/* epsilon for welding vertices */
GLboolean  facet_normal = GL_FALSE;	/* draw with facet normal? */
GLboolean  bounding_box = GL_FALSE;	/* bounding box on? */
GLboolean  performance = GL_FALSE;	/* performance counter on? */
GLboolean  stats = GL_FALSE;		/* statistics on? */
GLuint     material_mode = 0;		/* 0=none, 1=color, 2=material */
GLint      entries = 0;			    /* entries in model menu */
GLdouble   pan_x = 0.0;
GLdouble   pan_y = 0.0;
GLdouble   pan_z = 0.0;

GLboolean ourModel = GL_TRUE;

// unsigned char bitmapHeader[54];

struct texture {
	unsigned int dataPosition;
	unsigned int width, height;
	unsigned int imageSize;
	unsigned char * data;
};

// Struct for each vertex
struct vertex {
	GLfloat x, y, z;
	GLfloat nx, ny, nz;
	GLfloat tx, ty, tz;
	//Tangent Vector
	GLfloat tanvecx, tanvecy, tanvecz;
	//Bitangent Vector
	GLfloat bivecx, bivecy, bivecz;
	//Normal Vector
	GLfloat normvecx, normvecy, normvecz;
};

//Compute the Tangent Matrix
void tangentMatrix(struct vertex one, const struct vertex two, const struct vertex three) {
	GLfloat one_x, one_y, one_z, two_x, two_y, two_z, one_u_x, one_u_y, two_u_x, two_u_y;
	//Get the edges
	one_x = two.x - one.x;
	one_y = two.y - one.y;
	one_z = two.z - one.z;

	two_x = three.x - one.x;
	two_y = three.y - one.y;
	two_z = three.z - one.z;

	//Get the difference in tangents
	one_u_x = two.tx - one.tx;
	one_u_y = two.ty - one.ty;

	two_u_x = three.tx - one.tx;
	two_u_y = three.ty - one.ty;

	GLfloat determiner = one_u_x * two_u_y - two_u_x * one_u_y;
	//Tangent Vector
	one.tanvecx = (one_x * two_u_y - two_x * one_u_y) / determiner;
	one.tanvecy = (one_y * two_u_y - two_y * one_u_y) / determiner;
	one.tanvecz = (one_z * two_u_y - two_z * one_u_y) / determiner;
	//Bitangent Vector
	one.bivecx = (-1 * one_x * two_u_x - two_x * one_u_x) / determiner;
	one.bivecy = (-1 * one_y * two_u_x - two_y * one_u_x) / determiner;
	one.bivecz = (-1 * one_z * two_u_x - two_z * one_u_x) / determiner;
	//Normal Vector
	one.normvecx = one.bivecy * one.tanvecz - one.bivecz * one.tanvecy;
	one.normvecy = one.bivecz * one.tanvecx - one.bivecx * one.tanvecz;
	one.normvecz = one.bivecx * one.tanvecy - one.bivecy * one.tanvecx;
}

#if defined(_WIN32)
#include <sys/timeb.h>
#define CLK_TCK 1000
#else
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#endif
float
elapsed(void)
{
    static long begin = 0;
    static long finish, difference;
    
#if defined(_WIN32)
    static struct timeb tb;
    ftime(&tb);
    finish = tb.time*1000+tb.millitm;
#else
    static struct tms tb;
    finish = times(&tb);
#endif
    
    difference = finish - begin;
    begin = finish;
    
    return (float)difference/(float)CLK_TCK;
}

void
shadowtext(int x, int y, char* s) 
{
    int lines;
    char* p;
    
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 
        0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3ub(0, 0, 0);
    glRasterPos2i(x+1, y-1);
    for(p = s, lines = 0; *p; p++) {
        if (*p == '\n') {
            lines++;
            glRasterPos2i(x+1, y-1-(lines*18));
        }
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    }
    glColor3ub(0, 128, 255);
    glRasterPos2i(x, y);
    for(p = s, lines = 0; *p; p++) {
        if (*p == '\n') {
            lines++;
            glRasterPos2i(x, y-(lines*18));
        }
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}

void
lists(void)
{
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat specular[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat shininess = 65.0;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
	//Generate UV Coordinates for the model
	//glmSpheremapTexture(model);

    if (model_list)
        glDeleteLists(model_list, 1);
    
    /* generate a list */
    if (material_mode == 0) { 
        if (facet_normal)
            model_list = glmList(model, GLM_FLAT);
        else
            model_list = glmList(model, GLM_SMOOTH);
    } else if (material_mode == 1) {
        if (facet_normal)
            model_list = glmList(model, GLM_FLAT | GLM_COLOR);
        else
            model_list = glmList(model, GLM_SMOOTH | GLM_COLOR);
    } else if (material_mode == 2) {
        if (facet_normal)
            model_list = glmList(model, GLM_FLAT | GLM_MATERIAL);
        else
            model_list = glmList(model, GLM_SMOOTH | GLM_TEXTURE);
    }
}

//////////////////////////////// Section Beginning: File I/O Functions //////////////////////////////////////////////////

struct texture loadTexture(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("File could not be opened\n");
		return;
	}

	unsigned char header[54];
	
	//Check if file at the given path is a BMP file
	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return;
	}

	struct texture texture;

	// Read ints from the byte array
	texture.dataPosition = *(int*)&(header[0x0A]);
	texture.imageSize = *(int*)&(header[0x22]);
	texture.width = *(int*)&(header[0x12]);
	texture.height = *(int*)&(header[0x16]);

	printf("Width: %d \n Height: %d \n Size: %d \n", texture.width, texture.height, texture.imageSize);
	printf("Tex Coor %d \n", sizeof (model->texcoords));

	//If BMP File is not formatted correctly, format it
	if (texture.imageSize == 0) { texture.imageSize = texture.width * texture.height * 3; }
	if (texture.dataPosition == 0) { texture.dataPosition = 54; }

	texture.data = (char *)malloc(texture.imageSize * sizeof(unsigned char));
	fread(texture.data, sizeof(unsigned char), texture.imageSize, file);			  // Read the data
	fclose(file);													  // Close the file

	return texture;
}

char * loadShaders (const char * fileName) {
	FILE* filePointer = fopen(fileName, "r");                         // Opens file and associates with it a stream that can be identified with the pointer returned
	if (filePointer == NULL) {
		printf("File Could Not Be Found\n");
		return NULL;
	}
	fseek(filePointer, 0, SEEK_END);                                  // Sets the position indicator associated with the stream to the end of the file
	long fileLength = ftell(filePointer);                             // Returns the current value of the position indicator of the stream
	fseek(filePointer, 0, SEEK_SET);                                  // Sets the position indicator associated with the stream to the beginning of the file
	char* fileContents = (char *)malloc(fileLength + 1);			  // Creates array to store file contents
	for (int i = 0; i < fileLength + 1; ++i) {						  // Initializing array with default values
		fileContents[i] = 0;
	}
	fread(fileContents, 1, fileLength, filePointer);                  // Reads an array of fileLength elements each one with a size of 1 byte,
																	  // from the filePointer stream and stores them in the block of memory specified by fileContents
	fileContents[fileLength + 1] = '\0';                              // Adds terminating character for "C-style" arrays
	fclose(filePointer);                                              // Closes the filePointer associated stream and disassociates it
	return fileContents;                                              // Returns the contents of the file
}

//////////////////////////////// Section Ending: File I/O Functions //////////////////////////////////////////////////

//////////////////////////////// Section Beginning: Shader Functions //////////////////////////////////////////////////
GLboolean isCompiled(GLint shaderReference) {
	GLint compiled = 0;                                                   // Initialzes boolean variable
	glGetShaderiv(shaderReference, GL_COMPILE_STATUS, &compiled);         // Stores a GL_TRUE value if compilation of the shader was successful, GL_FALSE otherwise
	if (compiled) { return GL_TRUE; }                                     // If compilation was successful
	return GL_FALSE;                                                      // If compilation was unsuccessful
}

GLuint createVertexShader(const char* shader) {
	GLuint vertexShaderReference = glCreateShader(GL_VERTEX_SHADER);                // Creates a vertex shader object
	glShaderSource(vertexShaderReference, 1, (const GLchar**)&shader, NULL);        // Sets the source code in vertexShaderReference to the source code in shader
	glCompileShader(vertexShaderReference);                                         // Compiles the source code in vertexShaderReference
	if (isCompiled(vertexShaderReference)) { return vertexShaderReference; }        // If compilation was successful
	return -1;                                                                      // If compilation was unsuccessful
}

GLuint createFragmentShader(const char* shader) {
	GLuint fragmentShaderReference = glCreateShader(GL_FRAGMENT_SHADER);             // Creates a fragment shader object
	glShaderSource(fragmentShaderReference, 1, (const GLchar**)&shader, NULL);       // Sets the source code in fragmentShaderReference to the source code in shader
	glCompileShader(fragmentShaderReference);                                        // Compiles the source code in fragmentShaderReference
	if (isCompiled(fragmentShaderReference)) { return fragmentShaderReference; }     // If compilation was successful
	return -1;                                                                       // If compilation was unsuccessful
}

GLuint createShaderProgram(GLuint vertexShaderReference, GLuint fragmentShaderReference) {
	GLuint shaderReference = glCreateProgram();                                      // Creates an empty program object and returns a non-zero value for which it can be referenced
	glAttachShader(shaderReference, vertexShaderReference);                          // Attaches the vertexShaderReference to the shader program
	glAttachShader(shaderReference, fragmentShaderReference);                        // Attaches the fragmentShaderReference to the shader program
	glLinkProgram(shaderReference);                                                  // Links the program object. Creates executables for the attached vertex shader and fragment shader
																					 // that will run on the programmable vertex processor and programmable fragment processor respectively
	return shaderReference;                                                          // Returns the reference to the shader program
}
//////////////////////////////// Section Ending: Shader Functions //////////////////////////////////////////////////


void
init(void)
{
	gltbInit(GLUT_LEFT_BUTTON);

	/* read in the model */
	model = glmReadOBJ(model_file);
	scale = glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, smoothing_angle);

	if (model->nummaterials > 0)
		material_mode = 2;

	/* create new display lists */
	lists();

	//Generate UV Coordinates for the model
	//glmSpheremapTexture(model);

	///////////////////////////// Section Beginning: Texture Mapping & Normal Mapping //////////////////////////////

	glewInit();

	char* vertexShaderSourceCode = loadShaders("vShader.vert");
	char* fragmentShaderSourceCode = loadShaders("fShader.frag");
	GLuint vertexShaderReference = createVertexShader(vertexShaderSourceCode);
	GLuint fragmentShaderReference = createFragmentShader(fragmentShaderSourceCode);
	GLuint shaderProgramReference = createShaderProgram(vertexShaderReference, fragmentShaderReference);

	// Loading the texture map
	struct texture textureMap = loadTexture("StoneLowerQuality.bmp");															// Load the Texture Map
	glEnable(GL_TEXTURE_2D);																									// If enabled and no fragment shader is active, 
																																// two-dimensional texturing is performed
	GLuint textureMapName;																										// Declare Texture Name
	glGenTextures(1, &textureMapName);																							// Generate texture name
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMapName);																				// Binds a named texture to a texturing target
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureMap.width, textureMap.height, 0, GL_BGR, GL_UNSIGNED_BYTE, textureMap.data);	// Specifies a two-dimensional image texture

	// Loading the normal map
	//struct texture normalMap = loadTexture("StoneNormalMap.bmp");															// Load the Texture Map
	//glEnable(GL_TEXTURE_2D);																								// If enabled and no fragment shader is active, 
	//																														// two-dimensional texturing is performed
	//GLuint normalMapName;																									// Declare Texture Name
	//glGenTextures(1, &normalMapName);																						// Generate texture name
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, normalMapName);																			// Binds a named texture to a texturing target
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalMap.width, normalMap.height, 0, GL_BGR, GL_UNSIGNED_BYTE, normalMap.data);	// Specifies a two-dimensional image texture

	// Creating a Vertex Buffer
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// Vertex Shader Inputs
	GLint vertex2 = glGetAttribLocation(shaderProgramReference, "vertex");
	GLint normal2 = glGetAttribLocation(shaderProgramReference, "normal");
	GLint texcoord2 = glGetAttribLocation(shaderProgramReference, "texcoord");
	GLint _tangent2 = glGetAttribLocation(shaderProgramReference, "_tangent");
	GLint _bitangent2 = glGetAttribLocation(shaderProgramReference, "_bitangent");
	GLint _normal2 = glGetAttribLocation(shaderProgramReference, "_normal");

	// Vertex Shader Uniforms
	GLint light_position2 = glGetUniformLocation(shaderProgramReference, "light_position");
	GLint Projection2 = glGetUniformLocation(shaderProgramReference, "Projection");
	GLint View2 = glGetUniformLocation(shaderProgramReference, "View");
	GLint Model2 = glGetUniformLocation(shaderProgramReference, "Model");

	// Fragment Shader Uniforms
	GLint texture_sample2 = glGetUniformLocation(shaderProgramReference, "texture_sample");
	GLint normal_sample2 = glGetUniformLocation(shaderProgramReference, "normal_sample");
	GLint flag2 = glGetUniformLocation(shaderProgramReference, "flag");


	//Sets the parameters for the texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//glUseProgram(shaderProgramReference);
	//glUniform3f(light_position2, light_position.x, light_position.y, light_position.z);
	//glUniformMatrix4fv(Projection2, 1, GL_FALSE, value_ptr(Projection));
	//glUniformMatrix4fv(View2, 1, GL_FALSE, value_ptr(View));
	//glUniformMatrix4fv(Model2, 1, GL_FALSE, value_ptr(Model));
	//glUniform1i(flag2, (int)normal);
	//glUniform1i(texture_sample2, 0);
	//glUniform1i(normal_sample2, 1);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture2);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture2normal);

	//glEnableVertexAttribArray(vertex2);
	//glVertexAttribPointer(vertex2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), 0);
	//glEnableVertexAttribArray(normal2);
	//glVertexAttribPointer(normal2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 12);
	//glEnableVertexAttribArray(texcoord2);
	//glVertexAttribPointer(texcoord2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 24);
	//glEnableVertexAttribArray(_tangent2);
	//glVertexAttribPointer(_tangent2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 36);
	//glEnableVertexAttribArray(_bitangent2);
	//glVertexAttribPointer(_bitangent2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 48);
	//glEnableVertexAttribArray(_normal2);
	//glVertexAttribPointer(_normal2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 60);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);

	//glDrawElements(GL_QUADS, 16, GL_UNSIGNED_INT, 0);

	///////////////////////////// Section Ending: Texture Mapping & Normal Mapping //////////////////////////////

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

}

void
reshape(int width, int height)
{
    gltbReshape(width, height);
    
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)height / (GLfloat)width, 1.0, 128.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);
}

#define NUM_FRAMES 5
void
display(void)
{
    static char s[256], t[32];
    static char* p;
    static int frames = 0;
    
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    
    glTranslatef(pan_x, pan_y, 0.0);
    
    gltbMatrix();
    
#if 0   /* glmDraw() performance test */
    if (material_mode == 0) { 
        if (facet_normal)
            glmDraw(model, GLM_FLAT);
        else
            glmDraw(model, GLM_SMOOTH);
    } else if (material_mode == 1) {
        if (facet_normal)
            glmDraw(model, GLM_FLAT | GLM_COLOR);
        else
            glmDraw(model, GLM_SMOOTH | GLM_COLOR);
    } else if (material_mode == 2) {
        if (facet_normal)
            glmDraw(model, GLM_FLAT | GLM_MATERIAL);
        else
            glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
    }
#else
    glCallList(model_list);
#endif
    
	// Important! Pass that data to the shader variables
	/*glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, M);
	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, V);
	glUniformMatrix4fv(perspectiveMatrixID, 1, GL_TRUE, P);
	glUniformMatrix4fv(allRotsMatrixID, 1, GL_TRUE, allRotsMatrix);
	glUniform4fv(lightID, 1, light);
	*/
    glDisable(GL_LIGHTING);
    if (bounding_box) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glColor4f(1.0, 0.0, 0.0, 0.25);
        glutSolidCube(2.0);
        glDisable(GL_BLEND);
    }
    
    glPopMatrix();

    if (stats) {
        /* XXX - this could be done a _whole lot_ faster... */
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        glColor3ub(0, 0, 0);
        sprintf(s, "%s\n%d vertices\n%d triangles\n%d normals\n"
            "%d texcoords\n%d groups\n%d materials",
            model->pathname, model->numvertices, model->numtriangles, 
            model->numnormals, model->numtexcoords, model->numgroups,
            model->nummaterials);
        shadowtext(5, height-(5+18*1), s);
    }
    /* spit out frame rate. */
    frames++;
    if (frames > NUM_FRAMES) {
        sprintf(t, "%g fps", frames/elapsed());
        frames = 0;
    }
    if (performance) {
        shadowtext(5, 5, t);
    }
    
    glutSwapBuffers();
    glEnable(GL_LIGHTING);
}

void
keyboard(unsigned char key, int x, int y)
{
    GLint params[2];
    
    switch (key) {
    case 'h':
        printf("help\n\n");
        printf("w         -  Toggle wireframe/filled\n");
        printf("c         -  Toggle culling\n");
        printf("n         -  Toggle facet/smooth normal\n");
        printf("b         -  Toggle bounding box\n");
        printf("r         -  Reverse polygon winding\n");
        printf("m         -  Toggle color/material/none mode\n");
        printf("p         -  Toggle performance indicator\n");
        printf("s/S       -  Scale model smaller/larger\n");
        printf("t         -  Show model stats\n");
        printf("o         -  Weld vertices in model\n");
        printf("+/-       -  Increase/decrease smoothing angle\n");
        printf("W         -  Write model to file (out.obj)\n");
        printf("q/escape  -  Quit\n\n");
        break;
    
	case 't':
        stats = !stats;
        break;
        
    case 'p':
        performance = !performance;
        break;
        
    case 'm':
        material_mode++;
        if (material_mode > 2)
            material_mode = 0;
        printf("material_mode = %d\n", material_mode);
        lists();
        break;
        
    case 'd':
        glmDelete(model);
        init();
        lists();
        break;
        
    case 'w':
        glGetIntegerv(GL_POLYGON_MODE, params);
        if (params[0] == GL_FILL)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
        
    case 'c':
        if (glIsEnabled(GL_CULL_FACE))
            glDisable(GL_CULL_FACE);
        else
            glEnable(GL_CULL_FACE);
        break;
        
    case 'b':
        bounding_box = !bounding_box;
        break;
        
    case 'n':
        facet_normal = !facet_normal;
        lists();
        break;
        
    case 'r':
        glmReverseWinding(model);
        lists();
        break;
        
    case 's':
        glmScale(model, 0.8);
        lists();
        break;
        
    case 'S':
        glmScale(model, 1.25);
        lists();
        break;
        
    case 'o':
        //printf("Welded %d\n", glmWeld(model, weld_distance));
        glmVertexNormals(model, smoothing_angle);
        lists();
        break;
        
    case 'O':
        weld_distance += 0.01;
        printf("Weld distance: %.2f\n", weld_distance);
        glmWeld(model, weld_distance);
        glmFacetNormals(model);
        glmVertexNormals(model, smoothing_angle);
        lists();
        break;
        
    case '-':
        smoothing_angle -= 1.0;
        printf("Smoothing angle: %.1f\n", smoothing_angle);
        glmVertexNormals(model, smoothing_angle);
        lists();
        break;
        
    case '+':
        smoothing_angle += 1.0;
        printf("Smoothing angle: %.1f\n", smoothing_angle);
        glmVertexNormals(model, smoothing_angle);
        lists();
        break;
        
    case 'W':
        glmScale(model, 1.0/scale);
        glmWriteOBJ(model, "out.obj", GLM_SMOOTH | GLM_MATERIAL);
        break;
        
    case 'R':
        {
            GLuint i;
            GLfloat swap;
            for (i = 1; i <= model->numvertices; i++) {
                swap = model->vertices[3 * i + 1];
                model->vertices[3 * i + 1] = model->vertices[3 * i + 2];
                model->vertices[3 * i + 2] = -swap;
            }
            glmFacetNormals(model);
            lists();
            break;
        }
        
    case 27:
        exit(0);
        break;
    }
    
    glutPostRedisplay();
}

void
menu(int item)
{
    int i = 0;
    DIR* dirp;
    char* name;
    struct dirent* direntp;
    
    if (item > 0) {
        keyboard((unsigned char)item, 0, 0);
    } else {
        dirp = opendir(DATA_DIR);
        while ((direntp = readdir(dirp)) != NULL) {
            if (strstr(direntp->d_name, ".obj")) {
                i++;
                if (i == -item)
                    break;
            }
        }
        if (!direntp)
            return;
        name = (char*)malloc(strlen(direntp->d_name) + strlen(DATA_DIR) + 1);
        strcpy(name, DATA_DIR);
        strcat(name, direntp->d_name);
        model = glmReadOBJ(name);
        scale = glmUnitize(model);
        glmFacetNormals(model);
        glmVertexNormals(model, smoothing_angle);
        
        if (model->nummaterials > 0)
            material_mode = 2;
        else
            material_mode = 0;
        
        lists();
        free(name);
        
        glutPostRedisplay();
    }
}

static GLint      mouse_state;
static GLint      mouse_button;

void
mouse(int button, int state, int x, int y)
{
    GLdouble model[4*4];
    GLdouble proj[4*4];
    GLint view[4];
    
    /* fix for two-button mice -- left mouse + shift = middle mouse */
    if (button == GLUT_LEFT_BUTTON && glutGetModifiers() & GLUT_ACTIVE_SHIFT)
        button = GLUT_MIDDLE_BUTTON;
    
    gltbMouse(button, state, x, y);
    
    mouse_state = state;
    mouse_button = button;
    
    if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON) {
        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);
        gluProject((GLdouble)x, (GLdouble)y, 0.0,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        pan_y = -pan_y;
    }
    
    glutPostRedisplay();
}

void
motion(int x, int y)
{
    GLdouble model[4*4];
    GLdouble proj[4*4];
    GLint view[4];
    
    gltbMotion(x, y);
    
    if (mouse_state == GLUT_DOWN && mouse_button == GLUT_MIDDLE_BUTTON) {
        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);
        gluProject((GLdouble)x, (GLdouble)y, 0.0,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
            model, proj, view,
            &pan_x, &pan_y, &pan_z);
        pan_y = -pan_y;
    }
    
    glutPostRedisplay();
}

int
main(int argc, char** argv)
{
    int buffering = GLUT_DOUBLE;
    struct dirent* direntp;
    DIR* dirp;
    int models;

    glutInitWindowSize(512, 512);
    glutInit(&argc, argv);
    
    while (--argc) {
        if (strcmp(argv[argc], "-sb") == 0)
            buffering = GLUT_SINGLE;
        else
            model_file = argv[argc];
    }
    
    if (!model_file) {
        model_file = "data/PotatoV4.obj";
    }
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | buffering);
    glutCreateWindow("Smooth");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    
    models = glutCreateMenu(menu);
    dirp = opendir(DATA_DIR);
    if (!dirp) {
        fprintf(stderr, "%s: can't open data directory.\n", argv[0]);
    } else {
        while ((direntp = readdir(dirp)) != NULL) {
            if (strstr(direntp->d_name, ".obj")) {
                entries++;
                glutAddMenuEntry(direntp->d_name, -entries);
            }
        }
        closedir(dirp);
    }
    
    glutCreateMenu(menu);
    glutAddMenuEntry("Smooth", 0);
    glutAddMenuEntry("", 0);
    glutAddSubMenu("Models", models);
    glutAddMenuEntry("", 0);
    glutAddMenuEntry("[w]   Toggle wireframe/filled", 'w');
    glutAddMenuEntry("[c]   Toggle culling on/off", 'c');
    glutAddMenuEntry("[n]   Toggle face/smooth normals", 'n');
    glutAddMenuEntry("[b]   Toggle bounding box on/off", 'b');
    glutAddMenuEntry("[p]   Toggle frame rate on/off", 'p');
    glutAddMenuEntry("[t]   Toggle model statistics", 't');
    glutAddMenuEntry("[m]   Toggle color/material/none mode", 'm');
    glutAddMenuEntry("[r]   Reverse polygon winding", 'r');
    glutAddMenuEntry("[s]   Scale model smaller", 's');
    glutAddMenuEntry("[S]   Scale model larger", 'S');
    glutAddMenuEntry("[o]   Weld redundant vertices", 'o');
    glutAddMenuEntry("[+]   Increase smoothing angle", '+');
    glutAddMenuEntry("[-]   Decrease smoothing angle", '-');
    glutAddMenuEntry("[W]   Write model to file (out.obj)", 'W');
    glutAddMenuEntry("", 0);
    glutAddMenuEntry("[Esc] Quit", 27);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    init();
 
    glutMainLoop();
    return 0;
}
