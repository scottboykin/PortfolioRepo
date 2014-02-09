#include "DebugOutputManager.h"
#include "DebugOutput.h"
#include "DebugPath.h"
#include "DebugPoint.h"
#include "DebugBox.h"
#include "DebugEntity.h"
#include "Entity.h"

namespace SBGraphics
{
    DebugOutputManager::DebugOutputManager()
        :   m_currentSimTime( 0.0 )
    {}

    DebugOutputManager::~DebugOutputManager()
    {
        for( auto iter = m_debugOutputs.begin(); iter != m_debugOutputs.end(); )
        {
            delete (*iter);
            iter = m_debugOutputs.erase( iter );
        }
    }

    void DebugOutputManager::update( float currentSimTime )
    {
        m_currentSimTime = currentSimTime;

        for( auto iter = m_debugOutputs.begin(); iter != m_debugOutputs.end(); )
        {
            (*iter)->update( currentSimTime );

            if( (*iter)->wantsToDie() )
            {
                delete (*iter);
                iter = m_debugOutputs.erase( iter );
            }

            else
            {
                ++iter;
            }
        }
    }

    void DebugOutputManager::draw()
    {
        for( auto iter = m_debugOutputs.begin(); iter != m_debugOutputs.end(); ++iter )
        {
            (*iter)->draw();
        }
    }

    void DebugOutputManager::drawDebugLine( SBMath::vec3& startPosition, SBMath::vec3& endPosition )
    {
        DebugPath debugPath( startPosition, endPosition );
        debugPath.draw();
    }

    void DebugOutputManager::drawDebugPoint( SBMath::vec3& position, SBMath::mat3x3& orientation )
    {
        DebugPoint debugPoint( position, orientation );
        debugPoint.draw();
    }

    void DebugOutputManager::drawDebugCube( SBMath::vec3& position, SBMath::vec3& axis, float size )
    {
        DebugBox debugBox( position, axis, size );
        debugBox.draw();
    }

    void DebugOutputManager::drawDebugHighlight( Entity& entity )
    {
        DebugEntity debugEntity( entity );
        debugEntity.draw();
    }

    void DebugOutputManager::addDebugLine( SBMath::vec3& startPosition, SBMath::vec3& endPosition, float timeToDie )
    {
        m_debugOutputs.push_back( new DebugPath( startPosition, endPosition, m_currentSimTime + timeToDie ) );
    }

    void DebugOutputManager::addDebugPoint( SBMath::vec3& position, SBMath::mat3x3& orientation, float timeToDie )
    {
        m_debugOutputs.push_back( new DebugPoint( position, orientation, m_currentSimTime + timeToDie ) );
    }

    void DebugOutputManager::addDebugCube( SBMath::vec3& position, SBMath::vec3& axis, float size, float timetoDie )
    {
        m_debugOutputs.push_back( new DebugBox( position, axis, size, m_currentSimTime + timetoDie ) );
    }

    void DebugOutputManager::addDebugHighlight( Entity& entity, float timeToDie )
    {
        m_debugOutputs.push_back( new DebugEntity( entity, m_currentSimTime + timeToDie ) );
    }
}