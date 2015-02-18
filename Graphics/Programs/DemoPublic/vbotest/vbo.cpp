/*
   This is a variation of tutorial3 using VertexBufferObject's
   and GLSLProgram objects. Modified by jnc@ecs.soton.ac.uk

   origionally by consultit@katamail.com

Modified to use GLM, GLFW by jnc@ecs.soton.ac.uk
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VertexBufferObject.h"
#include "glslprogram.h"


struct Vertex {
    GLfloat position[4];
    GLfloat color[3];
};

struct Vertex tetrahedron[12] = {
    {{  0.25,  0.25,  0.25  }, {  1.0f,  0.0f,  0.0f  }},
    {{ -0.25, -0.25,  0.25  }, {  1.0f,  0.0f,  0.0f  }},
    {{ -0.25,  0.25, -0.25  }, {  1.0f,  0.0f,  0.0f  }},
    {{  0.25,  0.25,  0.25  }, {  0.0f,  1.0f,  0.0f  }},
    {{ -0.25, -0.25,  0.25  }, {  0.0f,  1.0f,  0.0f  }},
    {{  0.25, -0.25, -0.25  }, {  0.0f,  1.0f,  0.0f  }},
    {{  0.25,  0.25,  0.25  }, {  0.0f,  0.0f,  1.0f  }},
    {{ -0.25,  0.25, -0.25  }, {  0.0f,  0.0f,  1.0f  }},
    {{  0.25, -0.25, -0.25  }, {  0.0f,  0.0f,  1.0f  }},
    {{ -0.25, -0.25,  0.25  }, {  1.0f,  1.0f,  1.0f  }},
    {{ -0.25,  0.25, -0.25  }, {  1.0f,  1.0f,  1.0f  }},
    {{  0.25, -0.25, -0.25  }, {  1.0f,  1.0f,  1.0f  }}
};

void Render(int i, GLSLProgram O, VertexBufferObject vb) {
    GLfloat angle;
    glm::mat4 Projection = glm::mat4(1.);
    angle = (GLfloat) (i % 360);
    glm::mat4 View = glm::mat4(1.);
    View = glm::rotate(View, angle, glm::vec3(1.f, 1.f, 1.f));
    View = glm::rotate(View, angle, glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 Model = glm::mat4(1.0);
    glm::mat4 MVP = Projection * View * Model;
    Check("Inside render loop 1");
    vb.SelectVAO();
    O.SetUniform("uMVPmatrix", MVP);
    vb.DeSelectVAO();
    Check("Inside render loop 2");
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vb.Draw();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


int main( void ) {
    int k = 0;
    GLFWwindow* window;
    if( !glfwInit() ) {
        printf("Failed to start GLFW\n");
        exit( EXIT_FAILURE );
    }
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        printf("GLFW Failed to start\n");
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    int err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stderr, "Glew done\n");
    glfwSetKeyCallback(window, key_callback);
    fprintf(stderr, "GL INFO %s\n", glGetString(GL_VERSION));
    VertexBufferObject VB;
    VB.vboName = "VB";
    VB.SetVerbose(true);
    VB.CollapseCommonVertices( false );
    VB.SetTol( .001f );
    VB.UseBufferObjects(true);
    VB.glBegin( GL_TRIANGLES );
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 3; j++) {
            int k = i * 3 + j;
            VB.glVertex3fv(tetrahedron[k].position);
            VB.glColor3fv(tetrahedron[k].color);
        }
    }
    VB.glEnd();
    VB.Print();
    GLSLProgram O;
    O.SetVerbose(true);
    O.SetGstap(false);
    bool good = O.Create("vbo/vbo.vert", "vbo/vbo.frag");
    if( !good ) {
        fprintf( stderr, "GLSL Program wasn�t created.\n" );
        exit(0);
    }
    VB.SelectVAO();
    O.SetAttribute( "aPosition", VB, VERTEX_ATTRIBUTE );
    O.SetAttribute( "aColor", VB, COLOR_ATTRIBUTE );
    // or NORMAL_ATTRIBUTE or TEXTURE_ATTRIBUTE or ...
    VB.DeSelectVAO();
    glEnable(GL_DEPTH_TEST);
    Check("Before render loop");
    while(!glfwWindowShouldClose(window)) {
        Render(k, O, VB);
        k++;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    exit( EXIT_SUCCESS );
}

