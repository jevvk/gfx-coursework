#include <glad/glad.h>
#include <GL/glut.h>

#include "Config.h"
#include "Particle.h"
#include "Constants.h"

#include "simulation/Simulation.h"
#include "simulation/CPUNaiveSimulation.h"
#include "simulation/CPUBarnesHutSimulation.h"
#include "simulation/GPUBarnesHutSimulation.h"

#include "universe/Universe.h"
#include "universe/SimpleUniverse.cpp"
// #include "universe/TwinCollisionUniverse.cpp"

Simulation* simulation;
Universe* universe;

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

void initiliase_world() {
  if (universe_choice == RANDOM_SPHERE) {
    universe = (Universe*) (new SimpleUniverse());
  } else {
    exit(2);
  }

  if (simulation_choice == CPU_NAIVE) {
    simulation = (Simulation*)(new CPUNaiveSimulation(universe));
  } else if (simulation_choice == CPU_BARNES_HUT) {
    simulation = (Simulation*)(new CPUBarnesHutSimulation(universe));
  } else if (simulation_choice == GPU_BARNES_HUT) {
    simulation = (Simulation*)(new GPUBarnesHutSimulation(universe));
  } else {
    exit(2);
  }
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
  if (width == SCREEN_WIDTH && height == SCREEN_HEIGHT) {
    return;
  }

  glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void initialise_gl(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("GFX coursework");
  glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

  if (!gladLoadGL()) {
    std::cerr << "Failed to load OpenGL context." << std::endl;
    exit(1);
  }

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(cursor_keys);
  glutIdleFunc(idle);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_POINT_SIZE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_POINT_SPRITE);
  glEnable(GL_PROGRAM_POINT_SIZE);

  Particle::init();
}

void exit_gl() {
  Particle::exit();
}

int main(int argc, char * argv[]) {
  get_options(argc, argv);
  initialise_gl(argc, argv);
  initiliase_world();

  timer(0);
  glutMainLoop();

  exit_gl();

	return 0;
}

