#pragma once

#include <map>

namespace SBAI
{
    class AStarPathData
    {

    public:

        AStarPathData()
            :   m_defaultGCost( 1.0f )
        {}

        AStarPathData( float defaultGCost )
            :   m_defaultGCost( defaultGCost )
        {}

        void setGCostForNode( float gCost, int nodeID ){ m_GCostForNodeType[nodeID] = gCost; }

        float getGCost( int nodeTypeID ) const 
        {
            auto gCostIter = m_GCostForNodeType.find( nodeTypeID );

            if( gCostIter != m_GCostForNodeType.end() )
            {
                return gCostIter->second;
            }

            return m_defaultGCost;
        }

    private:

        std::map< int, float > m_GCostForNodeType;

        float m_defaultGCost;

    };
}