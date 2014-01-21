#include <glew.h>
#include <glut.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <map>

#include "Font.h"
#include "vec3.h"
#include "vec2.h"
#include "Camera.h"
#include "CommandWindow.h"
#include "Texture.h"
#include "Utilities.h"
#include "mat3x3.h"
#include "mat4x4.h"
#include "MatrixStack.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "Material.h"
#include "DebugOutputManager.h"
#include "StringParser.h"
#include "Entity.h"
#include "Light.h"
#include "StringParser.h"
#include "XMLReader.h"
#include "Mesh.h"
#include "Model.h"
#include "NamedProperty.h"
#include "EventSubscriber.h"
#include "EventSystem.h"

using namespace sb;
using namespace SBMath;
using namespace SBGraphics;
using namespace SBUtil;

struct vertex_ {
    GLfloat  x,  y,  z;
    GLfloat nx, ny, nz;
    GLfloat tx, ty, tz;
    GLfloat _tx, _ty, _tz;
    GLfloat _bx, _by, _bz;
    GLfloat _nx, _ny, _nz;
    GLfloat padding[14];
};

void computeTangentSpaceMatrix(vertex_& p0, const vertex_& p1, const vertex_& p2) 
{
    GLfloat v1x, v1y, v1z, v2x, v2y, v2z, u1x, u1y, u2x, u2y, det;

    v1x = p1.x - p0.x;
    v1y = p1.y - p0.y;
    v1z = p1.z - p0.z;

    v2x = p2.x - p0.x;
    v2y = p2.y - p0.y;
    v2z = p2.z - p0.z;

    u1x = p1.tx - p0.tx;
    u1y = p1.ty - p0.ty;

    u2x = p2.tx - p0.tx;
    u2y = p2.ty - p0.ty;

    det = u1x * u2y - u2x * u1y;

    p0._tx = (v1x * u2y - v2x * u1y) / det;
    p0._ty = (v1y * u2y - v2y * u1y) / det;
    p0._tz = (v1z * u2y - v2z * u1y) / det;

    p0._bx = (-v1x * u2x + v2x * u1x) / det;
    p0._by = (-v1y * u2x + v2y * u1x) / det;
    p0._bz = (-v1z * u2x + v2z * u1x) / det;

    p0._nx = p0._by * p0._tz - p0._bz * p0._ty;
    p0._ny = p0._bz * p0._tx - p0._bx * p0._tz;
    p0._nz = p0._bx * p0._ty - p0._by * p0._tx;

    if( p0.y > 0 )
    {
        p0._ny = -p0._ny;
    }
}

void computeTangentSpaceMatrix( Vertex& p0, const Vertex& p1, const Vertex& p2) 
{
    float v1x, v1y, v1z, v2x, v2y, v2z, u1x, u1y, u2x, u2y, det;

    v1x = p1.position.x - p0.position.x;
    v1y = p1.position.y - p0.position.y;
    v1z = p1.position.z - p0.position.z;

    v2x = p2.position.x - p0.position.x;
    v2y = p2.position.y - p0.position.y;
    v2z = p2.position.z - p0.position.z;

    u1x = p1.texcoords.x - p0.texcoords.x;
    u1y = p1.texcoords.y - p0.texcoords.y;

    u2x = p2.texcoords.x - p0.texcoords.x;
    u2y = p2.texcoords.y - p0.texcoords.y;

    det = u1x * u2y - u2x * u1y;

    /*p0.tangent.x = (v1x * u2y - v2x * u1y) / det;
    p0.tangent.y = (v1y * u2y - v2y * u1y) / det;
    p0.tangent.z = (v1z * u2y - v2z * u1y) / det;

    p0.bitangent.x = (-v1x * u2x + v2x * u1x) / det;
    p0.bitangent.y = (-v1y * u2x + v2y * u1x) / det;
    p0.bitangent.z = (-v1z * u2x + v2z * u1x) / det;

    p0.tangentNormal.x = p0.bitangent.y * p0.tangent.z - p0.bitangent.z * p0.tangent.y;
    p0.tangentNormal.y = p0.bitangent.z * p0.tangent.x - p0.bitangent.x * p0.tangent.z;
    p0.tangentNormal.z = p0.bitangent.x * p0.tangent.y - p0.bitangent.y * p0.tangent.x;

    if( p0.position.y > 0 )
    {
        p0.tangentNormal.y = -p0.tangentNormal.y;
    }*/
}

void updateFrame();
void updateDisplay();
void setupGLUT( int argc, char** argv );

Font myFont;
Mesh myMeshes[10];
Model myModels[10];
int currentMesh = 0;
Camera& myCamera = Camera::getStaticCamera();
CommandWindow* myCommandWindow;
DebugOutputManager debugOutputManager;
GLuint modelProgramID;
int frame = 0;
EventSystem eventSystem;

std::map< std::string, Entity > entities;

GLuint vbo_vertices, vbo_indices, frameBufferID;
float time = 0;

const int useParallax = 0x00000001;
const int useDiffuse =  0x00000010;
const int useSpecular = 0x00000100;
const int useEmissive = 0x00001000;
const int useLighting = 0x00010000;
const int useFog =      0x00100000;

int shaderDebugInt = 0;
int numOfLights = 1;
Light lights[5];

bool bPaused = false;

const int DEFAULT_WINDOW_WIDTH = 1024;
const int DEFAULT_WINDOW_HEIGHT = 768;
const float WINDOW_ASPECT_RATIO = static_cast< float >( DEFAULT_WINDOW_WIDTH ) / DEFAULT_WINDOW_HEIGHT;

unsigned int texels[ DEFAULT_WINDOW_WIDTH * DEFAULT_WINDOW_HEIGHT ];
unsigned int depthTexels[ DEFAULT_WINDOW_WIDTH * DEFAULT_WINDOW_HEIGHT ];
Texture frameBufferTexture;
GLuint depthTextureID;

vertex_ vertices[] = 
{
    {-1.0,  1.0,  1.0,  0.0,  0.0,  1.0,  0.0,  0.0,  0.0},
    {-1.0, -1.0,  1.0,  0.0,  0.0,  1.0,  0.0,  1.0,  0.0},
    { 1.0, -1.0,  1.0,  0.0,  0.0,  1.0,  1.0,  1.0,  0.0},
    { 1.0,  1.0,  1.0,  0.0,  0.0,  1.0,  1.0,  0.0,  0.0},

    { 1.0,  1.0, -1.0,  0.0,  0.0, -1.0,  0.0,  0.0,  0.0},
    { 1.0, -1.0, -1.0,  0.0,  0.0, -1.0,  0.0,  1.0,  0.0},
    {-1.0, -1.0, -1.0,  0.0,  0.0, -1.0,  1.0,  1.0,  0.0},
    {-1.0,  1.0, -1.0,  0.0,  0.0, -1.0,  1.0,  0.0,  0.0},

    { 1.0,  1.0,  1.0,  1.0,  0.0,  0.0,  0.0,  0.0,  0.0},
    { 1.0, -1.0,  1.0,  1.0,  0.0,  0.0,  0.0,  1.0,  0.0},
    { 1.0, -1.0, -1.0,  1.0,  0.0,  0.0,  1.0,  1.0,  0.0},
    { 1.0,  1.0, -1.0,  1.0,  0.0,  0.0,  1.0,  0.0,  0.0},

    {-1.0,  1.0, -1.0, -1.0,  0.0,  0.0,  0.0,  0.0,  0.0},
    {-1.0, -1.0, -1.0, -1.0,  0.0,  0.0,  0.0,  1.0,  0.0},
    {-1.0, -1.0,  1.0, -1.0,  0.0,  0.0,  1.0,  1.0,  0.0},
    {-1.0,  1.0,  1.0, -1.0,  0.0,  0.0,  1.0,  0.0,  0.0},

    {-1.0, -1.0,  1.0, 0.0,  -1.0,  0.0,  0.0,  0.0,  0.0},
    {-1.0, -1.0, -1.0, 0.0,  -1.0,  0.0,  0.0,  1.0,  0.0},
    { 1.0, -1.0, -1.0, 0.0,  -1.0,  0.0,  1.0,  1.0,  0.0},
    { 1.0, -1.0,  1.0, 0.0,  -1.0,  0.0,  1.0,  0.0,  0.0},

    {1.0,  1.0, -1.0, 0.0,  1.0,  0.0,  0.0,  0.0,  0.0},
    {1.0,  1.0,  1.0, 0.0,  1.0,  0.0,  0.0,  1.0,  0.0},
    {-1.0,  1.0,  1.0, 0.0,  1.0,  0.0,  1.0,  1.0,  0.0},
    {-1.0,  1.0, -1.0, 0.0,  1.0,  0.0,  1.0,  0.0,  0.0},
};

float axes[]=
{
    0.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 1.0,
};

float axisColors[] =
{    
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
};

float lightAxisColor[] =
{
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
};

unsigned int indices[] = 
{
    0,  1,  2,  3,
    4,  5,  6,  7,
    8,  9, 10, 11,
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23
};

void updateFrame()
{
    if( !bPaused )
    {
        time += 1/60.0f;
    }

    myCamera.updateInput();
    debugOutputManager.update( time );

    entities[std::string("cube0")].setPosition( vec3( -2.0f) );

    updateDisplay();

    glutSwapBuffers();
    glutPostRedisplay();
}

void updateDisplay()
{
    glClearColor( 0.2f, 0.2f, 0.35f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //glBindFramebuffer( GL_FRAMEBUFFER, frameBufferID );
    glEnable( GL_DEPTH_TEST );
    //glClearDepth( 1 );
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    Renderer& renderer = Renderer::getRenderer();

    renderer.viewMatrixStack.getTop().loadIdentity();
    renderer.viewMatrixStack.loadMatrix( mat4x4( myCamera.getView().data() ) );

    const vec3& camPosition = myCamera.position();

    for( int i = 2; i < numOfLights; ++i )
    {
        float x = cos( time / 5 + i ) * 6.0f + 2;
        float y = cos( time / 4 ) * 2.0f;
        float z = sin( time / 5 + i ) * 2.0f;
        lights[i].setPosition( vec3( x, y, z ) );
    }

    /*lights[0].setDirection( vec3( 1, 0, 0 ).normalize() );
    lights[0].setType( Light::Directional );*/

    if( !bPaused )
    {
        lights[0].setPosition( myCamera.position() );
        lights[0].setDirection( ( myCamera.getNextPos( 0.1f ) - myCamera.position() ).normalize() );
        lights[0].getDirection().normalize();
        lights[0].setType(Light::Point);
        lights[0].attenuation = 1;
        lights[0].nearFalloff = 20;
        lights[0].farFalloff = 21;

        ++frame;
    }

    /*Material& myMaterial = renderer.getMaterialManager().getMaterial( "advanced" );

    glEnable( GL_DEPTH_TEST );
    myMaterial.setUniform( std::string( "time" ), Material::Uniformf, &time );
    myMaterial.setUniform( std::string("numberOfLights"), Material::Uniformi, &numOfLights );
    myMaterial.setUniform( std::string( "camera_position" ), Material::Uniformf, &(myCamera.position().x), 3 );
    myMaterial.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
    myMaterial.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );

    for( int i = 0; i < numOfLights; ++i )
    {
        std::stringstream ss;
        ss << "lights[" << i << "].position";
        myMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].getPosition().x), 3 );

        ss = std::stringstream();
        ss << "lights[" << i << "].direction";
        myMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].getDirection().x), 3 );

        ss = std::stringstream();
        ss << "lights[" << i << "].color";
        myMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].getColor().x), 4 );

        ss = std::stringstream();
        ss << "lights[" << i << "].attenuation";
        myMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].attenuation) );

        ss = std::stringstream();
        ss << "lights[" << i << "].aperture";
        myMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].aperture ) );

        ss = std::stringstream();
        ss << "lights[" << i << "].nearFalloff";
        myMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].nearFalloff ) );

        ss = std::stringstream();
        ss << "lights[" << i << "].farFalloff";
        myMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].farFalloff ) );
    }

    myMaterial.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), 0 );
    myMaterial.setVertexAttribPointer( std::string( "normal" ), 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 12 );
    myMaterial.setVertexAttribPointer( std::string( "texcoord" ), 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 24 );
    myMaterial.setVertexAttribPointer( std::string( "_tangent" ), 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 36 );
    myMaterial.setVertexAttribPointer( std::string( "_bitangent" ), 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 48 );
    myMaterial.setVertexAttribPointer( std::string( "_normal" ), 3, GL_FLOAT, GL_FALSE, sizeof(vertex_), (char *)NULL + 60 );

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    for( int i = 0; i < 2; ++i )
    {
        renderer.modelMatrixStack.pushMatrix();
        renderer.modelMatrixStack.translate( i * 4.0f, 0.0f, 0.0f );
        myMaterial.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );
        renderer.drawElements( myMaterial, GL_QUADS, 24, GL_UNSIGNED_INT, 0 );
        renderer.modelMatrixStack.popMatrix();
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );*/

    Material& myBasicMaterial = renderer.getMaterialManager().getMaterial( "basic" );

    for( auto iter = myModels[currentMesh].getMaterials().begin(); iter != myModels[currentMesh].getMaterials().end(); ++iter )
    {
        Material& modelMaterial = *iter;

        modelMaterial.setUniform( std::string("numberOfLights"), Material::Uniformi, &numOfLights );
        modelMaterial.setUniform( std::string( "camera_position" ), Material::Uniformf, &(myCamera.position().x), 3 );

        for( int i = 0; i < numOfLights; ++i )
        {
            std::stringstream ss;
            ss << "lights[" << i << "].position";
            modelMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].getPosition().x), 3 );

            ss = std::stringstream();
            ss << "lights[" << i << "].direction";
            modelMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].getDirection().x), 3 );

            ss = std::stringstream();
            ss << "lights[" << i << "].color";
            modelMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].getColor().x), 4 );

            ss = std::stringstream();
            ss << "lights[" << i << "].attenuation";
            modelMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].attenuation) );

            ss = std::stringstream();
            ss << "lights[" << i << "].aperture";
            modelMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].aperture ) );

            ss = std::stringstream();
            ss << "lights[" << i << "].nearFalloff";
            modelMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].nearFalloff ) );

            ss = std::stringstream();
            ss << "lights[" << i << "].farFalloff";
            modelMaterial.setUniform( ss.str(), Material::Uniformf, &(lights[i].farFalloff ) );
        }
    }

    myModels[currentMesh].render( frame / 2 );

    renderer.modelMatrixStack.pushMatrix();
    renderer.modelMatrixStack.getTop().scale( 10.0f, 10.0f, 10.0f );
    
    myBasicMaterial.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
    myBasicMaterial.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
    myBasicMaterial.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

    myBasicMaterial.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, 0, axes );
    myBasicMaterial.setVertexAttribPointer( std::string( "inColor" ), 3, GL_FLOAT, GL_FALSE, 0, axisColors );
    int useTex = 0;
    myBasicMaterial.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );
    
    glLineWidth( 3.0f );
    renderer.drawVertexArray( myBasicMaterial, GL_LINES, 0, 6 );

    renderer.modelMatrixStack.popMatrix();

    myBasicMaterial.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, 0, axes );
    myBasicMaterial.setVertexAttribPointer( std::string( "inColor" ), 3, GL_FLOAT, GL_FALSE, 0, lightAxisColor );
    myBasicMaterial.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );

    glLineWidth( 3.0f );

    //glDisable( GL_DEPTH_TEST );
    //for( int i = 0; i < 1; ++i )
    //{
    //    renderer.modelMatrixStack.pushMatrix();
    //    renderer.modelMatrixStack.loadIdentity();
    //    //renderer.modelMatrixStack.getTop().translate( lights[i].getPosition().x, lights[i].getPosition().y, lights[i].getPosition().z );
    //    renderer.modelMatrixStack.getTop().translate( units[i].getPosition().x, units[i].getPosition().y, 0 );
    //    //renderer.modelMatrixStack.getTop().scale( 0.25f, 0.25f, 0.25f );
    //    myBasicMaterial.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );
    //    myBasicMaterial.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
    //    myBasicMaterial.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );

    //    renderer.drawVertexArray( myBasicMaterial, GL_LINES, 0, 6 );

    //    renderer.modelMatrixStack.popMatrix();
    //}

    debugOutputManager.draw();

    glEnable( GL_BLEND );
    if( myCommandWindow->displayWindow )
    {
        myCommandWindow->drawWindow();
    }
}

void onKeyDown( unsigned char keyCode, int mouseX, int mouseY )
{
    if( keyCode == '`' )
    {
        myCommandWindow->displayWindow = !myCommandWindow->displayWindow;
        myCamera.bLockMouse = !myCamera.bLockMouse;
    }
    else if( keyCode == 27 )
    {
        exit( 0 );
    }
    else if( myCommandWindow->displayWindow )
    {
        myCommandWindow->input( keyCode );
    }
    else
    {
        myCamera.keys[keyCode] = 1;

        if( keyCode == 'r' )
        {
            time = 0;
        }
        else if( keyCode == 'p' )
        {
            bPaused = !bPaused;
        }
        else if( keyCode == 'c' )
        {
            myCamera.bLockMouse = !myCamera.bLockMouse;
            if( myCamera.bLockMouse )
            {
                glutSetCursor( GLUT_CURSOR_NONE );
            }
            else
            {
                glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
            }
        }
        else if( keyCode == '-' )
        {
            if( numOfLights > 0 )
                --numOfLights;
        }
        else if( keyCode == '+' )
        {
            if( numOfLights < 5 )
                ++numOfLights;
        }
    }
}

void onKeyUp( unsigned char keyCode, int mouseX, int mouseY )
{
    myCamera.keys[keyCode] = 0;
}

void onSpecialKeyDown( int keyCode, int mouseX, int mouseY )
{
    if( myCommandWindow->displayWindow )
    {
        switch( keyCode )
        {
        case GLUT_KEY_LEFT:
            myCommandWindow->input( 17 );
            break;

        case GLUT_KEY_UP:
            myCommandWindow->input( 18 );
            break;

        case GLUT_KEY_RIGHT:
            myCommandWindow->input( 19 );
            break;

        case GLUT_KEY_DOWN:
            myCommandWindow->input( 20 );
            break;

        case GLUT_KEY_HOME:
            myCommandWindow->input( 21 );
            break;

        case GLUT_KEY_END:
            myCommandWindow->input( 22 );
            break;
        }
    }

    switch( keyCode )
    {

    case GLUT_KEY_UP:
        ++frame;
        std::cout << frame << std::endl;
        break;

    case GLUT_KEY_PAGE_DOWN:
        if( ++currentMesh > 9 )
        {
            currentMesh = 0;
        }
        frame = 0;
        break;

    case GLUT_KEY_PAGE_UP:
        frame = 0;
        if( --currentMesh < 0 )
        {
            currentMesh = 9;
        }
        break;
    }
}

void onSpecialKeyUp( int keyCode, int mouseX, int mouseY )
{
    static bool wireFrame = false;
    switch( keyCode )
    {

    case GLUT_KEY_F4:
        if( glutGetModifiers() & GLUT_ACTIVE_ALT)
        {
            exit( 0 );
        }
        break;

    case GLUT_KEY_F3:
        wireFrame = !wireFrame;
        if ( wireFrame ) 
        {
            glPointSize( 50 );
            glPolygonMode( GL_FRONT, GL_LINE );
        }

        else
        {
            glPolygonMode( GL_FRONT, GL_FILL );
        }

        break;
    }
}

void onMouseButton( int whichButton, int state, int mouseX, int mouseY )
{
    myCamera.updateMousePos( vec2( (float)mouseX, (float)mouseY ) );
}

void onMouseDrag( int mouseX, int mouseY )
{
    myCamera.updateMousePos( vec2( (float)mouseX, (float)mouseY ) );
}

void onMouseMove( int mouseX, int mouseY )
{
    myCamera.updateMousePos( vec2( (float)mouseX, (float)mouseY ) );
}

void listCommands( int argc, std::string* argv )
{
    myCommandWindow->help();
}

void quit( int argc, std::string* argv )
{
    exit( 0 );
}

void toggleShaderOption( int argc, std::string* argv )
{
    int command = 0;
    Material& myMaterial = Renderer::getRenderer().getMaterialManager().getMaterial( "advanced" );
    
    if( (argv[0]).compare( "diffuse" ) == 0 )
    {
        shaderDebugInt ^= useDiffuse;
        myMaterial.setUniform( std::string( "debug" ), Material::Uniformi, &shaderDebugInt );
    }

    else if( (argv[0]).compare( "lighting" ) == 0  )
    {
        shaderDebugInt ^= useLighting;
        myMaterial.setUniform( std::string( "debug" ), Material::Uniformi, &shaderDebugInt );
    }

    else if( (argv[0]).compare( "specular" ) == 0  )
    {
        shaderDebugInt ^= useSpecular;
        myMaterial.setUniform( std::string( "debug" ), Material::Uniformi, &shaderDebugInt );
    }

    else if( (argv[0]).compare( "emissive" ) == 0  )
    {
        shaderDebugInt ^= useEmissive;
        myMaterial.setUniform( std::string( "debug" ), Material::Uniformi, &shaderDebugInt );
    }

    else if( (argv[0]).compare( "fog" ) == 0  )
    {
        shaderDebugInt ^= useFog;
        myMaterial.setUniform( std::string( "debug" ), Material::Uniformi, &shaderDebugInt );
    }

    else if( (argv[0]).compare( "parallax" ) == 0  )
    {
        shaderDebugInt ^= useParallax;
        myMaterial.setUniform( std::string( "debug" ), Material::Uniformi, &shaderDebugInt );
    }
}

void addDebugOutput( int argc, std::string* argv )
{
    if( argc )
    {
        if( argv[0].compare( "line" ) == 0 )
        {
            StringParser sp( argv[1], std::string( "," ) );
            vec3 startPos( (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ) );

            sp = StringParser( argv[2], std::string(",") );
            vec3 endPos( (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ) );
            
            float timeToLive = (float)atof( argv[3].c_str() );

            debugOutputManager.addDebugLine( startPos, endPos, timeToLive );
        }

        else if( argv[0].compare( "point" ) == 0 )
        {
            StringParser sp( argv[1], std::string( "," ) );
            vec3 position( (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ) );

            sp = StringParser( argv[2], std::string(",") );
            float yaw = SBUtil::degreeToRadians( (float)atof( sp.getNextToken().c_str() ) );
            float pitch = SBUtil::degreeToRadians( (float)atof( sp.getNextToken().c_str() ) );
            float roll = SBUtil::degreeToRadians( (float)atof( sp.getNextToken().c_str() ) );

            mat3x3 orientation;
            orientation.fromEulerAngles( yaw, pitch, roll );

            float timeToLive = (float)atof( argv[3].c_str() );
            
            debugOutputManager.addDebugPoint( position, orientation, timeToLive );
        }

        else if( argv[0].compare( "cube" ) == 0 )
        {
            StringParser sp( argv[1], std::string( "," ) );
            vec3 position( (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ) );

            sp = StringParser( argv[2], std::string( "," ) );
            vec3 axis( (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ), (float)atof( sp.getNextToken().c_str() ) );

            float size = (float)atof( argv[3].c_str() );

            float timeToLive = (float)atof( argv[4].c_str() );

            debugOutputManager.addDebugCube( position, axis, size, timeToLive );
        }

        else if( argv[0].compare( "tag" ) == 0 )
        {
            float timeToLive = (float)atof( argv[2].c_str() );

            debugOutputManager.addDebugHighlight( entities[argv[1]], timeToLive );
        }
    }
}

void computeTagentSpaceMatrices()
{
    computeTangentSpaceMatrix(vertices[0], vertices[1], vertices[2]);
    computeTangentSpaceMatrix(vertices[1], vertices[2], vertices[3]);
    computeTangentSpaceMatrix(vertices[2], vertices[3], vertices[0]);
    computeTangentSpaceMatrix(vertices[3], vertices[0], vertices[1]);

    computeTangentSpaceMatrix(vertices[4], vertices[5], vertices[6]);
    computeTangentSpaceMatrix(vertices[5], vertices[6], vertices[7]);
    computeTangentSpaceMatrix(vertices[6], vertices[7], vertices[4]);
    computeTangentSpaceMatrix(vertices[7], vertices[4], vertices[5]);

    computeTangentSpaceMatrix(vertices[8], vertices[9], vertices[10]);
    computeTangentSpaceMatrix(vertices[9], vertices[10], vertices[11]);
    computeTangentSpaceMatrix(vertices[10], vertices[11], vertices[8]);
    computeTangentSpaceMatrix(vertices[11], vertices[8], vertices[9]);

    computeTangentSpaceMatrix(vertices[12], vertices[13], vertices[14]);
    computeTangentSpaceMatrix(vertices[13], vertices[14], vertices[15]);
    computeTangentSpaceMatrix(vertices[14], vertices[15], vertices[12]);
    computeTangentSpaceMatrix(vertices[15], vertices[12], vertices[13]);

    computeTangentSpaceMatrix(vertices[16], vertices[17], vertices[18]);
    computeTangentSpaceMatrix(vertices[17], vertices[18], vertices[19]);
    computeTangentSpaceMatrix(vertices[18], vertices[19], vertices[16]);
    computeTangentSpaceMatrix(vertices[19], vertices[16], vertices[17]);

    computeTangentSpaceMatrix(vertices[20], vertices[21], vertices[22]);
    computeTangentSpaceMatrix(vertices[21], vertices[22], vertices[23]);
    computeTangentSpaceMatrix(vertices[22], vertices[23], vertices[20]);
    computeTangentSpaceMatrix(vertices[23], vertices[20], vertices[21]);
}

void materialSetup()
{
    MaterialManager& materialManager = Renderer::getRenderer().getMaterialManager();

    GLuint programID = materialManager.createOrGetProgram( materialManager.createOrGetShader( "VertexShader.glsl", GL_VERTEX_SHADER ),
        materialManager.createOrGetShader( "PixelShader.glsl", GL_FRAGMENT_SHADER ) );
    materialManager.createMaterial( "advanced", programID );

    programID = materialManager.createOrGetProgram( materialManager.createOrGetShader( "BasicVertexShader.glsl", GL_VERTEX_SHADER ),
        materialManager.createOrGetShader( "BasicFragmentShader.glsl", GL_FRAGMENT_SHADER ) );
    materialManager.createMaterial( "basic", programID );

    programID = materialManager.createOrGetProgram( materialManager.createOrGetShader( "FontVertexShader.glsl", GL_VERTEX_SHADER ),
        materialManager.createOrGetShader( "FontFragmentShader.glsl", GL_FRAGMENT_SHADER ) );
    materialManager.createMaterial( "fiery", programID );

    programID = materialManager.createOrGetProgram( materialManager.createOrGetShader( "FrameBufferVertexShader.glsl", GL_VERTEX_SHADER ),
        materialManager.createOrGetShader( "FrameBufferFragShader.glsl", GL_FRAGMENT_SHADER ) );
    materialManager.createMaterial( "framebuffer", programID );

    modelProgramID = materialManager.createOrGetProgram( materialManager.createOrGetShader( "ModelVertexShader.glsl", GL_VERTEX_SHADER ),
        materialManager.createOrGetShader( "ModelFragmentShader.glsl", GL_FRAGMENT_SHADER ) );
    materialManager.createMaterial( "model", modelProgramID );

    Material& material = materialManager.getMaterial( "advanced" );

    if( &material )
    {
        Texture *diffuseTex = Texture::CreateOrGetTexture( std::string( "cobblestonesDiffuse.png" ) );
        Texture *normalTex = Texture::CreateOrGetTexture( std::string( "cobblestonesNormal.png" ) );
        Texture *emissiveTex = Texture::CreateOrGetTexture( std::string( "cobblestonesEmissive.png" ) );
        Texture *specularTex = Texture::CreateOrGetTexture( std::string( "cobblestonesSpecular.png" ) );
        Texture *heightTex = Texture::CreateOrGetTexture( std::string( "cobblestonesHeight.png" ) );

        material.setTexture( std::string( "diffuseTex" ), GL_TEXTURE0, diffuseTex->getTextureID() );
        material.setTexture( std::string( "normalTex" ), GL_TEXTURE1, normalTex->getTextureID() );
        material.setTexture( std::string( "emissiveTex" ), GL_TEXTURE2, emissiveTex->getTextureID() );
        material.setTexture( std::string( "specularTex" ), GL_TEXTURE3, specularTex->getTextureID() );
        material.setTexture( std::string( "heightTex" ), GL_TEXTURE4, heightTex->getTextureID() );
    }

    glGenBuffers(1, &vbo_vertices);
    glGenBuffers(1, &vbo_indices);

    computeTagentSpaceMatrices();
}

int main( int argc, char** argv )
{
    setupGLUT( argc, argv );
    myCamera.setScreenCenter( glutGet( GLUT_WINDOW_WIDTH ) / 2, glutGet( GLUT_WINDOW_HEIGHT ) / 2 );

    materialSetup();

    myModels[0].loadModel( "Models\\01-singlepoly.MY3D", modelProgramID );
    myModels[1].loadModel( "Models\\02-boxes.MY3D", modelProgramID );
    myModels[2].loadModel( "Models\\03-obj.MY3D", modelProgramID );
    myModels[3].loadModel( "Models\\04-cube-6-materials.MY3D", modelProgramID );
    myModels[4].loadModel( "Models\\05-sibenik.MY3D", modelProgramID );
    myModels[5].loadModel( "Models\\06-looney-level.MY3D", modelProgramID );
    myModels[6].loadModel( "Models\\07-scaling-box.MY3D", modelProgramID );
    myModels[7].loadModel( "Models\\08-skinned-tube.MY3D", modelProgramID );
    myModels[8].loadModel( "Models\\09-zax_mesh.MY3D", modelProgramID );
    myModels[9].loadModel( "Models\\10-capstone-char.MY3D", modelProgramID );
    
    myFont.loadFont( "MainFont_EN_00.png", "MainFont_EN.FontDef.xml" );
    
    myCommandWindow = new CommandWindow( myFont );
    myCommandWindow->registerCommand( "quit", quit, "Exits the program" );
    myCommandWindow->registerCommand( "toggle", toggleShaderOption, "Toggles Shader Options" );
    myCommandWindow->registerCommand( "help", listCommands, "Lists all commands" );
    myCommandWindow->registerCommand( "debug", addDebugOutput, "Adds new debug outputs" );

    /*for( int i = 0; i < 5; ++i )
    {
        for( int j = 0; j < 5; ++j )
        {
            for( int k = 0; k < 5; ++k )
            {
                Entity e;
                e.setPosition( vec3( i * 2.0f, j * 2.0f, k * 2.0f ) );
                std::stringstream ss;
                ss << "cube" << i << j << k;
                entities.insert( std::pair< std::string, Entity >( ss.str(), e ) );
            }
        }
    }*/
    //entities.insert( std::pair< std::string, Entity >( "cube0", Entity() ));

    /*glGenFramebuffers( 1, &frameBufferID );
    frameBufferTexture.LoadFromMemory( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, texels );
    
    glGenTextures( 1, &depthTextureID );

    glBindTexture(GL_TEXTURE_2D, depthTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 768, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );


    glBindFramebuffer( GL_FRAMEBUFFER, frameBufferID );
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture.getTextureID(), 0 );
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );*/

    /*debugOutputManager.addDebugLine( vec3(), vec3( 10.0f ), 10.0f );
    debugOutputManager.addDebugPoint( vec3( 2.0f ), mat3x3(), 10.0f );
    debugOutputManager.addDebugCube( vec3(), vec3(), 2.0, 10.0f );
    debugOutputManager.addDebugHighlight( entities[std::string("cube0")], 10.0f );*/
    int color = 0;
    for( int i = 0; i < 5; ++i )
    {
        if( color == 0 )
            lights[i].setColor( vec3( 1.0f, 0.0f, 0.0f ) );
        else if( color == 1 )
            lights[i].setColor( vec3( 0.0f, 1.0f, 0.0f ) );
        else if( color == 2 )
            lights[i].setColor( vec3( 0.0f, 0.0f, 1.0f ) );
        lights[i].setType(Light::Point);
        color = (color + 1) % 3;
    }
    lights[0].setColor( vec3( 1.0f, 1.0f, 1.0f ) );

    glutMainLoop();

    delete myCommandWindow;

    return 0;
}

void setupGLUT( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE );
    glutInitWindowSize( 1152, 864 );
    glutInitWindowPosition( 1920 / 2 - 576, 1080 / 2 - 432 );
    glutCreateWindow( "Shader" );
    glutSetCursor( GLUT_CURSOR_NONE );

    glewInit();

    Renderer::getRenderer().projectionMatrixStack.getTop().perspectiveProjection( 45.0f, WINDOW_ASPECT_RATIO, 0.1f, 100000.0f );

    glutDisplayFunc( updateFrame );
    glutKeyboardFunc( onKeyDown );
    glutKeyboardUpFunc( onKeyUp );
    glutSpecialFunc( onSpecialKeyDown );
    glutSpecialUpFunc( onSpecialKeyUp );
    glutMouseFunc( onMouseButton );
    glutMotionFunc( onMouseDrag );
    glutPassiveMotionFunc( onMouseMove );

    glEnable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );
    //glEnable( GL_CULL_FACE );
    //glFrontFace( GL_CW );
}