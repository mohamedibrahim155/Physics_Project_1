
#include "Renderer.h"


#pragma region Rendering Errors


void GLClrError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OPENGL] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

#pragma endregion

void Renderer::Clear()
{

    glClearColor(0.7f, 0.5f, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}

void Renderer::Draw(const VertexArray& va, const Shader& shader) const
{
    va.Bind();
    shader.use();
    GLCall( glDrawArrays(GL_TRIANGLES, 0, 36));


}
