//
//  main.cpp
//  SimpleShaderCpp
//
//  Created by Emmanuel Piuze on 13-03-21.
//  Copyright (c) 2013 Emmanuel Piuze. All rights reserved.
//

#include <iostream>
#include "shader_prog.h"

const GLchar *vertex_shader[] = {
    "void main(void) {\n",
    "    gl_Position = ftransform();\n",
    "    gl_FrontColor = gl_Color;\n",
    "}"
};

const GLchar *color_shader[] = {
    "void main() {\n",
    "    gl_FragColor.r = 1.0 - gl_Color.r; \n",
    "    gl_FragColor.g = 1.0 - gl_Color.g; \n",
    "    gl_FragColor.b = 1.0 - gl_Color.b; \n",
    "}"
};

void init(void);
void display();
void changeSize(int w, int h);
void processNormalKeys(unsigned char key, int x, int y);

int main(int argc, char * argv[])
{
    //Initialise GLUT with command-line parameters.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Test OpenGL App with GLUT");
    
    //Call init (initialise GLUT
    init();
    
    //Call "display" function
    glutDisplayFunc(display);
    
    glutReshapeFunc(changeSize);
    
    glutKeyboardFunc(processNormalKeys);
    
    //Enter the GLUT event loop
    glutMainLoop();
    
    return 0;
}

void processNormalKeys(unsigned char key, int x, int y) {
    
    if (key == 27)
        exit(0);
}

void display() {
    // compile and link the specified shaders:
    static shader_prog prog(vertex_shader, color_shader);
    
    // Use the compiled shaders:
    prog();
    
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw something:
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3d(0.0, -1.0, -1.0);
    glEnd();
    
//    glFlush();
    glutSwapBuffers();
}

void changeSize(int w, int h) {
    
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;
    
    float ratio = 1.0* w / h;
    
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    
    // Set the correct perspective.
    gluPerspective(45,ratio,1,1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0,0.0,5.0,
              0.0,0.0,-1.0,
              0.0f,1.0f,0.0f);
}

void init(void)
{
    //select clearing (background) color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    //initialize viewing values
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    
}