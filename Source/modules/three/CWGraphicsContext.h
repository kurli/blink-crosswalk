#ifndef CWGraphicsContext_H
#define CWGraphicsContext_H

typedef char GLchar;
typedef unsigned GLenum;
typedef unsigned char GLboolean;
typedef unsigned GLbitfield;
typedef signed char GLbyte;
typedef unsigned char GLubyte;
typedef short GLshort;
typedef unsigned short GLushort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef signed long int GLintptr;
typedef signed long int GLsizeiptr;
typedef void GLvoid;

namespace blink {
class WebGLRenderingContext;
class WebGLBuffer;
class CWGraphicsContext {
public:
    static WebGLRenderingContext* mContext;

    static void genBuffers(GLsizei count, GLuint* ids);
    static void bindBuffer(GLenum type, GLuint buffer);

    static void bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);

    static void enableVertexAttribArray(GLuint index);

    static void disableVertexAttribArray(GLuint index);

    static void vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,
                         GLsizei stride, GLvoid* offset);
    static void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* offset);

    static void drawArrays(GLenum mode, GLint first, GLsizei count);

    static void uniform1f(GLint location, GLfloat x);

    static void uniform1fv(GLint location, GLsizei count, const GLfloat* v);

    static void uniform1i(GLint location, GLint x);

    static void uniform1iv(GLint location, GLsizei count, const GLint* v);

    static void uniform2f(GLint location, GLfloat x, GLfloat y);

    static void uniform2fv(GLint location, GLsizei count, const GLfloat* v);

    static void uniform2i(GLint location, GLint x, GLint y);

    static void uniform2iv(GLint location, GLsizei count, const GLint* v);

    static void uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);

    static void uniform3fv(GLint location, GLsizei count, const GLfloat* v);

    static void uniform3i(GLint location, GLint x, GLint y, GLint z);

    static void uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) ;

    static void uniform4fv(GLint location, GLsizei count, const GLfloat* v);

    static void uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);

    static void uniform4iv(GLint location, GLsizei count, const GLint* v);

    static void uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

    static void uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

    static void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

    static void useProgram(GLuint program);

    static GLint createProgram();

    static GLuint createShader(GLint shaderType);

    static void shaderSource(GLint shader,   GLsizei count, const GLchar **string, const GLint *length);

    static void compileShader(GLint shader) ;

    static void getShaderiv(GLint shader, GLenum pname, GLint* value) ;

    static void getShaderInfoLog(GLint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog) ;

    static GLint getUniformLocation(GLint program, const GLchar* name) ;

    static GLint getAttribLocation(GLint program, const GLchar* name);

    static void attachShader(GLint program, GLint shader) ;

    static void bindAttribLocation(GLint program, GLuint index, const GLchar* name);

    static void linkProgram(GLint program) ;

    static void deleteShader(GLint shader);

    static void getIntegerv(GLenum pname, GLint* value);

    static void viewport(GLint x, GLint y, GLsizei width, GLsizei height);

    static void clear(GLbitfield mask);

    static void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    static void disable(GLenum cap);

    static void enable(GLenum cap);

    static void frontFace(GLenum mode);

    static void cullFace(GLenum mode);

    static void depthMask(GLboolean flag);

    static void lineWidth(GLfloat width) ;

    static void polygonOffset(GLfloat factor, GLfloat units);
    static void blendEquation(GLenum mode);
    static void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
    static void blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

    static void blendFunc(GLenum sfactor, GLenum dfactor);
    static void blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

    static void genTextures(GLsizei count, GLuint* ids);

    static void activeTexture(GLenum texture) ;

    static void bindTexture(GLenum target, GLint texture) ;

    static void texImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                                             GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);

    static void generateMipmap(GLenum target);

    static void clearDepth(GLclampf depth);

    static void clearStencil(GLint s);

    static void depthFunc(GLenum func);

    static GLenum getError();

    static void deleteBuffers(GLsizei count, unsigned* ids);

    static void deleteTextures(GLsizei count, unsigned* ids);

    static void genFramebuffers(GLsizei count, unsigned* ids);

    static void deleteFramebuffers(GLsizei count, unsigned* ids);

    static void genRenderbuffers(GLsizei count, unsigned* ids);

    static unsigned createTexture();

    static void deleteRenderbuffers(GLsizei count, unsigned* ids);

    static void getFloatv(GLenum pname, GLfloat* value);

    static void getTexParameteriv(GLenum target, GLenum pname, GLint* value);

    static void getTexParameterfv(GLenum target, GLenum pname, GLfloat* value);

    static void getProgramiv(unsigned program, GLenum pname, GLint* value);

    static void uniform3iv(GLint location, GLsizei count, const GLint* v);

    static void texParameteri(GLenum target, GLenum pname, GLint param);

    static void texParameterf(GLenum target, GLenum pname, GLfloat param);

    static void scissor(GLint x, GLint y, GLsizei width, GLsizei height);

    static void deleteProgram(unsigned program);

    static void getProgramInfoLog(unsigned program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

    static void bindFramebuffer(GLenum target, unsigned framebuffer);

    static void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, unsigned texture, GLint level);

    static void bindRenderbuffer(GLenum target, unsigned renderbuffer);

    static void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    
    static void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, unsigned renderbuffer);

    static void pixelStorei(GLenum pname, GLint param);

};
}
#endif
