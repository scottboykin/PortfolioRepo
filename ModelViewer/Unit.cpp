#include <iostream>
#include "Unit.h"
#include "XMLReader.h"
#include "Color.h"
#include "vec4.h"
#include "Renderer.h"
#include "MaterialManager.h"
#include "Material.h"

using namespace SBUtil;
using namespace SBMath;
using namespace SBGraphics;

namespace SBGame
{
    Unit::Unit( UnitBlueprint& blueprint, char teamNum, vec2 position )
        :   m_name( &blueprint.getName() )
        ,   m_goldCost( blueprint.getGoldCost() )
        ,   m_stoneCost( blueprint.getStoneCost() )
        ,   m_timeToBuild( blueprint.getTimeToBuild() )
        ,   m_maxHealth( blueprint.getMaxHealth() )
        ,   m_currentHealth( blueprint.getMaxHealth() )
        ,   m_damage( blueprint.getDamage() )
        ,   m_armor( blueprint.getArmor() )
        ,   m_moveSpeed( blueprint.getMoveSpeed() )
        ,   m_damageType( blueprint.getDamageType() )
        ,   m_armorType( blueprint.getArmorType() )
        ,   m_position( position )
        ,   m_destination( position )
        ,   m_state( AI_IDLE )
        ,   m_teamNum( teamNum )
    {
        for( int i = 0; i < 4; ++i )
        {
            m_abilitiesNames[i] = &blueprint.getAbilityName( i );
        }
    }

    void Unit::update( float dt )
    {
        vec2 direction;

        switch( m_state )
        {
            case AI_IDLE:
                break;

            case AI_MOVE:
                direction = ( m_destination - m_position ).normalize();

                if( m_destination.squaredDistance( m_position ) < m_destination.squaredDistance( direction * m_moveSpeed ) )
                {
                    m_position = m_destination;
                    m_state = AI_IDLE;
                }

                else
                {
                    m_position += direction * m_moveSpeed;
                }
                break;

            case AI_HARVEST:
                break;

        }

    }

    void Unit::render()
    {
        vec4 colorAsFloats = Color().getFloatColor();

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
        renderer.modelMatrixStack.translate( m_position.x, m_position.y, 0 );

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