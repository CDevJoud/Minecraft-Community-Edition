#include "XPP.hpp"

class MyMod : public IXPP {
public:
	MyMod();
	~MyMod();
	XAPIDescriptor query() override {
		return XAPIDescriptor();
	}
	int onInit() override {
		XPP::logDebug("Hello C++");
		return 1;
	}
	int onUpdate() override {
		return 1;
	}
	int onShutdown() override {
		return 1;
	}
}mod;

MyMod::MyMod() {
	XPP::initXPP(&mod);
}

MyMod::~MyMod() {

}
