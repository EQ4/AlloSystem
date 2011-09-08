#include "allocore/system/al_MainLoop.hpp"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>		// snprintf
#include <stdlib.h>		// exit


// GLUT includes:
#ifdef AL_OSX
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
	#define AL_GRAPHICS_INIT_CONTEXT\
		/* prevents tearing */ \
		{	GLint MacHackVBL = 1;\
			CGLContextObj ctx = CGLGetCurrentContext();\
			CGLSetParameter(ctx,  kCGLCPSwapInterval, &MacHackVBL); }
#endif
#ifdef AL_LINUX
	#include <GL/glew.h>
	#include <GL/glut.h>

	#define AL_GRAPHICS_INIT_CONTEXT\
		{	GLenum err = glewInit();\
			if (GLEW_OK != err){\
  				/* Problem: glewInit failed, something is seriously wrong. */\
  				fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));\
			}\
		}
#endif
#ifdef AL_WIN32
	#include <windows.h>
	#include <GL/glut.h>

	#define AL_GRAPHICS_INIT_CONTEXT
#endif


// native bindings:

extern "C" void al_main_native_init();
extern "C" void al_main_native_attach(al_sec interval);
extern "C" void al_main_native_enter(al_sec interval);

#ifdef AL_LINUX
	extern "C" void al_main_native_attach(al_sec interval) {
		printf("Linux native loop not yet implemented\n");
	}
	extern "C" void al_main_native_enter(al_sec interval) {
		printf("Linux native loop not yet implemented\n");
	}
	extern "C" void al_main_native_init() {}
#endif

namespace al {

static bool gInitialized = false;

////////////////////////////////////////////////////////////////
// GLUT impl:
static void mainGLUTTimerFunc(int id);
static void mainGLUTExitFunc();

static void mainGLUTInit() {
	int   argc   = 1;
	char name[] = {'a','l','l','o'};
	char *argv[] = {name, NULL};
	glutInit(&argc,argv);
	
	atexit(mainGLUTExitFunc);
}

static void mainGLUTExitFunc(){
	// call any exit handlers:
	Main::get().exit();
}

static void mainGLUTTimerFunc(int id) { 
	Main& M = Main::get();
	M.tick();
	if (M.isRunning()) {
        // schedule another tick:
        glutTimerFunc((unsigned int)(1000.0*M.interval()), mainGLUTTimerFunc, 0);
    }
}

////////////////////////////////////////////////////////////////

Main::Handler :: ~Handler() {
	Main::get().remove(*this);
}

////////////////////////////////////////////////////////////////

Main::Main() 
:	mT0(al_time()), mT1(0), 
	mInterval(0.01), 
	mIntervalActual(0.01),
	mLogicalTime(0), 
	mCPU(0),
	mDriver(Main::SLEEP),
	mActive(false)
{
	if (!gInitialized) {
		
		mainGLUTInit();
	
		al_main_native_init();
	
		gInitialized = true;
	}
}

void Main::tick() {
	al_sec t1 = al_time();
	mLogicalTime = t1 - mT0;
	
	mIntervalActual = t1 - mT1;
	mT1 = t1;
	
	// trigger any scheduled functions:
	mQueue.update(mLogicalTime);
	
	// call tick handlers... 
	std::vector<Handler *>::iterator it = mHandlers.begin(); 
	bool active = true; 
	while(it != mHandlers.end()){
		(*it)->onQuit(); 
		++it; 
	}
	
	// measure CPU usage:
	al_sec t2 = al_time();
	al_sec used = (t2-t1)/interval();
	// running average:
	mCPU += 0.1 * (used - mCPU);
}

Main& Main::get() {
	static Main main;
	return main;
}

void Main::start() {
	if (!mActive) {
		mActive = true;
		
		while (mActive) {
			// check inside sleep loop, so that we can smoothly migrate
			// from a sleep-loop to a GLUT or NATIVE loop:
			switch (mDriver) {
				case Main::GLUT:
					// start the GLUT mainloop
					glutTimerFunc((unsigned int)(1000.0*interval()), mainGLUTTimerFunc, 0);
					glutMainLoop();
					break;
				case Main::NATIVE:
					al_main_native_enter(interval());
					break;

				default:
					// sleep version for non-GLUT:
					tick();				
					al_sleep(interval());
					break;
			}
		}
	}
	// trigger exit handlers:
	Main::exit();
}

void Main::stop() {
	if (mActive) {
		mActive = false;
		
		// GLUT can't be stopped; the only option is a hard exit. Yeah, it sucks that bad.
		::exit(0); // Note: this will call our function registered with atexit()
	}
}

void Main::exit() {
	// call exit handlers... 
	std::vector<Handler *>::iterator it = mHandlers.begin(); 
	bool active = true; 
	while(it != mHandlers.end()){
		(*it)->onQuit(); 
		++it; 
	}
}


Main& Main::add(Main::Handler& v) {
	if (std::find(mHandlers.begin(), mHandlers.end(), &v) != mHandlers.end()) {
		mHandlers.push_back(&v);
	}
	return *this;
}

Main& Main::remove(Main::Handler& v) {
	std::vector<Handler *>::iterator it = std::find(mHandlers.begin(), mHandlers.end(), &v);
	if (it != mHandlers.end()) {
		mHandlers.erase(it);
	}
	return *this;
}


} //al::


