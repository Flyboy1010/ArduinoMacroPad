#pragma once

#include <thread>
#include <string>
#include <asio.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

enum ActionType
{
	NONE,
	KEY_MACRO,
	OPEN_PROCESS
};

struct Action
{
	ActionType type;
	std::vector<unsigned char> keys;
	std::string processPath;
};

class ArduinoMacroPadController
{
private:
	// led struct

	struct led_t
	{
		uint8_t r, g, b;
	};

public:
	ArduinoMacroPadController();
	~ArduinoMacroPadController();

	void ConnectToPort(const std::string& portName, unsigned int baudios);
	void Disconnect();

	void Update(float delta);
	void RenderImGui();

private:
	void SerializeConfig(const std::string& path) const;
	void DeserializeConfig(const std::string& path);

	void ProcessCommand(const std::string& command) const;
	void CommandListenerProcess();

	// functions that have a lua wrap

	inline void SetLedColor(int index, led_t color) { m_ledsData[index] = color; }
	static int SetLedColorLuaWrap(lua_State* l);

	inline led_t GetLedColor(int index) const { return m_ledsData[index]; }
	static int GetLedColorLuaWrap(lua_State* l);

private:
	unsigned int m_baudios;
	std::string m_portName;
	std::thread m_listenerThread;

	// asio serial port

	asio::io_service m_io;
	asio::serial_port m_port;

	// commands & actions

	std::unordered_map<std::string, Action> m_commandsMap;

	// leds of the macro keys

	led_t m_ledsData[441]; // 9 keys

	// lua scripting

	lua_State* m_script;
	float m_time;
};