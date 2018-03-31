# PotatoMapping

## Configuration for Visual Studio 2017

1. Add Source Files to "Source Files" in Visual Studio 2017
2. Open "smooth.c"
3. Add the "data" folder and the "freeglut.dll" to the debug folder
4. In the top bar, select Project and click on "PotatoMapping Properties"

5. For each click on the drop down:
	5a. VC++ Directories
		Add the full path to freeglut/include to Include Directories
		Add the full path to freeglut/lib Library Directories

	5b. C/C++ -> Preprocessor
		Add _CRT_SECURE_NO_DEPRECATE;_CRT_NONSTDC_NO_DEPRECATE; to Preprocessor Definitions

	5c. Linker -> Input
		Add OpenGL32.lib;freeglut.lib; to Additional Dependencies

	5d. Linker -> System
		Select console in SubSystem

6. Restart Visual Studio
7. Ctrl+Shft+B to build the program
8. Run the executable in the debug folder