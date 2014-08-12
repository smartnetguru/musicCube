///////////////////////////////////////////////////
// 
// Info:
//
//	...
//
// Credits:
//
//	musikCube by Casey Langen
//	Plugin by Martin Dittus
//
///////////////////////////////////////////////////

#ifndef __C_MUSIKPLUGINPROPS_H__
#define __C_MUSIKPLUGINPROPS_H__


// import to a new standard playlist or add to "now playing"?
#undef CREATE_PLAYLIST
//#define CREATE_PLAYLIST


///////////////////////////////////////////////////

// plugin info

// name of plugin
#ifdef _DEBUG
#define STR_PLUGIN_NAME _T( "Import Playlist (Debug build)" )
#else
#define STR_PLUGIN_NAME _T( "Import Playlist" )
#endif

// plugin version
#define STR_PLUGIN_VERSION _T( "0.3" )


///////////////////////////////////////////////////

// etc

// website url
#define STR_PLUGIN_WEBSITE _T( "http://dekstop.de/" )


#endif
