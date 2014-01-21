#pragma once

#include <vector>
#include <map>
#include <string>
#include <math.h>
#include "vec3.h"

namespace SBAI
{
    class AStarPathData;

    class AStarNode
    {

    public:

        AStarNode( const std::string& nodeTypename );
        virtual ~AStarNode(){};

        void generatePath( AStarNode* destination, std::vector< AStarNode* >& pathingResult );
        void generatePath( AStarNode* destination, AStarPathData* data, std::vector< AStarNode* >& pathingResult );

        int getID() const { return m_nodeID; }
        bool setID( int nodeID )
        { 
            if( m_allAStarNodes.find( nodeID ) == m_allAStarNodes.end() )
            {
                m_allAStarNodes.erase( m_nodeID );
                m_nodeID = nodeID;
                m_allAStarNodes[nodeID] = this;
                return true;
            }
            return false;
            
        }

        void open( int pathID ){ m_lastOpenedByPathID = pathID; }
        bool isOpenedBy( int pathID ) const { return m_lastOpenedByPathID == pathID; }
        
        void close( int pathID ) { m_lastClosedByPathID = pathID; }
        bool isClosedBy( int pathID ) const { return m_lastClosedByPathID == pathID; }

        void setDefaultGCost( float g ){ m_defaultGCosts[m_nodeTypeID] = g; }
        float getDefaultGCost() const { return m_defaultGCosts[m_nodeTypeID]; }

        void setNodeTypeID( const std::string& nodeTypename )
        {
            auto nodeIDIter = m_nodeTypeIDs.find( nodeTypename );

            if( nodeIDIter != m_nodeTypeIDs.end() )
            {
                m_nodeTypeID = nodeIDIter->second;
            }
            else
            {
                m_nodeTypeID = m_nextTypeID;
                m_nodeTypeIDs[ nodeTypename ] = m_nodeTypeID;
                ++m_nextTypeID;
                m_defaultGCosts.push_back( 1.0f );
            }
        }

        int getNodeTypeID() const { return m_nodeTypeID; }

        void setG( float g ){ m_g = g; }
        float getG() const { return m_g; }

        void setH( float h ){ m_h = h; }
        float getH() const { return m_h; }

        float getF() const { return m_g + m_h; }

        void setParent( AStarNode* parent ){ m_parent = parent; }
        AStarNode* getParent() const { return m_parent; }

        void addNeighbor( AStarNode* neighbor )
        {
            bool alreadyANeighbor = false;
            unsigned int neighborSize = m_neighbors.size();

            for( unsigned int i = 0; i < neighborSize; ++i )
            {
                if( m_neighbors[i] == neighbor )
                {
                    alreadyANeighbor = true;
                    break;
                }
            }

            if( !alreadyANeighbor )
            {
                m_neighbors.push_back( neighbor ); 
            }

        }

        void removeNeighbor( AStarNode* exneighbor )
        {
            unsigned int neighborSize = m_neighbors.size();
            for( unsigned int i = 0; i < neighborSize; ++i )
            {
                if( m_neighbors[i] == exneighbor )
                {
                    m_neighbors[i] = m_neighbors.back();
                    m_neighbors.pop_back();
                }
            }
        }

        void clearNeighbors(){ m_neighbors.clear(); }

        const std::vector< AStarNode* >& getNeighbors(){ return m_neighbors; }

        virtual void setPosition( const SBMath::vec3& position ){ m_position = position; }
        const SBMath::vec3& getPosition() const { return m_position; }

        const std::map< int, AStarNode* >& getAllAStarNodes() const { return m_allAStarNodes; }

        int getManhattanDistance( const SBMath::vec3& a, const SBMath::vec3& b ) const
        {
            return (int)( abs( b.x - a.x ) + abs( b.y - a.y ) + abs( b.z - a.z ) );
        }

        void insertNode( AStarNode* node, std::vector< AStarNode* >& openNodes, int pathID );
        void editNode( AStarNode* node, std::vector< AStarNode* >& openNodes );

        bool hasStraightPathTo( AStarNode* destination, const SBMath::vec3& velocity, float collisionRadius = 1.0f ) const;

        float getDistanceToNextAxis( float position, float direction ) const
        {
            direction /= abs( direction );
            return (float)( ( (int)position + direction ) - position );
        }

    protected:

        int m_lastClosedByPathID;
        int m_lastOpenedByPathID;
        int m_nodeID;
        int m_nodeTypeID;

        static int m_nextTypeID;

        float m_g;
        float m_h;

        SBMath::vec3 m_position;

        AStarNode* m_parent;

        std::vector< AStarNode* > m_neighbors;        
        static std::vector< float > m_defaultGCosts;
        static std::map< std::string, int > m_nodeTypeIDs;
        static std::map< int, AStarNode* > m_allAStarNodes;

    };
}