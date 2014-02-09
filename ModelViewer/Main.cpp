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

float axis[] =
{
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

float axisColors[] =
{
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f
};

void updateFrame()
{
    if( !bPaused )
    {
        time += 1/60.0f;
    }

    myCamera.updateInput();
    debugOutputManager.update( time );

    updateDisplay();

    glutSwapBuffers();
    glutPostRedisplay();
}

void updateDisplay()
{
    glClearColor( 0.2f, 0.2f, 0.35f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    Renderer& renderer = Renderer::getRenderer();

    renderer.viewMatrixStack.getTop().loadIdentity();
    renderer.viewMatrixStack.loadMatrix( mat4x4( myCamera.getView().data() ) );

    const vec3& camPosition = myCamera.position();

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

    Material& myBasicMaterial = renderer.getMaterialManager().getMaterial( "basic" );
    
    myBasicMaterial.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
    myBasicMaterial.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
    myBasicMaterial.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

    myBasicMaterial.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, 0, axis );
    myBasicMaterial.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, 0, axisColors );
    int useTex = 0;
    myBasicMaterial.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );
    
    glLineWidth( 3.0f );
    renderer.drawVertexArray( myBasicMaterial, GL_LINES, 0, 6 );

    renderer.modelMatrixStack.popMatrix();

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
    }
}

void materialSetup()
{
    MaterialManager& materialManager = Renderer::getRenderer().getMaterialManager();

    int programID = materialManager.createOrGetProgram( materialManager.createOrGetShader( "BasicVertexShader.glsl", GL_VERTEX_SHADER ),
        materialManager.createOrGetShader( "BasicFragmentShader.glsl", GL_FRAGMENT_SHADER ) );
    materialManager.createMaterial( "basic", programID );

    modelProgramID = materialManager.createOrGetProgram( materialManager.createOrGetShader( "ModelVertexShader.glsl", GL_VERTEX_SHADER ),
        materialManager.createOrGetShader( "ModelFragmentShader.glsl", GL_FRAGMENT_SHADER ) );
    materialManager.createMaterial( "model", modelProgramID );
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
    glutCreateWindow( "ModelViewer" );
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
}