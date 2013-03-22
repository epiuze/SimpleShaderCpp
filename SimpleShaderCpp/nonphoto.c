/* display teapot with vertex and fragment shaders */
/* sets up elapsed time parameter for use by shaders */

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <math.h>


const GLdouble nearVal     = 1.0;
const GLdouble farVal      = 20.0;
GLfloat  lightPos[4] = {3.0f, 3.0f, 3.0f, 1.0f};
GLuint         program     = 0;
GLint          timeParam;

GLchar *ebuffer; /* buffer for error messages */
GLsizei elength;  /* length of error messages */

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

/* error printing function */


/* standard OpenGL initialization */

static void init()
{
    const float teapotColor[]     = {0.3f, 0.5f, 0.4f, 1.0f}; 
    const float teapotSpecular[]  = {0.8f, 0.8f, 0.8f, 1.0f};
    const float teapotShininess[] = {80.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, teapotColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, teapotSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, teapotShininess);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double) glutGet(GLUT_WINDOW_WIDTH) / (double) glutGet(GLUT_WINDOW_HEIGHT), nearVal, farVal);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING); 
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

/* GLSL initialization */

static void initShader(const GLchar* vShaderFile, const GLchar* fShaderFile)
{
    GLint status = glGetError()==GL_NO_ERROR; 
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

    /* compile vertex shaders */

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

    /* compile fragment shader */

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


    /* link */

    glLinkProgram(program);
    glGetShaderiv(program, GL_LINK_STATUS, &status);

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

    timeParam = glGetUniformLocation(program, "time");
}

static void draw(void)
{
    /* send elapsed time to shaders */

    glUniform1f(timeParam, glutGet(GLUT_ELAPSED_TIME));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glutSolidTeapot(1.0);
    glPopMatrix();
    glutSwapBuffers();
}

static void reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double) w / (double) h, nearVal, farVal);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, w, h);

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

void spinLight()
{
    float t;
    t =  glutGet(GLUT_ELAPSED_TIME)/1000.0;
    lightPos[0] = 3.0*sin(t);
    lightPos[2] = 2.0*cos(t);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glutPostRedisplay();
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Simple GLSL example");
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(spinLight);

    init();
    initShader("vnonphoto.glsl", "fPassthrough.glsl");

    glutMainLoop();
    return 0;
}
