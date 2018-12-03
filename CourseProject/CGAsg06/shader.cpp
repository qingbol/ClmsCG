#include <iostream> //std::cout
#include <fstream> //std::ifstream
#include <thread>  //std::thread
#include <cmath>  //math

// #include <GL/glew.h>
#ifdef __APPLE__
#include<OpenGL/gl3.h>
#include<OpenGL/gl3ext.h>
#else
// #include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include "includes/shader.h"

Shader::Shader(const char* vpath, const char* fpath) {
  GLint compiled, linked;
  GLchar log[512];

  //create vertex shader, load source code strings into shader
  svo = glCreateShader(GL_VERTEX_SHADER); 
  {
    std::ifstream source_file(vpath);
    std::string data;

    std::getline(source_file, data, '\0');
    const GLchar *vs(data.c_str());

    glShaderSource(svo, 1, (const GLchar**)&vs, NULL);
  }

  //create fragment shader, load source code strings into shader
  sfo = glCreateShader(GL_FRAGMENT_SHADER);
  {
    std::ifstream source_file(fpath);
    std::string data;

    std::getline(source_file, data, '\0');
    const GLchar *fs(data.c_str());

    glShaderSource(sfo, 1, (const GLchar**)&fs, NULL);
  }

  //compile vertex shader and print compiler log file
  glCompileShader(svo);
  glGetShaderiv(svo, GL_COMPILE_STATUS, &compiled);

  //chech compile status
  if(!compiled) {
    glGetShaderInfoLog(svo, 512, NULL, log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n";
    std::cout << log << std::endl;
  }

  //compile fragment shader and print compiler log file
  glCompileShader(sfo);
  glGetShaderiv(sfo, GL_COMPILE_STATUS, &compiled);

  //check compile status
  if(!compiled) {
    glGetShaderInfoLog(sfo, 512, NULL, log);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n";
    std::cout << log <<std::endl;
  }

  //create program
  spo = glCreateProgram();

  //create program object and attach the two compiled shaders
  glAttachShader(spo, svo);
  glAttachShader(spo, sfo);

  //link the program object and dprint linker log file
  glLinkProgram(spo);
  glGetProgramiv(spo, GL_LINK_STATUS, &linked);

  //check link status
  if(!linked) {
    glGetProgramInfoLog(spo, 512, NULL, log);
    std::cout << "ERROR:: SHADER::PROGRAM::LINK_FAILED\n";
  }

  //clean up - detach shader objects following attach to program object
  //this will flag them for deletion as soon as they are no longer
  //referenced, i.e.,when program object is deleted
  glDeleteShader(svo);
  glDeleteShader(sfo);
}

Shader::Shader(const char* vpath, const char* gpath, const char* fpath) {
  GLint compiled,linked;
  GLchar log[512];

  //create vertex shader, load source code strings into shader
  svo = glCreateShader(GL_VERTEX_SHADER);
  {
    std::ifstream source_file(vpath);
    std::string data;
    
    std::getline(source_file, data, '\0');
    const GLchar *vs(data.c_str());
    glShaderSource(svo, 1, (const GLchar**)&vs, NULL);
  }

  //create geometry shader, load source code strings into shader
  sgo = glCreateShader(GL_GEOMETRY_SHADER);
  {
    std::ifstream source_file(gpath);
    std::string data;

    std::getline(source_file, data, '\0');
    const GLchar* fs(data.c_str());
    glShaderSource(sgo, 1, (const GLchar**)&fs, NULL);
  }

  //create fragment shader, load source code strings into shader
  sfo = glCreateShader(GL_FRAGMENT_SHADER);
  {
    std::ifstream source_file(fpath);
    std::string data;
    
    std::getline(source_file, data, '\0');
    const GLchar* fs(data.c_str());
    glShaderSource(sfo, 1, (const GLchar**)&fs, NULL);
  }

  //compile vertex shader and print compiler log file
  glCompileShader(svo);
  glGetShaderiv(svo, GL_COMPILE_STATUS, &compiled);

  //check compile status
  if(!compiled) {
    glGetShaderInfoLog(svo, 512, NULL, log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n";
    std::cout << log << std::endl;
  }

  //compile geometry shader and print compiler log file
  glCompileShader(sgo);
  glGetShaderiv(sgo, GL_COMPILE_STATUS, &compiled);

  //check compile status
  if(!compiled) {
    glGetShaderInfoLog(sgo, 512, NULL, log);
    std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED'n";
    std::cout << log << std::endl;
  }

  //compile fragment shader and print compiler log file
  glCompileShader(sfo);
  glGetShaderiv(sfo, GL_COMPILE_STATUS, &compiled);

  //check compile status
  if(!compiled) {
    glGetShaderInfoLog(sfo, 512, NULL, log);
    std::cout << "ERROR::SHASER::FRAGMENT::COMPILATION_FAILED\n";
    std::cout << log << std::endl;
  }

  //create program
  spo = glCreateProgram();

  //create program object an attach the two compiled shaders
  glAttachShader(spo, svo);
  glAttachShader(spo, sgo);
  glAttachShader(spo, sfo);

  glLinkProgram(spo);
  glGetProgramiv(spo, GL_LINK_STATUS, &linked);

  //check link status
  if(!linked) {
    glGetProgramInfoLog(spo, 512, NULL, log);
    std::cout << "ERROR::SHADER::PROGRAM::LINkED_FAILED\n";
    std::cout << log << std::endl;
  }

  glDeleteShader(svo);
  glDeleteShader(sgo);
  glDeleteShader(sfo);

}
