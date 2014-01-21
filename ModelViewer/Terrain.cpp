#include <iostream>
#include "Terrain.h"
#include "XMLReader.h"
#include "Renderer.h"
#include "Material.h"
#include "MaterialManager.h"
#include "vec4.h"

using namespace SBMath;
using namespace SBUtil;
using namespace SBGraphics;

namespace SBGame
{
    Terrain::Terrain()
        :   m_name( 0 )
        ,   m_movementModifier( 1.0 )
        ,   m_blocksBuilding( false )
    {}

    Terrain::Terrain( TerrainBlueprint& blueprint  )
        :   m_name( &blueprint.getName() )
        ,   m_movementModifier( blueprint.getMovementModifer() )
        ,   m_blocksBuilding( blueprint.getBlocksBuild() )
        ,   m_color( blueprint.getColor() )
    {}

    void Terrain::render( int x, int y )
    {
        vec4 colorAsFloats = m_color.getFloatColor();

        float vertices[] =
        {
            0, 0, 0,
            1, 0, 0,
            0, 1, 0,
            0, 1, 0,
            1, 0, 0,
            1, 1, 0
        };

        float color[] =
        {
            colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w,
            colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w,
            colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w,
            colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w,
            colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w,
            colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w
        };

        Renderer& renderer = Renderer::getRenderer();
        Material& material = renderer.getMaterialManager().getMaterial( "basic" );

        renderer.modelMatrixStack.pushMatrix();

        renderer.modelMatrixStack.loadIdentity();
        renderer.modelMatrixStack.translate( (float)x, (float)y, 0 );

        material.setUniform( std::string( "Projection" ), Material::UniformMatrix4f, renderer.projectionMatrixStack.getTop().data() );
        material.setUniform( std::string( "View" ), Material::UniformMatrix4f, renderer.viewMatrixStack.getTop().data() );
        material.setUniform( std::string( "Model" ), Material::UniformMatrix4f, renderer.modelMatrixStack.getTop().data() );

        material.setVertexAttribPointer( std::string( "vertex" ), 3, GL_FLOAT, GL_FALSE, 0, vertices );
        material.setVertexAttribPointer( std::string( "inColor" ), 4, GL_FLOAT, GL_FALSE, 0, color );
        int useTex = 0;
        material.setUniform( std::string( "useTexture" ), Material::Uniformi, &useTex );

        renderer.drawVertexArray( material, GL_TRIANGLES, 0, 6 );

        renderer.modelMatrixStack.popMatrix();

    }
}