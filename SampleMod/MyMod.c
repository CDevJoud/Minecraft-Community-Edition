#include "CXAPI.h"

XAPI_LOCAL Xint32 onInit(Xvoid) {
	CXAPI_logDebug("Hello From MyMod!Joud!");
}

XAPI_LOCAL Xint32 onUpdate(Xvoid) {

}

XAPI_LOCAL Xint32 onShutdown(Xvoid) {

}

XAPI_LOCAL XAPIDescriptor queryCXAPIDescriptor() {
	XAPIDescriptor desc;
	desc.author = "CDevJoud";
	desc.language = "C";
	desc.name = "My Mod!";
	desc.version = 1;
	desc.dependencies = "mce.core.*";
	return desc;
}

XAPI_LOCAL Xvoid initCXAPI() {
	CXAPI_setOnInitCB(onInit);
	CXAPI_setOnUpdateCB(onUpdate);
	CXAPI_setOnShutdownCB(onShutdown);
}