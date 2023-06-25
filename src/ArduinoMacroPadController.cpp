#include "ArduinoMacroPadController.h"
#include <imgui/imgui.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <imgui/imgui.h>

using namespace nlohmann; // for using json instead of nlohmann::json

static const std::string g_actionTypeStr = "action_type";
static const std::string g_keysStr = "keys";
static const std::string g_processPathStr = "process_path";

static void SerializeAction(const std::string& commandName, const Action& action, json& jsonConfigFile)
{
    jsonConfigFile[commandName][g_actionTypeStr] = action.type;
    
    switch (action.type)
    {
    case KEY_MACRO:
        jsonConfigFile[commandName][g_keysStr] = action.keys;
        break;
    case OPEN_PROCESS:
        jsonConfigFile[commandName][g_processPathStr] = action.processPath;
        break;
    }
}

static Action DeserializeAction(const std::string& commandName, const json& jsonConfigFile)
{
    Action action;
    action.type = jsonConfigFile[commandName][g_actionTypeStr];

    switch (action.type)
    {
    case KEY_MACRO:
        action.keys = jsonConfigFile[commandName][g_keysStr].get<std::vector<unsigned char>>();
        break;
    case OPEN_PROCESS:
        action.processPath = jsonConfigFile[commandName][g_processPathStr];
        break;
    }

    return action;
}

static void OpenProcess(const std::string& path)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    // create c str path

    char* pathCStr = new char[path.size() + 1];
    strncpy_s(pathCStr, path.size() + 1, path.c_str(), path.size());
    pathCStr[path.size()] = '\0';

    // Create the process

    BOOL result = CreateProcessA(NULL, pathCStr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    // clean c str path

    delete[] pathCStr;

    // Check the result
    if (result)
    {
        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        std::cout << "Process: " << path << " opened succesfully" << std::endl;
    }
    else
    {
        // Failed to create the process
        // Handle the error accordingly

        std::cout << "Process: " << path << " couldn't be opened" << std::endl;
    }
}

static void PerformAction(const Action& action)
{
    switch (action.type)
    {
    case KEY_MACRO:
    {
        int numKeys = action.keys.size();
        INPUT* inputs = new INPUT[numKeys * 2];
        ZeroMemory(inputs, numKeys * 2 * sizeof(INPUT));

        // key presses

        for (int i = 0; i < numKeys; i++)
        {
            inputs[i].type = INPUT_KEYBOARD;
            inputs[i].ki.wVk = action.keys[i];
        }

        // key releases in the reverse order

        for (int i = numKeys - 1; i >= 0; i--)
        {
            int padding = numKeys + i;

            inputs[padding].type = INPUT_KEYBOARD;
            inputs[padding].ki.wVk = action.keys[i];
            inputs[padding].ki.dwFlags = KEYEVENTF_KEYUP;
        }

        // send the input

        UINT uSent = SendInput(numKeys * 2, inputs, sizeof(INPUT));
        
        // clean

        delete[] inputs;
    }
    break;
    case OPEN_PROCESS:
        OpenProcess(action.processPath);
        break;
    }
}

/* Arduino macro pad controller class */

ArduinoMacroPadController::ArduinoMacroPadController()
    : m_port(m_io), m_baudios(9600)
{
    /* Define commands */

    // increment volume action

    Action incrementVolumeAction;
    incrementVolumeAction.type = ActionType::KEY_MACRO;
    incrementVolumeAction.keys = { VK_VOLUME_UP };
    m_commandsMap["VOLUMEUP"] = incrementVolumeAction;

    // decrement volume action

    Action decrementVolumeAction;
    decrementVolumeAction.type = ActionType::KEY_MACRO;
    decrementVolumeAction.keys = { VK_VOLUME_DOWN };
    m_commandsMap["VOLUMEDOWN"] = decrementVolumeAction;

    // next track action

    Action nextTrackAction;
    nextTrackAction.type = ActionType::KEY_MACRO;
    nextTrackAction.keys = { VK_MEDIA_NEXT_TRACK };
    m_commandsMap["NEXTTRACK"] = nextTrackAction;

    // prev track action

    Action prevTrackAction;
    prevTrackAction.type = ActionType::KEY_MACRO;
    prevTrackAction.keys = { VK_MEDIA_PREV_TRACK };
    m_commandsMap["PREVTRACK"] = prevTrackAction;

    // play pause action

    Action playPauseAction;
    playPauseAction.type = ActionType::KEY_MACRO;
    playPauseAction.keys = { VK_MEDIA_PLAY_PAUSE };
    m_commandsMap["PLAYPAUSE"] = playPauseAction;

    // mute action

    Action muteAction;
    muteAction.type = ActionType::KEY_MACRO;
    muteAction.keys = { VK_VOLUME_MUTE };
    m_commandsMap["MUTE"] = muteAction;

    // each key of the macro pad

    for (int i = 0; i < 9; i++)
    {
        Action keyAction;
        keyAction.type = ActionType::OPEN_PROCESS;
        keyAction.processPath = "C:/Users/5davi/AppData/Roaming/Spotify/Spotify.exe";
        m_commandsMap["KEY" + std::to_string(i)] = keyAction;
    }

    Action keyAction;
    keyAction.type = ActionType::KEY_MACRO;
    keyAction.keys = { (int)'H', (int)'O', (int)'L', (int)'A' }; // screenshot
    m_commandsMap["KEY" + std::to_string(5)] = keyAction;

    // SerializeConfig("config.json");

    // DeserializeConfig("config.json");
}

ArduinoMacroPadController::~ArduinoMacroPadController()
{
    Disconnect();
}

void ArduinoMacroPadController::ConnectToPort(const std::string& portName, unsigned int baudios)
{
    m_portName = portName;
    m_baudios = baudios;

    // open serial port & start the command listener thread

    try
    {
        m_port.open(portName);
        m_port.set_option(asio::serial_port_base::baud_rate(m_baudios)); // Set baud rate to match Arduino

        m_listenerThread = std::thread([this]() {
            CommandListenerProcess();
        });
    }
    catch (const asio::system_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void ArduinoMacroPadController::Disconnect()
{
    if (m_port.is_open())
    {
        m_port.close();
    }
}

void ArduinoMacroPadController::SerializeConfig(const std::string& path) const
{
    json configFile;

    for (const auto& [commandName, action] : m_commandsMap)
    {
        SerializeAction(commandName, action, configFile);
    }

    std::ofstream file(path);
    file << std::setw(4) << configFile;
}

void ArduinoMacroPadController::DeserializeConfig(const std::string& path)
{
    std::ifstream file(path);
    json configFile = json::parse(file);

    for (auto& element : configFile.items())
    {
        const std::string& commandName = element.key();
        Action action = DeserializeAction(commandName, configFile);
        m_commandsMap[commandName] = action;
    }
}

void ArduinoMacroPadController::ProcessCommand(const std::string& command) const
{
    auto it = m_commandsMap.find(command);

    if (it != m_commandsMap.end())
    {
        const Action& action = it->second;

        PerformAction(action);
    }
}

void ArduinoMacroPadController::CommandListenerProcess()
{
    char buffer; // Buffer to hold received data
    size_t bytesRead = 0; // Number of bytes read
    std::string command;
    bool running = true;

    while (running)
    {
        try
        {
            bytesRead = asio::read(m_port, asio::buffer(&buffer, 1));

            if (bytesRead > 0)
            {
                if (buffer == '\n')
                {
                    std::cout << "Command received: " << command << std::endl;

                    ProcessCommand(command);
                    command.clear();
                }
                else
                {
                    command += buffer;
                }
            }
        }
        catch (const asio::system_error& e)
        {
            running = false;
        }
    }
}

void ArduinoMacroPadController::RenderImGui()
{
    return;
    static bool use_work_area = true;
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one of the other.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

    if (ImGui::Begin("ArduinoMacroPad", nullptr, flags))
    {
        ImVec2 buttonSize(200, 200);

        // Loop for each row
        for (int row = 0; row < 3; row++)
        {
            ImGui::Columns(3, nullptr, false);

            // Loop for each column
            for (int col = 0; col < 3; col++)
            {
                // Create a button
                if (ImGui::Button("Button", buttonSize))
                {
                    // Button clicked
                    // Add your button click logic here
                }
            }
            ImGui::NextColumn();

            ImGui::Columns(1);
        }
    }
    ImGui::End();
}
