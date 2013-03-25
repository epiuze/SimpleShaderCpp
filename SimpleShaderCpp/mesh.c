/* sets up flat mesh */
/* sets up elapsed time parameter for use by shaders */
/* vertex shader varies height of mesh sinusoidally */
/* uses a pass through fragment shader */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut,h>
#endif

#define N 32

#define bool int
#define false 0
#define true 1

float   angle = 0.0, axis[3], trans[3];
bool    trackingMouse = false;
bool    redrawContinue = false;
bool    trackballMove = false;

const GLdouble nearVal     = 1.0; /* near distance */
const GLdouble farVal      = 20.0; /* far distance */
GLuint         program     = 0; /* program object id */
/* GLint          timeParam; */

int winWidth, winHeight;

GLchar *ebuffer; /* buffer for error messages */
GLsizei elength; /* length of error message */

GLfloat data[N][N]; /* array of data heights */


/* 
** These functions implement a simple trackball-like motion control.
*/

float lastPos[3] = {0.0F, 0.0F, 0.0F};
int curx, cury;
int startX, startY;

void
trackball_ptov(int x, int y, int width, int height, float v[3])
{
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


void mouseMotion(int x, int y) {
    float curPos[3], dx, dy, dz;
    
    trackball_ptov(x, y, winWidth, winHeight, curPos);
    if(trackingMouse)
    {
        dx = curPos[0] - lastPos[0];
        dy = curPos[1] - lastPos[1];
        dz = curPos[2] - lastPos[2];
        
        if (dx || dy || dz) {
            angle = 90.0F * sqrt(dx*dx + dy*dy + dz*dz);
            
            axis[0] = lastPos[1]*curPos[2] - lastPos[2]*curPos[1];
            axis[1] = lastPos[2]*curPos[0] - lastPos[0]*curPos[2];
            axis[2] = lastPos[0]*curPos[1] - lastPos[1]*curPos[0];
            
            lastPos[0] = curPos[0];
            lastPos[1] = curPos[1];
            lastPos[2] = curPos[2];
        }
    }
    glutPostRedisplay();
}

void startMotion(int x, int y) {
    
    trackingMouse = true;
    redrawContinue = false;
    startX = x; startY = y;
    curx = x; cury = y;
    trackball_ptov(x, y, winWidth, winHeight, lastPos);
	trackballMove=true;
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


/* shader reader */
/* creates null terminated string from file */

static char* readShaderSource(const char* shaderFile)
{
    FILE* fp = fopen(shaderFile, "rb");
    char* buf;
    long size;

    if(fp==NULL) return NULL;
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    buf = (char*) malloc((size + 1) * sizeof(char)); 
    fread(buf, 1, size, fp);
    fclose(fp);
    return buf;
}

/* standard OpenGL initialization */

static void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0, 0.0, 0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.5,1.0,-5.5,5.5);
}

/* GLSL initialization */

static void initShader(const GLchar* vShaderFile, const GLchar* fShaderFile)
{
    GLint status; 
    GLchar *vSource, *fSource;
    GLuint vShader, fShader;

    /* read shader files */

    vSource = readShaderSource(vShaderFile);
    if(vSource==NULL)
    {
        printf( "Failed to read vertex shaderi\n");
        exit(EXIT_FAILURE);
    }

    fSource = readShaderSource(fShaderFile);
    if(fSource==NULL)
    {
        printf("Failed to read fragment shader");
        exit(EXIT_FAILURE);
    }

    /* create program and shader objects */

    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();

    /* attach shaders to the program object */

    glAttachShader(program, vShader); 
    glAttachShader(program, fShader);

    /* read shaders */

    glShaderSource(vShader, 1, (const GLchar**) &vSource, NULL);
    glShaderSource(fShader, 1, (const GLchar**) &fSource, NULL);

    /* compile vertex shader shader */

    glCompileShader(vShader);

    /* error check */

    glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
    if(status==GL_FALSE)
    {
       printf("Failed to compile the vertex shader.\n");
       glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &elength);
       ebuffer = malloc(elength*sizeof(char));
       glGetShaderInfoLog(vShader, elength, NULL, ebuffer);
       printf("%s\n", ebuffer);
       exit(EXIT_FAILURE);
    }

    /* compile fragment  shader shader */

    glCompileShader(fShader);

    /* error check */

    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
    if(status==GL_FALSE)
    {
       printf("Failed to compile the fragment shader.\n");
       glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &elength);
       ebuffer = malloc(elength*sizeof(char));
       glGetShaderInfoLog(fShader, elength, NULL, ebuffer);
       printf("%s\n", ebuffer); 
       exit(EXIT_FAILURE);
    }

    /* link  and error check */

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status==GL_FALSE)
    {
       printf("Failed to link program object.\n");
       glGetProgramiv(program, GL_INFO_LOG_LENGTH, &elength);
       ebuffer = malloc(elength*sizeof(char));
       glGetProgramInfoLog(program, elength, &elength, ebuffer);
       printf("%s\n", ebuffer);
       exit(EXIT_FAILURE);
    }

    /* use program object */

    glUseProgram(program);

    /* set up uniform parameter */

    /* timeParam = glGetUniformLocation(program, "time"); */
}

void mesh()
{
    int i,j;
    /* glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2.0, 2.0, 2.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0); */
    for(i=0; i<N; i++) for(j=0; j<N;j++)
    {
       glBegin(GL_LINE_LOOP);
          glVertex3f((float)i/N-0.5, data[i][j], (float)j/N-0.5);
          glVertex3f((float)i/N-0.5, data[i][j+1], (float)(j+1)/N-0.5);
          glVertex3f((float)(i+1)/N-0.5, data[i+1][j+1], (float)(j+1)/N-0.5);
          glVertex3f((float)(i+1)/N-0.5, data[i+1][j], (float)(j)/N-0.5);
       glEnd();  
    }
}

static void draw(void)
{
    /* send elapsed time to shaders */

    /* glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME)); */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(trackballMove) {
        glRotatef(angle, axis[0], axis[1], axis[2]);
    }
    mesh();
    glutSwapBuffers();
}

static void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.75,0.75,-0.5,1.0,-5.5,5.5);
    gluLookAt(2.0, 2.0, 2.0, 0.5, 0.0, 0.5, 0.0, 1.0, 0.0);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    winWidth = w;
    winHeight = h;
    glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
    case 'Q':
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    default:
        break;
    }
}

void idle()
{
   /* glUniform1f(timeParam, (GLfloat) glutGet(GLUT_ELAPSED_TIME)); */
   if(redrawContinue) glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y)
{
        if(button==GLUT_RIGHT_BUTTON) exit(0);
        if(button==GLUT_MIDDLE_BUTTON) trackballMove = false;
        if(button==GLUT_LEFT_BUTTON) switch(state)
        {
    case GLUT_DOWN:
        y=winHeight-y;
        startMotion( x,y);
        break;
    case GLUT_UP:
        stopMotion( x,y);
        break;
    }
}


int main(int argc, char** argv)
{
    int i,j;

    /* flat mesh */ 

    for(i=0;i<N;i++) for(j=0;j<N;j++) data[i][j]=0.0;

    glutInit(&argc, argv);
#ifndef __APPLE__
glewInit();
#endif
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    init();
    initShader("vmesh.glsl", "fPassthrough.glsl");

    glutMainLoop();
    return 0 ; /* to keep compiler from complaining */
}
