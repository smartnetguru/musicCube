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
//	HTML Support by Rajiv Makhijani
//
///////////////////////////////////////////////////

#ifndef __C_MUSIKPLUGINPROPS_H__
#define __C_MUSIKPLUGINPROPS_H__


///////////////////////////////////////////////////

// plugin info

// name of plugin
#ifdef _DEBUG
#define STR_PLUGIN_NAME _T( "Export Playlist (Debug build)" )
#else
#define STR_PLUGIN_NAME _T( "Export Playlist" )
#endif

// plugin version
#define STR_PLUGIN_VERSION _T( "0.4" )


///////////////////////////////////////////////////

// etc

// website url
#define STR_PLUGIN_WEBSITE _T( "http://dekstop.de/" )


#endif
