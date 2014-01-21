#pragma once

#include <map>
#include <vector>
#include <string>
#include "EventSubscriber.h"
#include "NamedProperty.h"

namespace SBUtil
{
    class EventSystem
    {

    public:

        EventSystem()
        {}

        void FireEvent( std::string& eventName )
        {
            NamedProperty properties;

            auto eventSubscribers = m_subscribers.find( eventName );

            if( eventSubscribers == m_subscribers.end() )
                return;

            std::vector< EventSubscriberBase* > subscribers = eventSubscribers->second;

            for( unsigned int i = 0; i < subscribers.size(); ++i )
            {
                subscribers[i]->Execute( properties );
            }
        }

        void FireEvent( std::string& eventName, NamedProperty& properties )
        {
            auto eventSubscribers = m_subscribers.find( eventName );

            if( eventSubscribers == m_subscribers.end() )
                return;

            std::vector< EventSubscriberBase* > subscribers = eventSubscribers->second;
            
            for( unsigned int i = 0; i < subscribers.size(); ++i )
            {
                subscribers[i]->Execute( properties );
            }
        }

        template< typename T_ObjectType > void RegisterObjForEvent( const std::string& eventName, T_ObjectType& obj, void( T_ObjectType::*func )( NamedProperty& ) )
        {
            EventSubscriberBase* newSub = new EventSubscriber< T_ObjectType >( obj, func );

            m_subscribers[eventName].push_back( newSub );
        }

        template< typename T_ObjectType > void UnregisterObjFromAllEvents( T_ObjectType& obj )
        {
            for( auto iter = m_subscribers.begin(); iter != m_subscribers.end(); ++iter )
            {
                std::vector< EventSubscriberBase* >& subscribers = iter->second;

                for( unsigned int i = 0; i < subscribers.size(); ++i )
                {
                    EventSubscriber< T_ObjectType >* eventSub = dynamic_cast< EventSubscriber< T_ObjectType >* >( subscribers[i] );
                    
                    if( eventSub )
                    {
                        subscribers[i] = subscribers.back();
                        subscribers.pop_back();
                    }
                }
            }
        }

        template< typename T_ObjectType > void UnregisterObjFromEvent( const std::string& eventName, T_ObjectType& obj )
        {
            auto eventSubscribers = m_subscribers.find( eventName );

            if( eventSubscribers == m_subscribers.end() )
                return;

            std::vector< EventSubscriberBase* > subscribers = eventSubscribers->second;

            for( unsigned int i = 0; i < subscribers.size(); ++i )
            {
                EventSubscriber< T_ObjectType >* eventSub = dynamic_cast< EventSubscriber< T_ObjectType >* >( subscribers[i] );

                if( eventSub )
                {
                    subscribers[i] = subscribers.back();
                    subscribers.pop_back();
                }
            }
        }

        template< typename T_ObjectType > void UnregisterObjFuncFromEvent( const std::string& eventName, T_ObjectType& obj, void( T_ObjectType::*func )( NamedProperty& ) )
        {
            auto eventSubscribers = m_subscribers.find( eventName );

            if( eventSubscribers == m_subscribers.end() )
                return;

            std::vector< EventSubscriberBase* > subscribers = eventSubscribers->second;

            for( unsigned int i = 0; i < subscribers.size(); ++i )
            {
                EventSubscriber< T_ObjectType >* eventSub = dynamic_cast< EventSubscriber< T_ObjectType >* >( subscribers[i] );

                if( eventSub && eventSub->getFuncPtr() == func )
                {
                    subscribers[i] = subscribers.back();
                    subscribers.pop_back();
                }
            }
        }

    private:

        std::map< std::string, std::vector< EventSubscriberBase* > > m_subscribers;

    };
}
