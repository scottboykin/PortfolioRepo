#include "Renderer.h"
#include "Material.h"
#include "Vertex.h"
#include "vec3.h"

using namespace SBMath;

namespace SBGraphics
{
    Renderer::Renderer()
        :   m_materialManager()
    {}

    void Renderer::drawVertexArray( Material& material, GLenum mode, GLint first, GLsizei count )
    {
        material.apply();
        glDrawArrays( mode, first, count );
        material.remove();
    }

    void Renderer::drawElements( Material& material, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
    {
        material.apply();
        glDrawElements( mode, count, type, indices );
        material.remove();
    }

    void Renderer::createWindow( int argc, char** argv, const char* windowName, int width, int height, int xPos, int yPos )
    {
        glutInit( &argc, argv );
        glutInitDisplayMode( GLUT_DOUBLE );
        glutInitWindowSize( width, height );
        glutInitWindowPosition( xPos, yPos );
        glutCreateWindow( windowName );

        glewInit();
    }

    Renderer& Renderer::getRenderer()
    {
        static Renderer renderer;
        return renderer;
    }

    void Renderer::generateCubeVertices( std::vector< Vertex >& outputVertices, std::vector< unsigned int >& outputIndices, bool requestIndices )
    {
        const int VERTS_PER_FACE = 6;
        int face = 0;

        for( int axis = 0; axis < 3; ++axis )
        {
            for( int sign = -1; sign < 2; sign += 2 )
            {
                for( int iRawVertex = 0; iRawVertex < VERTS_PER_FACE; ++iRawVertex )
                {
                    Vertex newVertex;
                    newVertex.normal[ axis ] = (float)sign;

                    int normalAxis = 
                        abs( newVertex.normal.x ) > abs( newVertex.normal.y )
                        ?
                        ( abs( newVertex.normal.x ) > abs( newVertex.normal.z ) ? 0 : 2 )
                        :
                        ( abs( newVertex.normal.y ) > abs( newVertex.normal.z ) ? 1 : 2 );

                    int otherAxisA = ( normalAxis + 1 ) % 3;
                    int otherAxisB = ( normalAxis + 2 ) % 3;

                    newVertex.position[ normalAxis ] = newVertex.normal[ normalAxis ] > 0 ? 1.0f : 0.0f;

                    bool reverseWinding = newVertex.normal[ normalAxis ] <= 0;

                    if( normalAxis == 0 )
                    {
                        std::swap( otherAxisA, otherAxisB );
                        reverseWinding = !reverseWinding;
                    }

                    int iLogicalVertex = ( iRawVertex > 3 ) ? ( VERTS_PER_FACE - iRawVertex ) : iRawVertex;

                    newVertex.position[ otherAxisA ] = ( iLogicalVertex & 1 ) ? 1.0f : 0.0f;

                    if( reverseWinding )
                    {
                        newVertex.position[ otherAxisA ] = 1.0f - newVertex.position[ otherAxisA ];
                    }

                    newVertex.position[ otherAxisB ] = ( iLogicalVertex >> 1 ) ? 1.0f : 0.0f;

                    newVertex.position -= vec3( 0.5 );

                    switch( iRawVertex )
                    {
                        case 0: 
                            newVertex.texcoords = vec2( 0.0, 0.0 );
                            break;

                        case 1:
                            newVertex.texcoords = vec2( 1.0, 0.0 );
                            break;

                        case 2:
                            newVertex.texcoords = vec2( 0.0, 1.0 );
                            break;

                        case 3:
                            newVertex.texcoords = vec2( 1.0, 1.0 );
                            break;
                    }

                    if( requestIndices )
                    {
                        outputIndices.push_back( iLogicalVertex + face * 4 );
                        if( iRawVertex > 3 )
                        {
                            continue;                        
                        }
                    }

                    outputVertices.push_back( newVertex );
                }
                ++face;
            }
        }
    }
}