#pragma once
#define XAPI_USE_V1_0_0
#include "XAPI.h"

typedef Xint32(XAPI_STDCALL onShutdownCB)(Xvoid);
typedef Xint32(XAPI_STDCALL onInitCB)(Xvoid);
typedef Xint32(XAPI_STDCALL onUpdateCB)(Xvoid);
XAPI_LOCAL Xvoid initCXAPI();
XAPI_LOCAL XAPIDescriptor queryCXAPIDescriptor();

XAPI_LOCAL XHQEventBus CXAPI_createQEventBus(Xcstr name);
XAPI_LOCAL Xuint32 CXAPI_postEvent(XHQEventBus* qBus, Xvoid* event, Xconst Xuint64 type);

XAPI_LOCAL Xvoid CXAPI_logDebug(Xcstr msg);

XAPI_LOCAL Xvoid CXAPI_setOnShutdownCB(onShutdownCB cb);
XAPI_LOCAL Xvoid CXAPI_setOnInitCB(onInitCB cb);
XAPI_LOCAL Xvoid CXAPI_setOnUpdateCB(onUpdateCB cb);

