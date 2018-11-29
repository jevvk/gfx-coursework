#include <algorithm>
#include <cstring>
#include <iostream>

#include <GL/glut.h>

#include "simulation/Simulation.h"
#include "simulation/CPUNaiveSimulation.h"
#include "simulation/CPUBarnesHutSimulation.h"

#include "universe/Universe.h"
#include "universe/SimpleUniverse.cpp"
// #include "universe/TwinCollisionUniverse.cpp"

#include "shaders/simulation.frag.cpp"
#include "shaders/simulation.vert.cpp"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720
#define MAX_FPS 60
#define FRAME_INTERVAL 1000 / MAX_FPS

GLuint program;
Simulation* simulation;
Universe* universe;

char** arg_start;
char** arg_end;

enum UniverseChoice { RANDOM_SPHERE, TWIN_COLLISION };
enum SimulationChoice { CPU_NAIVE, CPU_BARNES_HUT, GPU_NAIVE, GPU_BARNES_HUT };

static void check_errors(char* str) {
  std::cout << str << " ";

  int error = glGetError();

  switch(error) {
    case GL_INVALID_ENUM: std::cout<<"GL_INVALID_ENUM"<<std::endl;break;
    case GL_INVALID_VALUE: std::cout<<"GL_INVALID_VALUE"<<std::endl;break;
    case GL_INVALID_OPERATION: std::cout<<"GL_INVALID_OPERATION"<<std::endl;break;
    case GL_NO_ERROR: std::cout<<"GL_NO_ERROR"<<std::endl;break;
    case GL_STACK_OVERFLOW: std::cout<<"GL_STACK_OVERFLOW"<<std::endl;break;
    case GL_STACK_UNDERFLOW: std::cout<<"GL_STACK_UNDERFLOW"<<std::endl;break;
    case GL_OUT_OF_MEMORY: std::cout<<"GL_OUT_OF_MEMORY"<<std::endl;break;
    default: std::cout<<error<<std::endl;
  }
}

void print_usage() {
	std::cout << "Usage: gfx-coursework [-h] [--universe UNIVERSE] [--simulation SIMULATION]\n";
  std::cout << "Options: " << std::endl;
  std::cout << "\t" << "--universe" << "\t" << "twin (default), random" << std::endl;
  std::cout << "\t" << "--simulation" << "\t" << "cpu-naive, gpu-naive, cpu-barnes-hut, gpu-barnes-hut (default)" << std::endl;
}

char* cmd_option_value(const std::string & option) {
  char ** itr = std::find(arg_start, arg_end, option);

  return (itr != arg_end && ++itr != arg_end && *itr[0] != '-') ? *itr : 0;
}

bool cmd_option_exists(const std::string& option) {
  return std::find(arg_start, arg_end, option) != arg_end;
}

#define ESCAPE 

void keyboard(unsigned char key, int x, int y) {
  if (key == 27 || key == 'q') {
    exit(0);
  } else {
    simulation->handle_key(key);
  }
}

void cursor_keys(int key, int x, int y) {
  simulation->handle_specialkey(key);
}

void error_callback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
}

void get_options(int argc, char * argv[]) {
  arg_start = argv;
  arg_end = argv + argc;

  if (cmd_option_exists("-h")) {
    print_usage();
    exit(0);
  }

  UniverseChoice u_choice = TWIN_COLLISION;
  SimulationChoice s_choice = CPU_NAIVE;

  {
    char* u_selection;

    if (u_selection = cmd_option_value("--universe")) {
      if (std::strcmp(u_selection, "twin") == 0) {
        u_choice = TWIN_COLLISION;
      } else if (std::strcmp(u_selection, "random") == 0) {
        u_choice = RANDOM_SPHERE;
      } else {
        print_usage();
        exit(0);
      }
    }
  }

  {
    char* s_selection;

    if (s_selection = cmd_option_value("--simulation")) {
      if (std::strcmp(s_selection, "cpu-naive") == 0) {
        s_choice = CPU_NAIVE;
      } else if (std::strcmp(s_selection, "cpu-barnes-hut") == 0) {
        s_choice = CPU_BARNES_HUT;
      } if (std::strcmp(s_selection, "gpu-naive") == 0) {
        s_choice = GPU_NAIVE;
      } else if (std::strcmp(s_selection, "gpu-barnes-hut") == 0) {
        s_choice = GPU_BARNES_HUT;
      } else {
        print_usage();
        exit(0);
      }
    }
  }
}

void initiliase_world() {
  // TODO: configurability
  universe = (Universe*) (new SimpleUniverse());
  simulation = (Simulation*)(new CPUBarnesHutSimulation(universe));
}

void display() {
  GLint windowWidth, windowHeight;

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  simulation->render();

  glutSwapBuffers();
}

void idle() {}

void timer(int) {
  simulation->step();

  glutPostRedisplay();
  glutTimerFunc(FRAME_INTERVAL, timer, 0);
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLdouble) width / (GLdouble) height, 10.0, 80000000.0);
}

// GLuint create_shader(GLenum type, const char* source) {
//   GLuint shader = 0;
//   GLint status;

//   shader = glCreateShader(type);

//   glShaderSource(shader, 1, (const GLchar**) &source, NULL);
//   glCompileShader(shader);
//   glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

//   if (status != GL_TRUE) {
//     std::cerr << "Failed to load shader." << std::endl;
//     glDeleteShader(shader);
//     exit(1);
//   }

//   return shader;
// }

// GLenum create_program() {
//   GLuint vtx_shader_id = create_shader(GL_VERTEX_SHADER, _file_simulation_vert);
//   GLuint frag_shader_id = create_shader(GL_VERTEX_SHADER, _file_simulation_frag);

//   GLint status;

//   program = glCreateProgram();

//   glAttachShader(program, vtx_shader_id);
//   glAttachShader(program, frag_shader_id);

//   // bind attrib location and fragment data location???? what is this?

//   glLinkProgram(program);
//   glGetProgramiv(program, GL_LINK_STATUS, &status);

//   // delete shader points, doesn't actually delete them internally (I hope)
//   glDeleteShader(vtx_shader_id);
//   glDeleteShader(frag_shader_id);

//   if (status != GL_TRUE) {
//     std::cerr << "Failed to link program." << std::endl;
//     glDeleteProgram(program);
//     exit(1);
//   }
// }

void initialise_glut(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("GFX coursework");
  glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(cursor_keys);
  glutIdleFunc(idle);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
}

int main(int argc, char * argv[]) {
  get_options(argc, argv);
  initialise_glut(argc, argv);
  // create_program();
  initiliase_world();
  timer(0);
  glutMainLoop();

	return 0;
}

