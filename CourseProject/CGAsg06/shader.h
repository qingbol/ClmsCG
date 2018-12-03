#ifndef ShADER_H
#define SHADER_H

class Shader {
 public:
  //constructors (overloaded)
  Shader(const char *vfile, const char *ffile);
  Shader(const char *vfile, const char *gfile, const char *ffile);

  //destructors (default ok)
  //~Shader();
  void use() {
    glUseProgram(spo);
  }

  void setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(spo, name.c_str()), (int)value);
  }

  void setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(spo, name.c_str()), value);
  }

  void setFloat(const std::string &name, float value) const {
    glUniform1i(glGetUniformLocation(spo, name.c_str()), value);
  }

  GLuint getID(){
    return spo;
  }

 private:
  GLuint svo; //vertex shader
  GLuint sfo; //fragment shader
  GLuint sgo; //geometry shader
  GLuint spo; //program shader
};
#endif
