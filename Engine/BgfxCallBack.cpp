#include "BgfxCallBack.hpp"
#include "IO/Logger.hpp"
#define LOG_INFO(...) qBus.post(event::Log(event::Log::INFO, std::format(__VA_ARGS__), "bgfx"))
#define LOG_ERROR(...) qBus.post(event::Log(event::Log::ERROR, std::format(__VA_ARGS__), "bgfx"))
namespace dms {
	BgfxCallBack::BgfxCallBack(QEventBus& qBus) : qBus(qBus) {}
	void BgfxCallBack::fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) {
		LOG_ERROR("[BGFX] {} -> line: {}, error_code: {}, desc: ", _filePath, _line, static_cast<int>(_code), _str);
	}
	void BgfxCallBack::traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) {
		// Format the message
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), _format, _argList);

		std::string msg(buffer);
		
		// Strip \r and \n from the end (or all)
		msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
		msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());


		//MCE_DEBUG("[BGFX] desc: {}", msg);

		event::LoggerOutput lOut;
		lOut.channel = "bgfx";
		lOut.msg = msg;
		lOut.severity = event::LoggerOutput::Severity::DEBUG;
		qBus.post(lOut);
	}
	void BgfxCallBack::profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) {}
	void BgfxCallBack::profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) {}
	void BgfxCallBack::profilerEnd() {}
	uint32_t BgfxCallBack::cacheReadSize(uint64_t _id) {
		return 0;
	}
	bool BgfxCallBack::cacheRead(uint64_t _id, void* _data, uint32_t _size) {
		return false;
	}
	void BgfxCallBack::cacheWrite(uint64_t _id, const void* _data, uint32_t _size) {}
	void BgfxCallBack::screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, const void* _data, uint32_t _size, bool _yflip) {}
	void BgfxCallBack::captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) {}
	void BgfxCallBack::captureEnd() {}
	void BgfxCallBack::captureFrame(const void* _data, uint32_t _size) {}
}
