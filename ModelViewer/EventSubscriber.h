#pragma once

#include "NamedProperty.h"

namespace SBUtil
{
    class EventSubscriberBase
    {

    public:

        virtual ~EventSubscriberBase(){}
        virtual void Execute( NamedProperty& properties ) = 0;

    };

    template< typename T_ObjectType > class EventSubscriber : public EventSubscriberBase
    {

    public:

        typedef void( T_ObjectType::*MemFuncPtrType )( NamedProperty& );

        EventSubscriber( T_ObjectType& obj, MemFuncPtrType func )
            : m_object( &obj )
            , m_func( func )
        {}

        virtual void Execute( NamedProperty& params )
        {
            (m_object->*m_func)( params );
        }

        const T_ObjectType* getObject(){ return m_object; }

        const MemFuncPtrType getFuncPtr(){ return m_func; }

        bool operator==( EventSubscriber& other ){ return m_object == other.m_object && m_func == other.m_func; }

    private:

        T_ObjectType* m_object;
        MemFuncPtrType m_func;

    };
}