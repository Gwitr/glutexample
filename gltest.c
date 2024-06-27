#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "quad.h"
#include "texture.h"
#include "program.h"

char *read_whole_file(const char *path)
{
  void *contents = NULL;
  FILE *f = fopen(path, "rb");
  if (!f)
    goto err;
  fseek(f, 0, SEEK_END);
  ssize_t filesize = ftell(f);
  fseek(f, 0, SEEK_SET);
  contents = calloc(1, filesize + 1);
  if (!contents)
    goto err;
  if (!fread(contents, 1, filesize, f))
    goto err;
  fclose(f);
  return contents;
  err:
  if (f) fclose(f);
  free(contents);
  return NULL;
}

struct texture *quadTex;
struct program *quadProgram;
int frame;

void init(void)
{
  quad_init();

  char *vertexShaderSource = read_whole_file("program.vert");
  char *fragmentShaderSource = read_whole_file("program.frag");

  quadProgram = program_new(2, GL_VERTEX_SHADER, vertexShaderSource, GL_FRAGMENT_SHADER, fragmentShaderSource);
  if (program_define_uniforms(quadProgram,
    GL_INT, 1,   "tex",
    GL_FLOAT, 2, "position",
    NO_MORE_UNIFORMS
  ))
    abort();
  
  free(vertexShaderSource);
  free(fragmentShaderSource);

  if (!(quadTex = texture_new("test.png", GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT)))
    abort();

  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  frame = 0;
}

void deinit(void)
{
  quad_deinit();
  program_delete(quadProgram);
  texture_delete(quadTex);
}

void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  program_use(quadProgram);
  texture_set_active(0);
  texture_bind(quadTex);
  program_set_uniform(quadProgram, "tex", (GLint)0);
  program_set_uniform(quadProgram, "position", (GLfloat)sin((double)frame / 60 * M_PI), (GLfloat)cos((double)frame / 60 * M_PI));
  quad_draw();
  glutSwapBuffers();
}

void timer(int value)
{
  (void)value;
  glutTimerFunc(16, timer, -1);
  glutPostRedisplay();
  ++frame;
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(640, 480);
  glutCreateWindow("Test window");

  glewInit();
  init();

  glutDisplayFunc(draw);
  glutTimerFunc(16, timer, -1);
  glutMainLoop();

  deinit();
  return 0;
}
