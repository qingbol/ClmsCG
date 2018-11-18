#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <fstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
// #include <stdio.h>
// #include <cstring>

class ObjLoader{
 public:
  ObjLoader() : rotx_(0.0), roty_(0.0), rotz_(0.0) {}
  ObjLoader(const char* file)
      : rotx_(0.0), roty_(0.0), rotz_(0.0) {
    std::ifstream ifs;
    /* std::cout << "entering Class"<< std::endl; */
    ifs.open(file, std::ios::in);
    if(ifs.good()) {
      /* std::cout << "ifs entering"<< std::endl; */
      ifs >> this;
    }
    ifs.close();

    //initialize object center oc
    oc_ = glm::vec3(0.0f, 0.0f, 0.0f);
    //computer center of mass
    for (int i = 0; i < (int)v_coords_.size(); i++) {
        for (int j = 0; j < (int)3; j++) {
          oc_[j] = 1 / (float)v_coords_.size() * v_coords_[i][j];
        }
    }
  }

  void Pack(std::vector<GLfloat>& vpack);
  void CalcNormals();

  //friends
  friend std::istream& operator>>(std::istream& s, ObjLoader& rhs);
  friend std::istream& operator>>(std::istream& s, ObjLoader* rhs){
    return (s >> (*rhs));
  }
  friend std::ostream& operator << (std::ostream& s, const ObjLoader& rhs);
  friend std::ostream& operator << (std::ostream& s, ObjLoader* rhs){
    return s << *rhs;
  }

  //a bit of hack...
  friend int main(int argc, char** argv);
  friend void PrintInfo(ObjLoader objLoader, std::vector<GLfloat> vpack);
  
 private:
  std::vector<glm::vec3> v_coords_;
  std::vector<glm::vec2> vt_coords_;
  std::vector<glm::vec3> vn_coords_;
  //indices into verts list
  std::vector<unsigned int> v_indices_;
  //indices into txcds list
  std::vector<unsigned int> vt_indices_;
  //indices into normals list
  std::vector<unsigned int> vn_indices_;
  std::string group_;
  glm::vec3 oc_;
  std::vector<glm::vec3> face_normals_; //face normals
  std::vector<std::vector<unsigned int> > face_indices_;

  float rotx_;
  float roty_;
  float rotz_;
  };
#endif
