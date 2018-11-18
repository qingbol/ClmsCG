#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <cmath>
#include <numeric>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp> //GLM core
#include <glm/gtx/string_cast.hpp>

#include "obj_loader.h"

///////////////////  friends //////////////////////////////
std::istream& operator>>(std::istream& s, ObjLoader& rhs){
  char c, v;
  unsigned int i, t, n;
  glm::vec3 v_coord, vn_coord;
  glm::vec2 vt_coord;

 // std::cout << "we are entering >>" << std::endl;
  while (s.get(c) && !s.eof()){
    //std::cerr << "c: " << c << std::endl;
    switch (c) {
      case '#':
        do {s.get(c);} while (c != '\n');
        break;  
      case 'v':
        //loop to read in vertex info
        v = s.peek();
        switch (v) {
          case 't':
            s.get(v); //consume 't'
            s >> vt_coord.x >> vt_coord.y >> std::ws;
            rhs.vt_coords_.push_back(vt_coord);
            break;
          case 'n':
            s.get(v); //consume 'v'
            s >> vn_coord.x >> vn_coord.y >> vn_coord.z >> std::ws;
            rhs.vn_coords_.push_back(vn_coord);
            break;
          default:
            s >> v_coord.x >> v_coord.y >> v_coord.z >> std::ws;
            //std::cerr << glm::to_string(v_coord) << std::endl;
            rhs.v_coords_.push_back(v_coord);
            break;
        }
        break;
      case 'g':
        s >> rhs.group_ >> std::ws;
        break;
      case 'f':
        //loop to read face info
        //assuming leading f character has already been consumed
        while ((c = s.peek()) != '\n'){
          //std::cerr << c;
          if(isdigit(c)) {
            s >> i; //read v info
            rhs.v_indices_.push_back(i-1);
            //check for possible formats:
            if ((c = s.peek()) == '/') {
              s.get(c); //consume '/'
              //check next character, see if it is a digit
              if ((c = s.peek()) && isdigit(c)) {
                s >> t; //read vt info
                rhs.vt_indices_.push_back(t-1);
                if ((c = s.peek()) == '/') {
                  s.get(c); //consume '/'
                  if ((c = s.peek()) && isdigit(c)) {
                    s >> n; //read vn info
                    rhs.vn_indices_.push_back(t-1);
                  }
                }
              } else if ((c = s.peek()) == '/') {
                s.get(c);
                s >> n; //read vn info
                rhs.vn_indices_.push_back(t-1);
              }
            }
          } else
            s.get(c); //consume space
        }
        s.get(c); //consume trailing '\n'
        break;
      case '\n':
        break;
      default:
        do {s.get(c); std::cerr << c;} while (c != '\n');
        break;
    }
  }

  std::cerr << "number of v_coords_ read in:" << rhs.v_coords_.size() << std::endl;
  std::cerr << "number of vt_coords_ read in:" << rhs.vt_coords_.size() << std::endl;
  std::cerr << "number of vn_coords_ read in:" << rhs.vn_coords_.size() << std::endl;
  std::cerr << "number of v_indices_ read in:" << rhs.v_indices_.size() << std::endl;
  std::cerr << "number of vt_indices_ read in:" << rhs.vt_indices_.size() << std::endl;
  std::cerr << "number of vn_indices_ read in:" << rhs.vn_indices_.size() << std::endl;

  //allocat space for list of vertex to facebackpoints
  //each fi holds a vector to which we'll add face indices incident on vertex.
   rhs.face_indices_.resize(rhs.v_coords_.size());
   rhs.CalcNormals();

//  std::cout<<"=======in obj_class vertices coordinates================"<<std::endl;
//  std::cout<<"=======vertices size is  "<<rhs.v_coords_.size()<<"=========="<<std::endl;
//  //Using the iterator to output the value 
//   	for(std::vector<glm::vec3>::iterator m = rhs.v_coords_.begin(); m != rhs.v_coords_.end(); m++ )
//  {
//   std::cout<<m->x<<"||"<<m->y<<"||"<<m->z<<std::endl;
//  }
//  std::cout<<"=======texture coordinates================"<<std::endl;
// for(std::vector<glm::vec2>::iterator m = rhs.vt_coords_.begin(); m != rhs.vt_coords_.end(); m++ )
//
//  {
//   std::cout<<m->x<<"||"<<m->y<<"||"<<std::endl;
//  }

  return s;
}

//85 rows miss
std::ostream& operator<<(std::ostream& s, const ObjLoader& rhs) {
  s.setf(std::ios::fixed, std::ios::floatfield);
  s.precision(1);

  for(int i = 0; i < rhs.v_coords_.size(); i++) {
    s << "v" << " ";
    s << rhs.v_coords_[i].x << " ";
    s << rhs.v_coords_[i].y << " ";
    s << rhs.v_coords_[i].z << std::endl;
  }

  for (int i = 0; i<rhs.vt_coords_.size(); i++) {
    s << "vt" << " ";
    s << rhs.vt_coords_[i].x << " ";
    s << rhs.vt_coords_[i].y << std::endl;
  }

  for (int i=0; i < rhs.vn_coords_.size(); i++) {
    s << "vn" << " ";
    s << rhs.vn_coords_[i].x << " ";
    s << rhs.vn_coords_[i].y << " ";
    s << rhs.vn_coords_[i].z << std::endl;
  }

  s << "g" << rhs.group_ << std::endl;

  for (int i = 0; i < rhs.v_indices_.size(); i += 3) {
    s << "f" << " ";
    //plus 1 because AW objects are indexed form 1 to n and c++ wants 0
    for(int j = 0; j < 3; j++)
      s << rhs.v_indices_[i + j] + 1 << " ";
    s << std::endl;
  }
  return s;
}

void ObjLoader::Pack(std::vector<GLfloat>& vpack) {
  for(int i=0; i < (size_t)v_indices_.size(); i++) {
    vpack.push_back(v_coords_[v_indices_[i]].x);
    vpack.push_back(v_coords_[v_indices_[i]].y);
    vpack.push_back(v_coords_[v_indices_[i]].z);

    if (!vt_indices_.empty()) {
      vpack.push_back(vt_coords_[vt_indices_[i]].x);
      vpack.push_back(vt_coords_[vt_indices_[i]].y);
    }

    //note: we expect in the end as many vertex normals as vertices
    //we need to index into vn list using the current vertex index
    if(!vn_indices_.empty()) {
      vpack.push_back(vn_coords_[vn_indices_[v_indices_[i]]].x);
      vpack.push_back(vn_coords_[vn_indices_[v_indices_[i]]].y);
      vpack.push_back(vn_coords_[vn_indices_[v_indices_[i]]].z);
    }
  }
}

void ObjLoader::CalcNormals(){
  glm::vec3 p0, p1, p2, v1, v2;
  glm::vec3 fn;

  std::cerr << "v_indices_.size() = " << v_indices_.size() << std::endl;
  std::cerr << "v_indices_.size() / 3 = " << v_indices_.size() / 3 << std::endl;

  //assume each face is a triangle(hence iterate by 3)
  std::cerr << "==========Face normal=================" << std::endl;
  for (int f = 0; f < (size_t)v_indices_.size() / 3; f++) {
    std::cerr << "f " << v_indices_[3 * f + 0] + 1 << " ";
    std::cerr <<         v_indices_[3 * f + 1] + 1 << " ";
    std::cerr <<         v_indices_[3 * f + 2] + 1 << " ";

    //for each vertex, add in this face index
    face_indices_[v_indices_[3 * f + 0]].push_back(f);
    face_indices_[v_indices_[3 * f + 1]].push_back(f);
    face_indices_[v_indices_[3 * f + 2]].push_back(f);

    p0 = v_coords_[v_indices_[3 * f + 0]];
    p1 = v_coords_[v_indices_[3 * f + 1]];
    p2 = v_coords_[v_indices_[3 * f + 2]];

    v1 = p1 - p0;
    v2 = p2 - p0;

    //face normals
    fn = normalize(cross(v1, v2));
    // fn = -normalize(cross(v2, v1));

    std::cerr << glm::to_string(fn) << std::endl;
    face_normals_.push_back(fn);
  }
  //face_indices_[v] is the list of face indices incident on vertex v
  //debug: print out all face_indices incident on each vertex
  std::cerr << "========face indices incident on vertex========" << std::endl;
  for (int v = 0; v < (size_t)face_indices_.size(); v++) {
    for (int f = 0; f < (size_t)face_indices_[v].size(); f++) {
      std::cerr << face_indices_[v][f] + 1 << " ";
    }
    std::cerr << std::endl;
  }

  //compute vertex normals as average of face normals incident on vertex
  unsigned int n = 0; //index for vn in .obj format
  glm::vec3 vfn; //the vertex face normal
  if (vn_indices_.empty()) {
    vn_indices_.resize(v_indices_.size());
    for (int v =0; v < (size_t)face_indices_.size(); v++) {
      vfn = glm::vec3(0.0f, 0.0f, 0.0f);
      for (int f = 0; f < (size_t)face_indices_[v].size(); f++) {
        //use flag_fn to identify if this vector already exist.
        int flag_fn = 1;
        for (int g = 0; g < f; g++) {
          float dot_result = face_normals_[face_indices_[v][f]].x * face_normals_[face_indices_[v][g]].x \
                             + face_normals_[face_indices_[v][f]].y * face_normals_[face_indices_[v][g]].y \
                             + face_normals_[face_indices_[v][f]].z * face_normals_[face_indices_[v][g]].z; 
          float modulus_vf = face_normals_[face_indices_[v][f]].x * face_normals_[face_indices_[v][f]].x \
                             + face_normals_[face_indices_[v][f]].y * face_normals_[face_indices_[v][f]].y \
                             + face_normals_[face_indices_[v][f]].z * face_normals_[face_indices_[v][f]].z;
          float modulus_vg = face_normals_[face_indices_[v][g]].x * face_normals_[face_indices_[v][g]].x \
                             + face_normals_[face_indices_[v][g]].y * face_normals_[face_indices_[v][g]].y \
                             + face_normals_[face_indices_[v][g]].z * face_normals_[face_indices_[v][g]].z;
          float modulus_fg = sqrt(modulus_vf * modulus_vg);
          if (dot_result == modulus_fg) {
            flag_fn = 0;
            break;
          }
          // float dot_result = std::inner_product(face_normals_[face_indices_[v][f]], face_normals_[face_indices_[v][g]]); 
          // std::cerr << "dot_result is " << face_normals_[face_indices_[v][f]].x << std::endl;
          // std::cerr << "dot_result is " << dot_result << std::endl;
          // std::cerr << "dot_result is " << modulus_fg << std::endl;
        }
        std::cerr << "dot product result is " << flag_fn << std::endl;
        if (flag_fn == 1) {
          vfn += (float)(1.0 / (float)face_indices_[v].size()) * face_normals_[face_indices_[v][f]];
        // vfn += face_normals_[face_indices_[v][f]] ; /// (float)face_indices_[v].size();
        }
        std::cerr << v <<"row " << f << "column" << "value is " << glm::to_string(vfn) << "size is " << face_indices_[v].size() << std::endl;
      }
      // vfn = normalize(vfn);
      vfn = -normalize(vfn);
      vn_coords_.push_back(vfn);
      vn_indices_[n] = n;
      n++;
    }
  }

  //debug: print out vertex normals
  std::cerr << "========vertex normals========" << std::endl;
  for (int v = 0; v < vn_coords_.size(); v++){
    std::cerr << glm::to_string(vn_coords_[v]) << std::endl;
  }

}









