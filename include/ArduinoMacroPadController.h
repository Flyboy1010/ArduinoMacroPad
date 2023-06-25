#pragma once

#include <thread>
#include <string>
#include <asio.hpp>
#include <vector>
#include <unordered_map>
#include <functional>

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
public:
	ArduinoMacroPadController();
	~ArduinoMacroPadController();

	void ConnectToPort(const std::string& portName, unsigned int baudios);
	void Disconnect();

	void RenderImGui();

private:
	void SerializeConfig(const std::string& path) const;
	void DeserializeConfig(const std::string& path);

	void ProcessCommand(const std::string& command) const;
	void CommandListenerProcess();

private:
	unsigned int m_baudios;
	std::string m_portName;
	std::thread m_listenerThread;

	// asio serial port

	asio::io_service m_io;
	asio::serial_port m_port;

	// commands & actions

	std::unordered_map<std::string, Action> m_commandsMap;
};