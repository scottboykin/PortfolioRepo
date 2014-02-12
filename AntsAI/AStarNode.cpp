#include "AStarNode.h"
#include "AStarPathData.h"

using namespace SBMath;

namespace SBAI
{
    int AStarNode::m_nextTypeID = 0;
    std::vector< float > AStarNode::m_defaultGCosts;
    std::map< int, AStarNode* > AStarNode::m_allAStarNodes;
    std::map< std::string, int > AStarNode::m_nodeTypeIDs;

    AStarNode::AStarNode( const std::string& nodeTypename )
        :    m_lastClosedByPathID( -1 )
        ,    m_lastOpenedByPathID( -1 )
        ,    m_nodeID( -1 )
        ,    m_nodeTypeID( -1 )
        ,    m_g( 0.0f )
        ,    m_h( 0.0f )        
        ,    m_parent( 0 )
    {
        setNodeTypeID( nodeTypename );
        m_allAStarNodes[m_nodeID] = this;
    }

    void AStarNode::generatePath( AStarNode* destination, std::vector< AStarNode* >& pathingResult )
    {
        AStarPathData data;
        for( int i = 0; i < m_nextTypeID; ++i )
        {
            if( i < m_defaultGCosts.size() )
                data.setGCostForNode( m_defaultGCosts[i], i );
        }
        generatePath( destination, &data, pathingResult );
    }

    void AStarNode::generatePath( AStarNode* destination, AStarPathData* data, std::vector< AStarNode* >& pathingResult )
    {
        static int pathID = 0;

        if( destination )
        {
            std::vector< AStarNode* > openNodes;
            AStarNode* bestPossiblePath = this;

            m_parent = 0;
            m_g = 0;
            m_h = (float)getManhattanDistance( m_position, destination->getPosition() );
            insertNode( this, openNodes, pathID );

            while( openNodes.size() > 0 )
            {
                AStarNode* currentNode = openNodes.back();
                openNodes.pop_back();
                currentNode->m_lastClosedByPathID = pathID;

                if( currentNode == destination )
                {
                    bestPossiblePath = currentNode;
                    break;
                }

                else
                {
                    const std::vector< AStarNode* >& neighbors = currentNode->getNeighbors();

                    for( unsigned int i = 0; i < neighbors.size(); ++i )
                    {
                        AStarNode* neighbor = neighbors[i];
                        float neighborGCost = data->getGCost( neighbor->getNodeTypeID() );
                        if( neighborGCost > 0.0f )
                        {
                            float totalGCost = currentNode->getG() + neighborGCost;

                            if( neighbor->isClosedBy( pathID ) && totalGCost > neighbor->getG() )
                            {
                                continue;
                            }

                            if( !neighbor->isOpenedBy( pathID ) || totalGCost < neighbor->getG() )
                            {
                                neighbor->setParent( currentNode );
                                neighbor->setG( totalGCost );
                                neighbor->setH( (float)getManhattanDistance( neighbor->getPosition(), destination->getPosition() ) );

                                if( neighbor->getH() < bestPossiblePath->getH() )
                                    bestPossiblePath = neighbor;

                                if( neighbor->isOpenedBy( pathID ) )
                                    editNode( neighbor, openNodes );
                                else
                                    insertNode( neighbor, openNodes, pathID );
                            }
                        }
                    }
                }
            }


            while( bestPossiblePath->getParent() )
            {
                pathingResult.push_back( bestPossiblePath );
                bestPossiblePath = bestPossiblePath->getParent();
            }
        }

        ++pathID;
    }

    void AStarNode::insertNode( AStarNode* node, std::vector< AStarNode* >& openNodes, int pathID )
    {
        openNodes.push_back( node );
        node->open( pathID );

        for( int i = openNodes.size() - 1; i > 0; --i )
        {
            if( openNodes[i]->getF() > openNodes[i - 1]->getF() )
            {
                AStarNode* largerF = openNodes[i];
                openNodes[i] = openNodes[i -1];
                openNodes[i - 1] = largerF;
            }
            else
            {
                break;
            }
        }
    }

    void AStarNode::editNode( AStarNode* node, std::vector< AStarNode* >& openNodes )
    {
        bool nodeFound = false;
        for( unsigned int i = openNodes.size() - 1; i >= 0 && i + 1 < openNodes.size(); )
        {
            if( nodeFound )
            {
                if( openNodes[i]->getF() < openNodes[i+1]->getF() )
                {
                    AStarNode* smallerNode = openNodes[i];
                    openNodes[i] = openNodes[i+1];
                    openNodes[i+1] = smallerNode;
                    ++i;
                }
                else
                {
                    break;
                }
            }
            else
            {
                if( node == openNodes[i] )
                {
                    nodeFound = true;
                }
                else
                {
                    --i;
                }
            }
        }
    }

    bool AStarNode::hasStraightPathTo( AStarNode* destination, const SBMath::vec3& velocity, float collisionRadius ) const
    {
        if( velocity == vec3() )
            return false;

        vec3 currentPosition = m_position;
        bool destinationReached = false;

        while( m_position.squaredDistance( currentPosition ) < m_position.squaredDistance( destination->getPosition() ) )
        {
            int nextInterceptAxis = -1;
            float timeToNextIntercept = 10000000;

            for( int axis = 0; axis < 3; ++axis )
            {
                if( velocity[axis] == 0 )
                {
                    continue;
                }

                if( nextInterceptAxis == -1 )
                {
                    nextInterceptAxis = axis;
                    timeToNextIntercept = getDistanceToNextAxis( currentPosition[axis], velocity[axis] ) / velocity[axis];
                    continue;
                }

                float timeToNextAxis = getDistanceToNextAxis( currentPosition[axis], velocity[axis] ) / velocity[axis];
                if( timeToNextAxis < timeToNextIntercept )
                {
                    nextInterceptAxis = axis;
                    timeToNextIntercept = timeToNextAxis;
                }
            }

            for( int axis = 0; axis < 3; ++axis )
            {
                currentPosition[axis] += velocity[axis] * timeToNextIntercept;
            }

            vec3 checkPos = currentPosition;

            if( velocity.x > 0 )
            {
                ++checkPos.y;
                if( velocity.y < 0 )
                {
                    ++checkPos.x;
                }
            }

            else
            {
                ++checkPos.x;
                if( velocity.y > 0 )
                {
                    ++checkPos.y;
                }
            }

            if( m_allAStarNodes[ (int)checkPos.y * 50 + (int)checkPos.x ]->getDefaultGCost() == 0.0f )
                return false;
        }


        return true;
    }
}