#pragma once

#include <map>
#include <string>
/**
 * 3dsmax exporter plugin.
 */
class My3dsmaxExporter : public SceneExport
{
public:
	My3dsmaxExporter();
	~My3dsmaxExporter();

	/** Exports the scene. */
	int				DoExport( const TCHAR* name, ExpInterface *ei, Interface* i, BOOL suppressprompts=FALSE, DWORD options=0 );

	/** Show DLL's "About..." box */
	void			ShowAbout( HWND hwnd );

	/** Number of extensions supported */
	int				ExtCount();
	
	/** Extension #n (i.e. "3DS") */
	const TCHAR*	Ext( int n );					
	
	/** Long ASCII description (i.e. "Autodesk 3D Studio File") */
	const TCHAR*	LongDesc();
	
	/** Short ASCII description (i.e. "3D Studio") */
	const TCHAR*	ShortDesc();
	
	/** ASCII Author name */
	const TCHAR*	AuthorName();
	
	/** ASCII Copyright message */
	const TCHAR*	CopyrightMessage();
	
	/** Other message #1 */
	const TCHAR*	OtherMessage1();
	
	/** Other message #2 */
	const TCHAR*	OtherMessage2();
	
	/** Version number * 100 (i.e. v3.01 = 301) */
	unsigned int	Version();
	
	/** Returns TRUE if option supported. */
	BOOL			SupportsOptions( int ext, DWORD options );

    void writeSceneNodes();
    void writeChildNodes( IGameNode* node, int parentTransformID );
    void writeMeshData( IGameNode* meshNode, int parentTransformID, int transformID );
    void writeMaterial( IGameMaterial* material );

    std::vector< Matrix3 > writeLocalTransformationMatrices( IGameNode* node );
    std::vector< Matrix3 > writeWorldTransformationMatrices( IGameNode* node );

    int generateMaterialID();
    void resetMaterialID();

    int generateTransformID();
    void resetTransformID();

    int generateCameraID();
    void resetCameraID();

    int generateBoneID();
    void resetBoneID();

    std::string getFileName( std::string& filePath );
    std::string convertWCharToString( const wchar_t* string );

private:
	IGameScene*		m_igame;
	FILE*			m_fh;
    FILE*			m_log;
    int             m_materialID;
    int             m_transformID;
    int             m_cameraID;
    int             m_boneID;

    std::map< std::string, int > m_materialIDs;
    std::map< int, int > m_nodeIDtoBoneID;

	void			deinit();

	My3dsmaxExporter( const My3dsmaxExporter& );
	My3dsmaxExporter& operator=( const My3dsmaxExporter& );
};
