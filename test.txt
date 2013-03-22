//
//  shader_prog.h
//  SimpleShaderCpp
//
//  Created by Emmanuel Piuze on 13-03-21.
//  Copyright (c) 2013 Emmanuel Piuze. All rights reserved.
//

#ifndef SimpleShaderCpp_shader_prog_h
#define SimpleShaderCpp_shader_prog_h
#endif

#include <iostream>
#include <stdexcept>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif


class shader_prog {
    GLuint vertex_shader, fragment_shader, prog;
    
    template <int N>
    GLuint compile(GLuint type, char const *(&source)[N]) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, N, source, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, ' ');
            glGetShaderInfoLog(shader, length, &length, &log[0]);
            throw std::logic_error(log);
            return false;
        }
        return shader;
    }
public:
    template <int N, int M>
    shader_prog(GLchar const *(&v_source)[N], GLchar const *(&f_source)[M]) {
        vertex_shader = compile(GL_VERTEX_SHADER, v_source);
        fragment_shader = compile(GL_FRAGMENT_SHADER, f_source);
        prog = glCreateProgram();
        glAttachShader(prog, vertex_shader);
        glAttachShader(prog, fragment_shader);
        glLinkProgram(prog);
    }
    
    operator GLuint() { return prog; }
    void operator()() { glUseProgram(prog); }
    
    ~shader_prog() {
        glDeleteProgram(prog);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
};
