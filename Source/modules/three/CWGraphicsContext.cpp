#include "config.h"

#include "modules/three/CWGraphicsContext.h"
#include "modules/webgl/WebGLRenderingContext.h"
#include "modules/webgl/WebGLUniformLocation.h"
#include "modules/webgl/WebGLProgram.h"

blink::WebGLRenderingContext* blink::CWGraphicsContext::mContext = NULL;

#include <android/log.h>
#define  LOG_TAG    "Three"
#define  LOG(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__);
// #define DEBUG_GL
#define NOT_IMPL

namespace blink {

static unsigned bufferToId(RefPtr<blink::WebGLBuffer>& pBuffer)
{
   //This is a short cut, won't be compliant to 64bit OS
   pBuffer.get()->ref();
   return (unsigned) pBuffer.get();
}

static blink::WebGLBuffer* idToBuffer(blink::WebGLId id)
{
  //This is a short cut, won't be compliant to 64bit OS  
  return (WebGLBuffer*)id;
}

static WebGLProgram* idToProgram(WebGLId id)
{
    //This is a short cut, won't be compliant to 64bit OS
    return (WebGLProgram*)id;
}

static WebGLId programToId(RefPtr<WebGLProgram>& pProgram)
{
    //This is a short cut, won't be compliant to 64bit OS
    pProgram.get()->ref();
    return (WebGLId) pProgram.get();
}

static WebGLTexture* idToTexture(WebGLId id)
{
    //This is a short cut, won't be compliant to 64bit OS
    return (WebGLTexture*)id;
}

WebGLId shaderToId(RefPtr<WebGLShader>& pShader)
{
    //This is a short cut, won't be compliant to 64bit OS
    pShader.get()->ref();
    return (WebGLId) pShader.get();
}

WebGLShader* idToShader(WebGLId id)
{
    //This is a short cut, won't be compliant to 64bit OS
    return (WebGLShader*)id;
}

WebGLId textureToId(RefPtr<WebGLTexture>& pTexture)
{
   //This is a short cut, won't be compliant to 64bit OS
    pTexture.get()->ref();
    return (WebGLId) pTexture.get();
}

    void CWGraphicsContext::genBuffers(GLsizei count, GLuint* ids) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        
        for(int i=0; i<count; i++)
        {
          RefPtr<WebGLBuffer> pBuffer = mContext->createBuffer();
          ids[i] = bufferToId(pBuffer);
        }
    }

    void CWGraphicsContext::bindBuffer(GLenum type, GLuint buffer) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        
        WebGLBuffer* pBuffer = idToBuffer(buffer);
        mContext->bindBuffer(type, pBuffer);
    }

    void CWGraphicsContext::bufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        
        mContext->bufferData(target, size, data, usage);
    }

    void CWGraphicsContext::enableVertexAttribArray(GLuint index) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->enableVertexAttribArray(index);
    }

    void CWGraphicsContext::disableVertexAttribArray(GLuint index) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->disableVertexAttribArray(index);
    }

    void CWGraphicsContext::vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,
                         GLsizei stride, GLvoid* offset) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->vertexAttribPointer(index, size, type, normalized, stride, (GLintptr)offset);
    }

    void CWGraphicsContext::drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* offset) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->drawElements(mode, count, type, (GLintptr)offset);
    }

    void CWGraphicsContext::drawArrays(GLenum mode, GLint first, GLsizei count) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->drawArrays(mode, first, count);
    }

    void CWGraphicsContext::uniform1f(GLint location, GLfloat x) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform1f(location, x);
    }

    void CWGraphicsContext::uniform1fv(GLint location, GLsizei count, const GLfloat* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (v == 0) {
            LOG("v == NULL")
            return;
        }
        LOG("count: %d v:%p v[0]=%f", count, v, v[0])
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform1fv(location, count, v);
    }

    void CWGraphicsContext::uniform1i(GLint location, GLint x) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform1i(location, x);
    }

    void CWGraphicsContext::uniform1iv(GLint location, GLsizei count, const GLint* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (v == 0) {
            LOG("v == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform1iv(location, count, v);
    }

    void CWGraphicsContext::uniform2f(GLint location, GLfloat x, GLfloat y) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform2f(location, x, y);
    }

    void CWGraphicsContext::uniform2fv(GLint location, GLsizei count, const GLfloat* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        LOG("%p", v)
        #endif
        if (v == 0) {
            LOG("v == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform2fv(location, count, (const WGC3Dfloat*)v);
    }

    void CWGraphicsContext::uniform2i(GLint location, GLint x, GLint y) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform2i(location, x, y);
    }

    void CWGraphicsContext::uniform2iv(GLint location, GLsizei count, const GLint* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (v == 0) {
            LOG("v == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform2iv(location, count, (const GLint*)v);
    }

    void CWGraphicsContext::uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform3f(location, x, y, z);
    }

    void CWGraphicsContext::uniform3fv(GLint location, GLsizei count, const GLfloat* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (v == 0) {
            LOG("v == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform3fv(location, count, (const GLfloat*)v);
    }

    void CWGraphicsContext::uniform3i(GLint location, GLint x, GLint y, GLint z) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform3i(location, x, y, z);
    }

    void CWGraphicsContext::uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform4f(location, x, y, z, w);
    }

    void CWGraphicsContext::uniform4fv(GLint location, GLsizei count, const GLfloat* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (v == 0) {
            LOG("v == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform4fv(location, count, (const GLfloat*)v);
    }

    void CWGraphicsContext::uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform4i(location, x, y, z, w);
    }

    void CWGraphicsContext::uniform4iv(GLint location, GLsizei count, const GLint* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (v == 0) {
            LOG("v == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniform4iv(location, count, (const GLint*)v);
    }

    void CWGraphicsContext::uniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (value == 0) {
            LOG("value == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniformMatrix2fv(location, count, transpose, value);
    }

    void CWGraphicsContext::uniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (value == 0) {
            LOG("value == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniformMatrix3fv(location, count, transpose,  (const GLfloat*)value);
    }

    void CWGraphicsContext::uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        if (value == 0) {
            LOG("value == NULL")
            return;
        }
        WebGraphicsContext3D* context = mContext->webContext();
        context->uniformMatrix4fv(location, count, transpose, (const GLfloat*)value);
    }

    void CWGraphicsContext::useProgram(GLuint program) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLProgram *pProgram = idToProgram(program);
        mContext->useProgram(pProgram);
    }

    GLint CWGraphicsContext::createProgram() {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        RefPtr<WebGLProgram> pProgram = mContext->createProgram();
        return programToId(pProgram);
    }

    GLuint CWGraphicsContext::createShader(GLint shaderType) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        RefPtr<WebGLShader> pShader = mContext->createShader(shaderType);
        return shaderToId(pShader);
    }

    void CWGraphicsContext::shaderSource(GLint shader,   GLsizei count, const GLchar **string, const GLint *length) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    
        WebGLShader* pShader = idToShader(shader);
        String sourceString(string[0]);
        for(int i = 1; i < count; i++)
        {
          String tempString(string[i]);
          sourceString.append(tempString);
        }
        mContext->shaderSource(pShader, sourceString);
    }

    void CWGraphicsContext::compileShader(GLint shader) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLShader* pShader = idToShader(shader);
    	mContext->compileShader(pShader);
    }

    void CWGraphicsContext::getShaderiv(GLint shader, GLenum pname, GLint* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        WebGLShader* pShader = idToShader(shader);
        context->getShaderiv(objectOrZero(pShader), pname, value);
    }

    void CWGraphicsContext::getShaderInfoLog(GLint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLShader* pShader = idToShader(shader);
        String infoString = mContext->getShaderInfoLog(pShader);
        *length = infoString.length() < (unsigned)maxLength ?  infoString.length() : maxLength;
         memcpy(infoLog, infoString.characters8(),(unsigned)*length);
    }

    GLint CWGraphicsContext::getUniformLocation(GLint program, const GLchar* name) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        LOG(name)
        #endif
        WebGLProgram *pProgram = idToProgram(program);
        String nameString(name);
        PassRefPtrWillBeRawPtr<WebGLUniformLocation> location = mContext->getUniformLocation(pProgram, nameString.utf8().data());
        if (location != nullptr)
            return location->location();
        else
            return -1;
    }

    GLint CWGraphicsContext::getAttribLocation(GLint program, const GLchar* name) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        String nameString(name);
        WebGLProgram* pProgram = idToProgram(program);
    	return mContext->getAttribLocation(pProgram, nameString);
    }

    void CWGraphicsContext::attachShader(GLint program, GLint shader) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLProgram* pProgram = idToProgram(program);
        WebGLShader*  pShader = idToShader(shader);
    	mContext->attachShader(pProgram, pShader);
    }

    void CWGraphicsContext::bindAttribLocation(GLint program, GLuint index, const GLchar* name) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLProgram* pProgram = idToProgram(program);
        String nameString(name);
    	mContext->bindAttribLocation(pProgram, index, nameString);
    }

    void CWGraphicsContext::linkProgram(GLint program) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLProgram* pProgram = idToProgram(program);
    	mContext->linkProgram(pProgram);
    }

    void CWGraphicsContext::deleteShader(GLint shader) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLShader*  pShader = idToShader(shader);
    	mContext->deleteShader(pShader);
    }

    void CWGraphicsContext::getIntegerv(GLenum pname, GLint* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	WebGraphicsContext3D* context = mContext->webContext();
        return context->getIntegerv(pname, value);
    }

    void CWGraphicsContext::viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->viewport(x, y, width, height);
    }

    void CWGraphicsContext::clear(GLbitfield mask) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->clear(mask);
    }

    void CWGraphicsContext::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->clearColor(red, green, blue, alpha);
    }

    void CWGraphicsContext::disable(GLenum cap) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->disable(cap);
    }

    void CWGraphicsContext::enable(GLenum cap) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->enable(cap);
    }

    void CWGraphicsContext::frontFace(GLenum mode) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->frontFace(mode);
    }

    void CWGraphicsContext::cullFace(GLenum mode) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->cullFace(mode);
    }

    void CWGraphicsContext::depthMask(GLboolean flag) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->depthMask(flag);
    }

    void CWGraphicsContext::lineWidth(GLfloat width) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->lineWidth(width);
    }

    void CWGraphicsContext::polygonOffset(GLfloat factor, GLfloat units) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->polygonOffset(factor, units);
    }

    void CWGraphicsContext::blendEquation(GLenum mode) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->blendEquation(mode);
    }

    void CWGraphicsContext::blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->blendEquationSeparate(modeRGB, modeAlpha);
    }

    void CWGraphicsContext::blendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->blendColor(red, green, blue, alpha);
    }

    void CWGraphicsContext::blendFunc(GLenum sfactor, GLenum dfactor) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->blendFunc(sfactor, dfactor);
    }

    void CWGraphicsContext::blendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->blendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    }

    void CWGraphicsContext::genTextures(GLsizei count, GLuint* ids) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
	    for(int i=0; i<count; i++)
	    {
	      RefPtr<WebGLTexture> pTexture = mContext->createTexture();
	      ids[i] = textureToId(pTexture);
	    }
    }

    void CWGraphicsContext::activeTexture(GLenum texture) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->activeTexture(texture);
    }

    void CWGraphicsContext::bindTexture(GLenum target, GLint texture) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGLTexture* pTexture = idToTexture(texture);
    	mContext->bindTexture(target, pTexture);
    }

    void CWGraphicsContext::texImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                                             GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->texImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    }

    void CWGraphicsContext::generateMipmap(GLenum target) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->generateMipmap(target);
    }

    void CWGraphicsContext::clearDepth(GLclampf depth) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->clearDepth(depth);
    }

    void CWGraphicsContext::clearStencil(GLint s) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->clearStencil(s);
    }

    void CWGraphicsContext::depthFunc(GLenum func) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
    	mContext->depthFunc(func);
    }

    GLenum CWGraphicsContext::getError() {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        return mContext->getError();
    }

    void CWGraphicsContext::deleteBuffers(GLsizei count, unsigned* ids) {        
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        for(int i=0; i<count; i++)
        {
          WebGLBuffer* pBuffer = idToBuffer(ids[i]);
          mContext->deleteBuffer(pBuffer);
          // if(pBuffer) pBuffer->deref();
        }
    }

    void CWGraphicsContext::deleteTextures(GLsizei count, unsigned* ids) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        for(int i=0; i<count; i++)
        {
          WebGLTexture* pTexture = idToTexture(ids[i]);
          mContext->deleteTexture(pTexture);
          if(pTexture) pTexture->deref();
        }
    }

    void CWGraphicsContext::genFramebuffers(GLsizei count, unsigned* ids) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        //TODO
        // mContext->genFramebuffers(count,ids);
    }

    void CWGraphicsContext::pixelStorei(GLenum pname, GLint param) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif 
        mContext->pixelStorei(pname, param);
    }

//guangzhen
    void CWGraphicsContext::deleteFramebuffers(GLsizei count, unsigned* ids) {       
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif 
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        //m_pWebGraphicsContext3D->deleteFramebuffers(count,ids);
        //TODO
    }

    void CWGraphicsContext::genRenderbuffers(GLsizei count, unsigned* ids) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif

        // mContext->genRenderbuffers(count,ids);
    }

    unsigned CWGraphicsContext::createTexture() {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // RefPtr<WebGLTexture> pTexture = m_pWebGLRenderingContext->createTexture();
        // return textureToId(pTexture);
        return 0;
    }

    void CWGraphicsContext::deleteRenderbuffers(GLsizei count, unsigned* ids) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif

        // mContext->deleteRenderbuffers(count,ids);
    }

    void CWGraphicsContext::getFloatv(GLenum pname, GLfloat* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // m_pWebGraphicsContext3D->getFloatv(pname, value); //Shortcut
    }

    void CWGraphicsContext::getTexParameteriv(GLenum target, GLenum pname, GLint* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // WebGLGetInfo info = m_pWebGLRenderingContext->getTexParameter(target, pname);
        // *value = getIntfromGLInfo(info);//info.getInt();
    }

   void CWGraphicsContext::getTexParameterfv(GLenum target, GLenum pname, GLfloat* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        context->getTexParameterfv(target, pname, value);
    }

    void CWGraphicsContext::getProgramiv(unsigned program, GLenum pname, GLint* value) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        WebGraphicsContext3D* context = mContext->webContext();
        WebGLProgram* pProgram = idToProgram(program);
        context->getProgramiv(objectOrZero(pProgram), pname, value);
    }

    void CWGraphicsContext::uniform3iv(GLint location, GLsizei count, const GLint* v) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // if(m_pWebGraphicsContext3D)
        //    m_pWebGraphicsContext3D->uniform3iv(location, count, v);

    }

    void CWGraphicsContext::texParameteri(GLenum target, GLenum pname, GLint param) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        
        mContext->texParameteri(target, pname, param);
    }

    void CWGraphicsContext::texParameterf(GLenum target, GLenum pname, GLfloat param) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // m_pWebGLRenderingContext->texParameterf(target, pname, param);
    }

    void CWGraphicsContext::scissor(GLint x, GLint y, GLsizei width, GLsizei height) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // m_pWebGLRenderingContext->scissor(x, y, width, height);
    }

    void CWGraphicsContext::deleteProgram(unsigned program) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        
        WebGLProgram* pProgram = idToProgram(program);
        mContext->deleteProgram(pProgram);
        if(pProgram) pProgram->deref();
    }

    void CWGraphicsContext::getProgramInfoLog(unsigned program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        
        WebGLProgram* pProgram = idToProgram(program);
        String infoString = mContext->getProgramInfoLog(pProgram);
        *length = infoString.length() < (unsigned)maxLength ?  infoString.length() : maxLength;
        memcpy(infoLog, infoString.characters8(),(unsigned)*length);
    }

    void CWGraphicsContext::bindFramebuffer(GLenum target, unsigned framebuffer) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // m_pWebGraphicsContext3D->bindFramebuffer(target, framebuffer);
    }

    void CWGraphicsContext::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, unsigned texture, GLint level) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // WebGLTexture* pTexture = idToTexture(texture);
        // Platform3DObject textureObject = objectOrZero(pTexture);
        // switch (attachment) {
        // case GC3D_DEPTH_STENCIL_ATTACHMENT_WEBGL:
        //     m_pWebGraphicsContext3D->framebufferTexture2D(target, GL_DEPTH_ATTACHMENT, textarget, textureObject, level);
        //     m_pWebGraphicsContext3D->framebufferTexture2D(target, GL_STENCIL_ATTACHMENT, textarget, textureObject, level);
        //     break;
        // case GL_DEPTH_ATTACHMENT:
        //     m_pWebGraphicsContext3D->framebufferTexture2D(target, attachment, textarget, textureObject, level);
        //     break;
        // case GL_STENCIL_ATTACHMENT:
        //     m_pWebGraphicsContext3D->framebufferTexture2D(target, attachment, textarget, textureObject, level);
        //     break;
        // default:
        //     m_pWebGraphicsContext3D->framebufferTexture2D(target, attachment, textarget, textureObject, level);
        // }
    }

    void CWGraphicsContext::bindRenderbuffer(GLenum target, unsigned renderbuffer) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // m_pWebGraphicsContext3D->bindRenderbuffer(target, renderbuffer);
    }

    void CWGraphicsContext::renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // m_pWebGLRenderingContext->renderbufferStorage(target, internalformat, width, height);
    }

    void CWGraphicsContext::framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, unsigned renderbuffer) {
        #ifdef DEBUG_GL
        LOG(__FUNCTION__)
        #endif
        #ifdef NOT_IMPL
        LOG("===")
        LOG(__FUNCTION__)
        LOG("===")
        #endif
        // #ifdef DEBUG_GL
        // LOG(ERROR) << "CCGraphicsContext3DImpl:" << __FUNCTION__;
        // #endif
        
        // m_pWebGraphicsContext3D->framebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    }

}