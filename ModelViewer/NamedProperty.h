#pragma once

#include <map>
#include <vector>
#include <string>

namespace SBUtil
{
    class TypedDataBase
    {

    public:

        virtual ~TypedDataBase(){}

    };

    template< typename T > class TypedData : public TypedDataBase
    {

    public:

        TypedData( const T init = T() )
            :   data( init )
        {}

        T GetData(){ return data; }

    private:

        T data;

    };

    class NamedProperty
    {

    public:

        enum NPGetResult
        {
            SUCCESS = 0,
            NO_ENTRY,
            WRONG_TYPE
        };

        std::map< std::string, TypedDataBase* > m_properties;

        template < typename T > void Set( std::string& name, const T& value )
        {
            TypedDataBase* newProperty = new TypedData< T >( value );
            auto iter = m_properties.find( name );
            if( iter != m_properties.end() )
            {
                delete iter->second;
                iter->second = newProperty;
            }
            else
            {
                m_properties[ name ] = newProperty;
            }
        }

        template < typename T > NPGetResult Get( std::string name, T& value_out )
        {
            //Err: no entry
            //Err: wrong type
            //Success
            auto iter = m_properties.find(name);
            
            if( iter == m_properties.end() )
            {
                return NO_ENTRY;
            }

            TypedData< T >* asTypedData = dynamic_cast< TypedData< T >* >( iter->second );
            //virtual typeInfo& GetTypeID() const = 0;
            //typeid( T ) returns the id of the type T
            if( !asTypedData )
            {
                return WRONG_TYPE;
            }

            else
            {
                value_out = asTypedData->GetData();
                return SUCCESS;
            }
        }

    };
}