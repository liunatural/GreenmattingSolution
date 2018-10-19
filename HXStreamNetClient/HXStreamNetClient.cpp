// HXStreamNetClient.cpp : Defines the exported functions for the DLL application.
//

#include "HXVideoStreamNetClient.h"

HXSTREAMNETCLIENT_API HXStreamNetClient* CreateVideoStreamNetClientService()
{
	return new HXVideoStreamNetClient();

}
