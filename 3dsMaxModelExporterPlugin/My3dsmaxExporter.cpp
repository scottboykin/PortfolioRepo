#include <Max.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <IGame/IGame.h>
#include <IGame/IGameObject.h>
#include <IGame/IGameProperty.h>
#include <IGame/IGameControl.h>
#include <IGame/IGameModifier.h>
#include <IGame/IConversionManager.h>
#include <IGame/IGameError.h>
#include <IGame/IGameFX.h>

// C/C++
#include <vector>
#include <algorithm>
#include <fstream>
#include "My3dsmaxExporter.h"
#include "Vertex.h"
#include "SkinnedVertex.h"

My3dsmaxExporter::My3dsmaxExporter() :
	m_igame( 0 ),
	m_fh( 0 ),
    m_log( 0 )
{}

My3dsmaxExporter::~My3dsmaxExporter()
{
	deinit();
}

void My3dsmaxExporter::deinit()
{
	if ( m_fh != 0 )
	{
		fclose( m_fh );
		m_fh = 0;
	}

    if( m_log != 0 )
    {
        fclose( m_log );
        m_log = 0;
    }

	if ( m_igame != 0 )
	{
		m_igame->ReleaseIGame();
		m_igame = 0;
	}
}

int My3dsmaxExporter::DoExport( const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressprompts, DWORD options )
{
	try
	{
		const bool exportselected = 0 != (options & SCENE_EXPORT_SELECTED); // export only selected objects
        std::string logName = convertWCharToString( name );

        logName.erase( logName.begin() + logName.find( '.' ), logName.end() );
        logName += ".log";

		// initialize IGame
		m_igame = GetIGameInterface();
		if ( !m_igame )
			throw std::exception( "Failed to initialize IGame interface" );
        IGameConversionManager* conversion = GetConversionManager();
        conversion->SetCoordSystem( IGameConversionManager::IGAME_OGL );
		m_igame->InitialiseIGame( exportselected );
		// open file for writing
		m_fh = _wfopen( name, L"wb" );
        m_log = fopen( logName.c_str(), "wb" );

        writeSceneNodes();
		
		// release initialized stuff
		deinit();
	}
	catch ( std::exception& e )
	{
        //TSTR tstr( e.what() );
		MessageBoxA( i->GetMAXHWnd(), e.what(), ("Export Error"), MB_OK|MB_ICONERROR );
		deinit();
	}
	return TRUE;
}

void My3dsmaxExporter::ShowAbout( HWND hwnd )
{
}

int My3dsmaxExporter::ExtCount()
{
	return 1;
}

const TCHAR* My3dsmaxExporter::Ext( int /*n*/ )
{
	return _T("my3d");
}

const TCHAR* My3dsmaxExporter::LongDesc()
{
	return _T("My 3dsmax Exporter");
}

const TCHAR* My3dsmaxExporter::ShortDesc()
{
	return _T("My3dsmaxExporter");
}

const TCHAR* My3dsmaxExporter::AuthorName()
{
	return _T("Scott Boykin");
}

const TCHAR* My3dsmaxExporter::CopyrightMessage()
{
	return _T("Copyright (C) Scott Boykin 2013");
}

const TCHAR* My3dsmaxExporter::OtherMessage1()
{
	return _T("");
}

const TCHAR* My3dsmaxExporter::OtherMessage2()
{
	return _T("");
}

unsigned int My3dsmaxExporter::Version()
{
	return 1;
}

BOOL My3dsmaxExporter::SupportsOptions( int ext, DWORD options )
{
	return TRUE;
}

void My3dsmaxExporter::writeSceneNodes()
{
    resetMaterialID();
    resetTransformID();
    resetBoneID();

    for( int i = 0; i < m_igame->GetRootMaterialCount(); ++i )
    {
        IGameMaterial* material = m_igame->GetRootMaterial( i );
        writeMaterial( material );
    }

    for( int i = 0; i < m_igame->GetTopLevelNodeCount(); ++i )
    {
        IGameNode* node = m_igame->GetTopLevelNode( i );
        
        writeChildNodes( node, -1 );
    }
}

void My3dsmaxExporter::writeChildNodes( IGameNode* node, int parentTransformID )
{
    int transformID = generateTransformID();

    if( node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_MESH )
    {   
        fprintf( m_fh, "Name: %s\n", convertWCharToString( node->GetName() ).c_str() );
        writeMeshData( node, parentTransformID, transformID );
    }

    else if( node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_CAMERA )
    {
        fprintf( m_fh, "Name: %s\n", convertWCharToString( node->GetName() ).c_str() );
        std::vector< Matrix3 > transforms;
        transforms = writeLocalTransformationMatrices( node );

        for( int i = 0; i < transforms.size(); ++i )
        {
            fprintf( m_fh, "t, %i, %i", transformID, parentTransformID );
            for( int j = 0; j < 4; ++j )
            {
                Point3 row = transforms[i].GetRow(j);
                fprintf( m_fh, ", %f, %f, %f", row.x, row.y, row.z );
            }
            fprintf( m_fh, "\n" );
        }
    }

    else if( node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_BONE || node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_HELPER )
    {
        std::vector< Matrix3 > transforms = writeWorldTransformationMatrices( node );
        Matrix3 initialInverse = node->GetWorldTM().Inverse().ExtractMatrix3();

        fprintf( m_fh, "Name: %s\n", convertWCharToString( node->GetName() ).c_str() );

        int boneID = -1;
        if( m_nodeIDtoBoneID.find( node->GetNodeID() ) == m_nodeIDtoBoneID.end() )
        {
            boneID = generateBoneID();
            m_nodeIDtoBoneID[node->GetNodeID()] = boneID;
        }
        else
        {
            boneID = m_nodeIDtoBoneID[node->GetNodeID()];
        }

        fprintf( m_fh, "Bone %i,%i\n", boneID, transforms.size() );

        fwrite( &initialInverse, sizeof( Matrix3 ), 1, m_fh );
        fwrite( transforms.data(), sizeof( Matrix3 ), transforms.size(), m_fh );
        fputc( '\n', m_fh );
    }
    
    for( int i = 0; i < node->GetChildCount(); ++i )
    {
        IGameNode* child = node->GetNodeChild( i );
        writeChildNodes( child, transformID );
    }
}

void My3dsmaxExporter::writeMeshData( IGameNode* meshNode, int parentTransformID, int transformID )
{
    IGameObject* meshObj = meshNode->GetIGameObject();
    IGameMesh* mesh = reinterpret_cast<IGameMesh*>( meshObj );

    Matrix3 toLocalSpace = meshNode->GetWorldTM().Inverse().ExtractMatrix3();

    bool bIsStaticMesh = true;

    std::ofstream logOut( m_log );

    if( !mesh->InitializeData() )
    {
        throw( std::exception( "Failed Initialize" ) );
    }
    
    for( int i = 0; i < mesh->GetNumModifiers(); ++i )
    {
        IGameModifier* modifer = mesh->GetIGameModifier( i );
        if( modifer->IsSkin() )
        {
            bIsStaticMesh = false;
            IGameSkin* skin = reinterpret_cast< IGameSkin* >( modifer );
            std::vector< SkinnedVertex > skinnedVerts;

            fprintf( m_fh, "SkeletalMesh %i\n", mesh->GetNumberOfFaces() * 3 );

            for( int i = 0; i < mesh->GetNumberOfFaces(); ++i )
            {
                FaceEx* face = mesh->GetFace( i );

                IGameMaterial* mat = mesh->GetMaterialFromFace( face );
                char matName[250];
                if( mat )
                {
                    wcstombs( matName, mat->GetMaterialName(), 250 );
                }

                for( int j = 0; j < 3; ++j )    
                {
                    SkinnedVertex v;
                    Point3 vertexPoint;
                    Point3 vertexNormal;
                    Point3 vertexColor;
                    Point2 textureCoord;

                    if( !mesh->GetVertex( face->vert[j], vertexPoint ) )
                    {
                        throw( std::exception( "Bad Vertex Data" ) );
                    }

                    logOut << "v, ";

                    logOut << vertexPoint.x << ", ";
                    logOut << vertexPoint.y << ", ";
                    logOut << vertexPoint.z;

                    v.x = vertexPoint.x;
                    v.y = vertexPoint.y;
                    v.z = vertexPoint.z;

                    if( mesh->GetNormal( face->norm[j], vertexNormal ) )
                    {
                        vertexNormal = toLocalSpace.VectorTransform( vertexNormal );

                        logOut << ", " << vertexNormal.x << ", ";
                        logOut << vertexNormal.y << ", ";
                        logOut << vertexNormal.z;

                        v.nx = vertexNormal.x;
                        v.ny = vertexNormal.y;
                        v.nz = vertexNormal.z;
                    }
                    else
                    {
                        logOut << ", 0, 0, 0";
                    }

                    if( mesh->GetTexVertex( face->texCoord[j], textureCoord ) )
                    {
                        logOut << ", " << textureCoord.x << ", ";
                        logOut << -textureCoord.y;

                        v.tu = textureCoord.x;
                        v.tv = -textureCoord.y;
                    }
                    else
                    {
                        logOut << ", 0, 0";
                    }

                    if( mat )
                    {
                        logOut << ", " << m_materialIDs[ convertWCharToString( mat->GetMaterialName() ) ];
                        v.matID = m_materialIDs[ convertWCharToString( mat->GetMaterialName() ) ];
                    }
                    else
                    {
                        logOut << ", 0";
                    }

                    int vertexIndex = face->vert[j];
                    std::vector< std::pair< int, float > > allBonesAffectVert;

                    for( int k = 0; k < skin->GetNumberOfBones( vertexIndex ); ++k )
                    {
                        IGameNode* bone = skin->GetIGameBone( vertexIndex, k );
                        std::pair< int, float > boneIndexWeight;

                        if( m_nodeIDtoBoneID.find( bone->GetNodeID() ) == m_nodeIDtoBoneID.end() )
                        {
                            int newBoneID = generateBoneID();
                            m_nodeIDtoBoneID[ bone->GetNodeID() ] = newBoneID;
                            boneIndexWeight.first = newBoneID;
                        }
                        else
                        {
                            boneIndexWeight.first = m_nodeIDtoBoneID[ bone->GetNodeID() ];
                        }

                        boneIndexWeight.second = skin->GetWeight( vertexIndex, k );

                        allBonesAffectVert.push_back( boneIndexWeight ); 

                        for( int index = allBonesAffectVert.size() - 1; index > 0; --index )
                        {
                            if( allBonesAffectVert[index].second > allBonesAffectVert[index-1].second )
                            {
                                std::pair< int, float > temp = allBonesAffectVert[index];
                                allBonesAffectVert[index] = allBonesAffectVert[index-1];
                                allBonesAffectVert[index-1] = temp;
                            }
                        }
                    }
                    
                    if( allBonesAffectVert.size() > 4 )
                    {
                        float length = 0;
                        for( int index = 0; index < 4; ++index )
                        {
                            length += allBonesAffectVert[index].second;
                        }

                        for( int index = 0; index < 4; ++index )
                        {
                            allBonesAffectVert[index].second /= length;
                        }
                    }

                    logOut << "bones " << allBonesAffectVert.size() << ", ";
                    for( int index = 0; index < allBonesAffectVert.size() && index < 4; ++index )
                    {
                        v.boneIndices[index] = allBonesAffectVert[index].first;
                        v.boneWeights[index] = allBonesAffectVert[index].second;
                    }
                    logOut << " " << v.boneIndices[0] << "," << v.boneWeights[0] << "," << v.boneIndices[1] << "," << v.boneWeights[1] << "," << v.boneIndices[2] << "," << v.boneWeights[2] << "," << v.boneIndices[3] << "," << v.boneWeights[3]; 
                    logOut << std::endl;

                    skinnedVerts.push_back( v );
                }
            }

            fwrite( (void*)skinnedVerts.data(), sizeof( SkinnedVertex ), skinnedVerts.size(), m_fh );

        }
    }

    if( bIsStaticMesh )
    {
        std::vector< Matrix3 > transforms = writeLocalTransformationMatrices( meshNode );
        std::vector< Vertex > vertices;

        fprintf( m_fh, "StaticMesh %i,%i,%i,%i\n", mesh->GetNumberOfFaces() * 3, transforms.size(), transformID, parentTransformID );

        for( int i = 0; i < mesh->GetNumberOfFaces(); ++i )
        {
            FaceEx* face = mesh->GetFace( i );

            IGameMaterial* mat = mesh->GetMaterialFromFace( face );
            char matName[250];
            if( mat )
            {
                wcstombs( matName, mat->GetMaterialName(), 250 );
            }

            for( int j = 0; j < 3; ++j )    
            {
                Vertex v;
                Point3 vertexPoint;
                Point3 vertexNormal;
                Point3 vertexColor;
                Point2 textureCoord;

                if( !mesh->GetVertex( face->vert[j], vertexPoint ) )
                {
                    throw( std::exception( "Bad Vertex Data" ) );
                }

                vertexPoint = toLocalSpace.PointTransform( vertexPoint );

                logOut << "v, ";

                logOut << vertexPoint.x << ", ";
                logOut << vertexPoint.y << ", ";
                logOut << vertexPoint.z;

                v.x = vertexPoint.x;
                v.y = vertexPoint.y;
                v.z = vertexPoint.z;

                if( mesh->GetNormal( face->norm[j], vertexNormal ) )
                {
                    vertexNormal = toLocalSpace.VectorTransform( vertexNormal );

                    logOut << ", " << vertexNormal.x << ", ";
                    logOut << vertexNormal.y << ", ";
                    logOut << vertexNormal.z;

                    v.nx = vertexNormal.x;
                    v.ny = vertexNormal.y;
                    v.nz = vertexNormal.z;
                }
                else
                {
                    logOut << ", 0, 0, 0";
                }

                if( mesh->GetTexVertex( face->texCoord[j], textureCoord ) )
                {
                    logOut << ", " << textureCoord.x << ", ";
                    logOut << -textureCoord.y;

                    v.tu = textureCoord.x;
                    v.tv = -textureCoord.y;
                }
                else
                {
                    logOut << ", 0, 0";
                }

                if( mat )
                {
                    logOut << ", " << m_materialIDs[ convertWCharToString( mat->GetMaterialName() ) ];
                    v.matID = m_materialIDs[ convertWCharToString( mat->GetMaterialName() ) ];
                }
                else
                {
                    logOut << ", 0";
                }

                logOut << std::endl;
                vertices.push_back( v );
            }
        }

        for( int i = 0; i < transforms.size(); ++i )
        {
            logOut << "t, " << transformID << ", " << parentTransformID;
            for( int j = 0; j < 4; ++j )
            {
                Point3 row = transforms[i].GetRow(j);
                logOut << ", " << row.x << ", " << row.y << ", " << row.z; 
            }
            logOut << std::endl;
        } 

        fwrite( (void*)vertices.data(), sizeof( Vertex ), vertices.size(), m_fh );
        fwrite( (void*)transforms.data(), sizeof( Matrix3 ), transforms.size(), m_fh );
    }
}

void My3dsmaxExporter::writeMaterial( IGameMaterial* material )
{
    for( int i = 0; i < material->GetSubMaterialCount(); ++i )
    {
        IGameMaterial* subMaterial = material->GetSubMaterial( i );
        writeMaterial( subMaterial );
    }

    if( material->GetNumberOfTextureMaps() > 0 )
    {
        int materialID = generateMaterialID();
        m_materialIDs.insert( std::pair< std::string, int >( convertWCharToString( material->GetMaterialName() ), materialID ) );

        fprintf( m_fh, "Material %i,%i\n", materialID, material->GetNumberOfTextureMaps() );
        for( int i = 0; i < material->GetNumberOfTextureMaps(); ++i )
        {
            std::string fileName = getFileName( convertWCharToString( material->GetIGameTextureMap( i )->GetBitmapFileName() ) );
       
            fprintf( m_fh, "%i, %s\n", material->GetIGameTextureMap(i)->GetStdMapSlot(), fileName.c_str() );
        }
    }
}

std::vector< Matrix3 > My3dsmaxExporter::writeLocalTransformationMatrices( IGameNode* node )
{
    std::vector< Matrix3 > transforms;
    bool bIsOneFrame = true;
    
    for( int t = m_igame->GetSceneStartTime(); t < m_igame->GetSceneEndTime(); t += 4800/30 )
    {
        Matrix3 currentTransform = node->GetLocalTM( t ).ExtractMatrix3();
        Matrix3 nextTransform = node->GetLocalTM( t + 4800/30 ).ExtractMatrix3();

        transforms.push_back( currentTransform );

        if( !currentTransform.Equals( nextTransform ) )
        {
            bIsOneFrame = false;
        }
    }

    if( bIsOneFrame )
    {
        transforms.resize( 1 );
    }

    return transforms;
    
}

std::vector< Matrix3 > My3dsmaxExporter::writeWorldTransformationMatrices( IGameNode* node )
{
    std::vector< Matrix3 > transforms;
    bool bIsOneFrame = true;

    for( int t = m_igame->GetSceneStartTime(); t < m_igame->GetSceneEndTime(); t += 4800/30 )
    {
        Matrix3 currentTransform = node->GetWorldTM( t ).ExtractMatrix3();
        Matrix3 nextTransform = node->GetWorldTM( t + 4800/30 ).ExtractMatrix3();

        transforms.push_back( currentTransform );

        if( !currentTransform.Equals( nextTransform ) )
        {
            bIsOneFrame = false;
        }
    }

    if( bIsOneFrame )
    {
        transforms.resize( 1 );
    }

    return transforms;

}

std::string My3dsmaxExporter::getFileName(  std::string& filePath )
{
    if( filePath.length() > 0)
    {
        size_t startPos = filePath.length() - 1;

        for( startPos; startPos > 0; --startPos )
        {
            if( filePath[startPos] == '\\' )
            {
                ++startPos;
                break;
            }
        }

        return filePath.substr( startPos, filePath.length() - startPos );
    }

    return filePath;
}

std::string My3dsmaxExporter::convertWCharToString( const wchar_t* string )
{
    char buffer[1000];
    if( string )
    {
        wcstombs( buffer,  string, 1000 );
        return std::string( buffer );
    }

    return std::string();
}

int My3dsmaxExporter::generateMaterialID()
{
    int idToReturn = m_materialID;
    ++m_materialID;

    return idToReturn;
}

void My3dsmaxExporter::resetMaterialID()
{
    m_materialID = 0;
}

int My3dsmaxExporter::generateTransformID()
{
    int idToReturn = m_transformID;
    ++m_transformID;

    return idToReturn;
}

void My3dsmaxExporter::resetTransformID()
{
    m_transformID = 0;
}

int My3dsmaxExporter::generateBoneID()
{
    int idToReturn = m_boneID;
    ++m_boneID;

    return idToReturn;
}

void My3dsmaxExporter::resetBoneID()
{
    m_boneID = 0;
}