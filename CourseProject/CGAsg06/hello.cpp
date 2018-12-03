// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#ifdef __APPLE__
#include<OpenGL/gl3.h>
#include<OpenGL/gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "includes/shader.h"
#include "includes/texture.h"
#include "includes/obj_loader.h"

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

void PrintInfo(ObjLoader objLoader, std::vector<GLfloat> vpack);

int main(int argc, char** argv) {
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

    GLFWwindow* window;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "CG asignment - Qingbo Lai", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
	// Dark blue background
	// glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClearColor(0.0f, 0.3f, 0.48f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Create and compile our GLSL program from the shaders
    Shader defaultShader("passthru.vert", "passthru.frag");
    Shader normalShader("normals.vert", "normals.geom", "normals.frag");
    Shader lampShader("phong.vert", "phong.frag");

	// Load the texture
	GLuint Texture = loadBMP_custom("mandrill.bmp");

	// Read  .obj file
	ObjLoader objLoader("pyramid.obj");
	//ObjLoader objLoader("nanosuit.obj");
    std::vector<GLfloat> vpack;
    objLoader.Pack(vpack);

    std::vector<GLfloat> fn_vpack;
    objLoader.FaceNormalPack(fn_vpack);

    //compute the stride
    int istride = 3;
    if (!objLoader.vt_indices_.empty()) {
      istride += 2;
    }
    if (!objLoader.vn_indices_.empty()) {
      istride += 3;
    }
    std::cerr << "istride is " << istride <<std::endl;
   // GLsize istrides = istride * sizeof(GLfloat);
    int istrides = istride * sizeof(GLfloat);

    // Print some Info;
	/* PrintInfo(objLoader, vpack); */
	PrintInfo(objLoader, fn_vpack);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	// Load it into a VBO
	/* GLuint vertexbuffer; */
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	/* glBufferData(GL_ARRAY_BUFFER, vpack.size() * sizeof(GLfloat), &vpack[0], GL_STATIC_DRAW); */
	glBufferData(GL_ARRAY_BUFFER, fn_vpack.size() * sizeof(GLfloat), &fn_vpack[0], GL_STATIC_DRAW);
    
	GLuint vnVBO;
	glGenBuffers(1, &vnVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vnVBO);
	glBufferData(GL_ARRAY_BUFFER, vpack.size() * sizeof(GLfloat), &vpack[0], GL_STATIC_DRAW);
    
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//0 rst attribute buffer : vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istrides, (void*)0);
	glEnableVertexAttribArray(0);
	// 1st attribute buffer : UVs
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istrides, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 2nd attribute buffer : normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istrides, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
    
	glBindBuffer(GL_ARRAY_BUFFER, vnVBO);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, istrides, (void*)0);
	glEnableVertexAttribArray(4);
	// 2nd attribute buffer : normal
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, istrides, (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(5);

    unsigned int lampVAO;
    glGenVertexArrays (1, &lampVAO);
    glBindVertexArray (lampVAO);
    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, istrides, (void*)0);
    glEnableVertexAttribArray(3);

    // create transformations
    glm::mat4 model;
    glm::mat4 projection;
    glm::mat4 MVP;
    glm::mat4 m, t1, t2, t3, p;
    glm::mat4 view;

    GLuint programID_default;
    GLuint MatrixID_default; 
    GLuint programID_normal;
    GLuint MatrixID_normal; 
    GLuint programID_lamp;
    GLuint MatrixID_lamp; 

    float tc[]={
        0.0f, 0.0f, 0.0f
    };
    
    // lighting
    glm::vec3 lampPos(2.0f, 0.5f, 0.3f);
    glm::vec3 lampColor(1.0f, 1.0f, 1.0f);
    glm::vec3 viewPos(1.0f, 0.0f, 1.5f);

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tc[0]=cos((float)glfwGetTime()/2.0);
        tc[1]=sin((float)glfwGetTime()/2.0);

	    // create transformations
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        m=glm::mat4(1.0f);
        m=glm::rotate(m,(float)glfwGetTime()*2,glm::vec3(0.5f, 1.0f, 0.0f));      
        t1=glm::translate(glm::mat4(1.0f),glm::vec3(-objLoader.oc_[0],-objLoader.oc_[1], -objLoader.oc_[2]));
        t2=glm::translate(glm::mat4(1.0f),glm::vec3(objLoader.oc_[0], objLoader.oc_[1], objLoader.oc_[2]));
        t3=glm::translate(glm::mat4(1.0f),glm::vec3(tc[0], tc[1],  tc[2]));
        model=t3*(t2*m*t1);
        // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
		MVP = projection * view * model;

		// Use our shader
        defaultShader.use();
	    // Get a handle for our "MVP" uniform
        programID_default = defaultShader.getID();
	    /* MatrixID_default = glGetUniformLocation(programID_default, "MVP"); */
		// Send our transformation to the currently bound shader, // in the "MVP" uniform
		/* glUniformMatrix4fv(MatrixID_default, 1, GL_FALSE, &MVP[0][0]); */
		glUniformMatrix4fv(glGetUniformLocation(programID_default, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID_default, "model"), 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID_default, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID_default, "projection"), 1, GL_FALSE, &projection[0][0]);
		glUniform3fv(glGetUniformLocation(programID_default, "lampPos"), 1, &lampPos[0]);
		glUniform3fv(glGetUniformLocation(programID_default, "lampColor"), 1, &lampColor[0]);
		glUniform3fv(glGetUniformLocation(programID_default, "viewPos"), 1, &viewPos[0]);
        glBindVertexArray(VAO);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vpack.size() / 8 );

		// Use our shader
        normalShader.use();
	    // Get a handle for our "MVP" uniform
        programID_normal = normalShader.getID();
	    MatrixID_normal = glGetUniformLocation(programID_normal, "MVP");
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID_normal, 1, GL_FALSE, &MVP[0][0]);
        glBindVertexArray(VAO);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vpack.size()/8);

        //draw the lamp object
        lampShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lampPos);
        model = glm::scale(model, glm::vec3(0.3f));
		MVP = projection * view * model;
        programID_lamp = lampShader.getID();
	    MatrixID_lamp = glGetUniformLocation(programID_lamp, "MVP");
		glUniformMatrix4fv(MatrixID_normal, 1, GL_FALSE, &MVP[0][0]);
        glBindVertexArray(lampVAO);
		glDrawArrays(GL_TRIANGLES, 0, vpack.size()/8);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
		
	// Cleanup VBO and shader
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(programID_default);
	glDeleteProgram(programID_normal);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VAO);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void PrintInfo(ObjLoader objLoader, std::vector<GLfloat> vpack) {
    std::cout<<"=======in main vertices coordinates================"<<std::endl;
    std::cout<<"=======vertices size is  "<<objLoader.v_coords_.size()<<"=========="<<std::endl;
    //Using the iterator to output the value 
	for(std::vector<glm::vec3>::iterator m = objLoader.v_coords_.begin(); m != objLoader.v_coords_.end(); m++ )
     {
      std::cout<<m->x<<"||"<<m->y<<"||"<<m->z<<std::endl;
     }
     std::cout<<"=======texture coordinates size is " << objLoader.vt_coords_.size() << " ================"<<std::endl;
    for(std::vector<glm::vec2>::iterator m = objLoader.vt_coords_.begin(); m != objLoader.vt_coords_.end(); m++ )
     {
      std::cout<<m->x<<"||"<<m->y<<"||"<<std::endl;
     }
     std::cout<<"=======vpack coordinates size is " << vpack.size() << "=========="<<std::endl;
     for (GLfloat m : vpack) {
       std::cout << m << "||";
     }
    std::cout << std::endl;
    std::cout << vpack.size() << std::endl;
    std::cout<<"=======Center of mass================"<<std::endl;
    std::cout << objLoader.oc_[0] << "||" << objLoader.oc_[1] << "||"  << objLoader.oc_[2] << std::endl;
}
