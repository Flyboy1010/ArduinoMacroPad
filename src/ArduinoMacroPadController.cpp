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

static std::unordered_map<ActionType, std::string> g_actionTypeToStringMap = {
    { ActionType::NONE        , "none"         },
    { ActionType::KEY_MACRO   , "key_macro"    },
    { ActionType::OPEN_PROCESS, "open_process" }
};

static std::unordered_map<std::string, ActionType> g_stringToActionTypeMap = {
    { "none"     ,    ActionType::NONE         },
    { "key_macro",    ActionType::KEY_MACRO    },
    { "open_process", ActionType::OPEN_PROCESS }
};

static bool CheckLua(lua_State* l, int r)
{
    if (r != LUA_OK)
    {
        std::string errorString = lua_tostring(l, -1);
        std::cout << errorString << std::endl;

        return false;
    }

    return true;
}

static void SerializeAction(const std::string& commandName, const Action& action, json& jsonConfigFile)
{
    jsonConfigFile[commandName][g_actionTypeStr] = g_actionTypeToStringMap[action.type];
    
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
    action.type = g_stringToActionTypeMap[jsonConfigFile[commandName][g_actionTypeStr]];

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

        SendInput(numKeys * 2, inputs, sizeof(INPUT));
        
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
    // init leds color to be purple

    for (int i = 0; i < 441; i++)
    {
        m_ledsData[i] = { 175, 45, 246 };
    }

    m_time = 0.0f;

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

    for (int i = 0; i < 441; i++)
    {
        Action keyAction;
        keyAction.type = ActionType::OPEN_PROCESS;
        keyAction.processPath = "C:/Users/5davi/AppData/Roaming/Spotify/Spotify.exe";
        m_commandsMap["KEY" + std::to_string(i)] = keyAction;
    }

    Action keyAction;
    keyAction.type = ActionType::KEY_MACRO;
    keyAction.keys = { VK_LWIN, VK_SHIFT, (int)'S'}; // screenshot
    m_commandsMap["KEY" + std::to_string(5)] = keyAction;

    // SerializeConfig("config.json");

    // DeserializeConfig("config.json");

    /* LUA TESTING */

    // Step 1: Set up the Lua environment

    m_script = luaL_newstate();
    luaL_openlibs(m_script);

    // Step 2: Load and execute the Lua script

    if (CheckLua(m_script, luaL_dofile(m_script, "assets/scripts/rainbow.lua"))) // if it fails do smth
    {

    }

    // Step 3: Expose this instance and functions to Lua

    lua_pushlightuserdata(m_script, this);
    lua_pushcclosure(m_script, SetLedColorLuaWrap, 1);
    lua_setglobal(m_script, "set_led");

    lua_pushlightuserdata(m_script, this);
    lua_pushcclosure(m_script, GetLedColorLuaWrap, 1);
    lua_setglobal(m_script, "get_led");
}

ArduinoMacroPadController::~ArduinoMacroPadController()
{
    Disconnect();

    lua_close(m_script);
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

int ArduinoMacroPadController::SetLedColorLuaWrap(lua_State* l)
{
    ArduinoMacroPadController* macroPadController = (ArduinoMacroPadController*)lua_touserdata(l, lua_upvalueindex(1));
    int index = lua_tointeger(l, 1);
    uint8_t r = lua_tointeger(l, 2);
    uint8_t g = lua_tointeger(l, 3);
    uint8_t b = lua_tointeger(l, 4);
    macroPadController->SetLedColor(index, { r, g, b });
    return 0;
}

int ArduinoMacroPadController::GetLedColorLuaWrap(lua_State* l)
{
    ArduinoMacroPadController* macroPadController = (ArduinoMacroPadController*)lua_touserdata(l, lua_upvalueindex(1));
    int index = lua_tointeger(l, 1);
    led_t ledColor = macroPadController->GetLedColor(index);
    lua_pushinteger(l, ledColor.r);
    lua_pushinteger(l, ledColor.g);
    lua_pushinteger(l, ledColor.b);
    return 3;
}

void ArduinoMacroPadController::Update(float delta)
{
    // TODO: modify led color by lua script (execute update_leds function)

    lua_getglobal(m_script, "update_leds");

    if (lua_isfunction(m_script, -1))
    {
        lua_pushnumber(m_script, m_time);

        if (CheckLua(m_script, lua_pcall(m_script, 1, 0, 0)))
        {

        }
    }

    // write the led data to the arduino (if it is connected)

    if (m_port.is_open())
    {
        asio::write(m_port, asio::buffer("LEDSDATA\n"));
        asio::write(m_port, asio::buffer(m_ledsData, 441 * 3));
    }

    // increment time

    m_time += delta;
}

void ArduinoMacroPadController::RenderImGui()
{
    /* LED TESTING BUTTONS */

    ImGui::Begin("Leds Colors");

    static float ledDataNormalized[441 * 3];

    for (int i = 0; i < 441; i++)
    {
        ledDataNormalized[i * 3 + 0] = m_ledsData[i].r / 255.0f;
        ledDataNormalized[i * 3 + 1] = m_ledsData[i].g / 255.0f;
        ledDataNormalized[i * 3 + 2] = m_ledsData[i].b / 255.0f;
    }

    for (int j = 0; j < 21; j++)
    {
        ImGui::NewLine();
        for (int i = 0; i < 21; i++)
        {
            int index = i + j * 21;
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ledDataNormalized[index * 3], ledDataNormalized[index * 3 + 1], ledDataNormalized[index * 3 + 2], 1.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
            ImGui::PushID(index);
            ImGui::Button("", ImVec2(20, 20));
            ImGui::PopID();
            ImGui::PopStyleVar(2);
            //ImGui::Button(std::string("KEY" + std::to_string(index)).c_str(), ImVec2(20, 20));
            ImGui::PopStyleColor();
        }
    }

    for (int i = 0; i < 441; i++)
    {
        m_ledsData[i].r = ledDataNormalized[i * 3 + 0] * 255;
        m_ledsData[i].g = ledDataNormalized[i * 3 + 1] * 255;
        m_ledsData[i].b = ledDataNormalized[i * 3 + 2] * 255;
    }


    ImGui::End();

    /* AUDIO PANEL */

    ImGui::Begin("Audio Panel");

    // Volume Up button
    if (ImGui::Button("Volume Up"))
    {
        ProcessCommand("VOLUMEUP");
    }

    // Volume Down button
    if (ImGui::Button("Volume Down"))
    {
        ProcessCommand("VOLUMEDOWN");
    }

    // Mute button
    if (ImGui::Button("Mute"))
    {
        ProcessCommand("MUTE");
    }

    // Mute button
    if (ImGui::Button("Play/Pause"))
    {
        ProcessCommand("PLAYPAUSE");
    }

    ImGui::End();
}
