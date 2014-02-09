#pragma once

#include <glew.h>
#include <glut.h>
#include <map>

#include <vector>

#include "MatrixStack.h"
#include "MaterialManager.h"

namespace SBGraphics
{
    class Vertex;

    class Renderer
    {

    public:

        sb::MatrixStack projectionMatrixStack, viewMatrixStack, modelMatrixStack;

        void drawVertexArray( Material& material, GLenum mode, GLint first, GLsizei count );
        void drawElements( Material& material, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );

        void createWindow( int argc, char** argv, const char* windowName, int width, int height, int xPos, int yPos );
        
        void useMaterial( const char* name );

        static Renderer& getRenderer();
        MaterialManager& getMaterialManager(){ return m_materialManager; }

        static void generateCubeVertices( std::vector< Vertex >& outputVertices, std::vector< unsigned int >& outputIndices = std::vector<unsigned int>(), bool requestIndices = false );

    private:

        MaterialManager m_materialManager;

        Renderer();

    };
}