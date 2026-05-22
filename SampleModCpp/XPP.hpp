#pragma once
#define XAPI_USE_V1_0_0
#include "XAPI.h"
#include <string>

class IXPP {
public:
	virtual ~IXPP() {

	}
	virtual XAPIDescriptor query() = 0;
	virtual int onInit() = 0;
	virtual int onUpdate() = 0;
	virtual int onShutdown() = 0;
};

class XPP {
public:
	static XPP& getSingleton();
	static void initXPP(IXPP* instance);
	static void logDebug(const std::string& msg);
	static IXPP* getModInstance();
	static XHQEventBus createQEventBus(const std::string& name);
	static int postEvent(XHQEventBus* _qBus, Xvoid* event, Xconst Xuint64 type);
private:
	static IXPP* _instance;
};

