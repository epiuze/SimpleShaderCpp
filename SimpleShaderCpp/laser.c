//
//  RayTracer.c
//  SimpleRayTracer
//
//  Created by Emmanuel Piuze-Phaneuf on 2013-03-23.
//  Copyright (c) 2013 Emmanuel Piuze-Phaneuf. All rights reserved.
//

#include <math.h>
#include "ShaderUtil.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define bool int
#define false 0
#define true 1

GLuint width = 800;
GLuint height= 400;

GLfloat curxy[2];

float angle = 0.0, axis[3], trans[3];
bool trackingMouse = false;
bool redrawContinue = false;
bool trackballMove = false;

GLuint shaderProgram;

// uniforms
GLuint uniform_time;
GLuint uniform_mouse;
GLuint uniform_resolution;

void drawFullscreenQuad() {
    glBegin(GL_QUADS);
    
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(1, 0);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(0, 1);
    glVertex2f(0, 1);
    
    glEnd();
    
}

void drawTestTriangle() {
    // Draw a triangle
    glPushMatrix();
    glScaled(0.25, 0.25, 0.25);
    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 0.0, 1.0);  /* blue */
    glVertex2d(-1, -1);
    glColor3f(0.0, 1.0, 0.0);  /* green */
    glVertex2d(0, 1);
    glColor3f(1.0, 0.0, 0.0);  /* red */
    glVertex2d(1, -1);
    glEnd();
    glPopMatrix();
    
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glVertex3d(0, 0, 0);
    glEnd();
    
}

/*
 ** These functions implement a simple trackball-like motion control.
 */

float lastPos[3] = {0.0F, 0.0F, 0.0F};
int startX, startY;
bool rightMouseDown = false;

void trackball_ptov(int x, int y, int width, int height, float v[3]) {
    float d, a;
    
    /* project x,y onto a hemi-sphere centered within width, height */
    v[0] = (2.0F*x - width) / width;
    v[1] = (height - 2.0F*y) / height;
    d = (float) sqrt(v[0]*v[0] + v[1]*v[1]);
    v[2] = (float) cos((M_PI/2.0F) * ((d < 1.0F) ? d : 1.0F));
    a = 1.0F / (float) sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}

void mouseMotionPassive(int x, int y) {
    curxy[0] = x;
    curxy[1] = y;
    
    printf("(%.1f, %.1f)\n", curxy[0], curxy[1]);
}

void mouseMotion(int x, int y) {
    float curPos[3], dx, dy, dz;
    
//    printf("(%i, %i)\n", x, y);
//    curxy[0] = x;
//    curxy[1] = y;
    
    if(trackingMouse)
    {
        trackball_ptov(x, y, width, height, curPos);
        dx = curPos[0] - lastPos[0];
        dy = curPos[1] - lastPos[1];
        dz = curPos[2] - lastPos[2];
        
        if (dx || dy || dz) {
            angle = 90.0F * sqrt(dx*dx + dy*dy + dz*dz);
            
            axis[0] = lastPos[1]*curPos[2] - lastPos[2]*curPos[1];
            axis[1] = lastPos[2]*curPos[0] - lastPos[0]*curPos[2];
            axis[2] = lastPos[0]*curPos[1] - lastPos[1]*curPos[0];
        }
    }
    glutPostRedisplay();
}

void startMotion(int x, int y) {
    trackingMouse = true;
    redrawContinue = false;
    startX = x; startY = y;
    curxy[0] = x;
    curxy[1] = y;
    
    trackball_ptov(x, y, width, height, lastPos);
	trackballMove=true;
    
    float curPos[3];
    trackball_ptov(x, y, width, height, curPos);
    lastPos[0] = curPos[0];
    lastPos[1] = curPos[1];
    lastPos[2] = curPos[2];
    
}

void stopMotion(int x, int y) {
    trackingMouse = false;
    
    if (startX != x || startY != y) {
        redrawContinue = true;
    } else {
        angle = 0.0F;
        redrawContinue = false;
        trackballMove = false;
    }
}

void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON) switch(state)
    {
        case GLUT_DOWN:
            rightMouseDown = true;
            break;
        case GLUT_UP:
            rightMouseDown = false;
            break;
    }
    if (button == GLUT_MIDDLE_BUTTON) trackballMove = false;
    if (button == GLUT_LEFT_BUTTON) switch(state)
    {
        case GLUT_DOWN:
            y=height-y;
            startMotion( x,y);
            break;
        case GLUT_UP:
            stopMotion( x,y);
            break;
    }
}

GLfloat time = 0;

void draw() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

//    drawTestTriangle();
    drawFullscreenQuad();
    glUniform1f(uniform_time, time);
    
//    glUniform2iv(uniform_mouse, 1, (const GLint*) curxy);
//    glUniform2iv(uniform_resolution, 1, (const GLint*) resolution);
    glUniform2f(uniform_mouse, curxy[0] / width, 1 - curxy[1] / height);
//    glUniform2f(uniform_resolution, width, height);
    glUniform2f(uniform_resolution, (float) width, (float) height);
    
    time += 0.01f;
    
    // Done!
    glutSwapBuffers();
    
    glutPostRedisplay();

}

void init() {
    glClearColor (0.0,0.0,0.0,1.0);
    
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    
//    shaderProgram = initShader("./passThrough.vp", "./laser.fp");
//    shaderProgram = initShader("./passThrough.vp", "./circulars.fp");
    shaderProgram = initShader("./passThrough.vp", "./texture.fp");
    uniform_time = glGetUniformLocation(shaderProgram, "time");
    uniform_mouse = glGetUniformLocation(shaderProgram, "mouse");
    uniform_resolution = glGetUniformLocation(shaderProgram, "resolution");
}

static void reshape(int w, int h)
{
    // Update dimension
    width = w;
    height = h;
    
//    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
//    glMatrixMode (GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
//    glMatrixMode (GL_MODELVIEW);
//    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glViewport(0, 0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
    
    glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
        case 'Q':
        case 'q':
            exit(0);
            break;
        default:
            break;
    }
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Simple Ray Tracer example");
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
//    glutIdleFunc(draw);
    
    glutPassiveMotionFunc(mouseMotionPassive);
    
    // Initialize program
    init();
    
    glutMainLoop();
    
    return 0;
}