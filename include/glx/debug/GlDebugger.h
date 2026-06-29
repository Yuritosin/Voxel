#ifndef GL_DEBUGGER_H
#define GL_DEBUGGER_H

#include <string>
#include <iostream>

#include <GL/gl.h>

namespace GlDebugger {
    static GLenum glCheckError_(const char* file, const int line) {
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            std::string message;
            switch (err) {
                case GL_INVALID_ENUM: message = "GL_INVALID_ENUM"; break;
                case GL_INVALID_VALUE: message = "GL_INVALID_VALUE"; break;
                case GL_INVALID_OPERATION: message = "GL_INVALID_OPERATION"; break;
                case GL_STACK_OVERFLOW: message = "GL_STACK_OVERFLOW"; break;
                case GL_OUT_OF_MEMORY: message = "GL_OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: message = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cerr << "OpenGL error: "<< message << " (" << err << ") " << file << " (line: " << line << ")\n";
        }
        return err;
    }
    #define glCheckError() glCheckError_((__FILE__), (__LINE__));
};


#endif
