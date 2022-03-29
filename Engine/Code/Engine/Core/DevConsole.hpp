#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Rgba8.hpp"
#include <queue>
#include <mutex>
class RenderContext;
class Camera;
class InputSystem;
class Window;
class Clock;
typedef void(*EventCallbackFunctionPtrType)();

struct ColoredLine
{
public:
	Rgba8 m_color;
	std::string m_text;
public:
	ColoredLine(const Rgba8& color, std::string text);
};
class DevConsole
{
public:
	DevConsole() = default;
	~DevConsole();
	void StartUp();
	void BeginFrame();
	void EndFrame();
	void ShutDown();
	void Update();
	void PrintString(const Rgba8& textColor, std::string devConsolePrintString);
	void Error(std::string devConsolePrintString);
	void Render(RenderContext& renderer, const Camera& camera) const;
	void SetIsOpen(bool isOpen);
	bool IsOpen() const;
	void ProcessInput();
	void ProcessAutoInput();
	void AddCharacterToInput(char c);
	void RemovePreviousCharacterFromInput();
	void RemoveNextCharacterFromInput();
	void RenderCarot(RenderContext& renderer) const;
	void RenderSelectedAABB(RenderContext& renderer) const;
	void ResetCarot();
	void SetLineHeight(float lineHeight);
	void ShowHistory();
	void AutoComplete();
	void Clear();
	void ClearHistory();
	void JumpCarotToNextWord();
	void JumpCarotToPreviousWord();
	std::string GetSelectedString();
	void ClearSelectedString();
	void ReplaceSelectedString();
	void AppendStringToInput(std::string str);
	void SetLockInput(bool lock) { m_isInputLocked = lock; }
	void AutoType(std::string text, float deltaSeconds, Rgba8 color = Rgba8::WHITE);
	bool PopCharacterAuto(char* out);
public:
	Window* m_window = nullptr;
private:
	void IncrementCarotIndex();
	void DecrementCarotIndex();
	void IncrementTextOffset(float amount);
	void DecrementTextOffset(float amount);
	void ClearCurrText();
	void ReplaceCurrText(std::string newText);
	void AddToHistory(std::string command);
private:
	bool m_isOpen = false;
	bool m_isSelectOn = false;
	bool m_isInputLocked = false;
	//for auto type
	float m_autoInputInterval = 0.f;
	float m_autoInputCounter = 0.f;
	//for hold to delete
	float m_autoDeleteStartInterval = .5f;
	float m_autoDeleteStartCounter = 0.f;
	float m_autoDeleteInterval = .03f;
	float m_autoDeleteCounter = 0.f;

	std::queue<char> m_autoInputCharacters;
	bool m_isAutoInputComplete = true;
	Rgba8 m_autoTypeColor = Rgba8::WHITE;

	std::vector<ColoredLine> m_lines;
	std::vector<std::string> m_historyCommands;
	std::string m_currText = "";
	int m_currCarotIndex = 0;
	int m_currHistoryIndex = 0;
	int m_currSelectedStartIndex = 0;
	int m_currSelectedEndIndex = 0;
	int m_maxNumofLines = 50;
	float m_lineHeight = 0.f;
	float m_carotFlashCounter = 0.3f;
	float m_textOffset = 0.f;
	float m_maxTextOffset = 0.f;
	bool m_isCarotOn = true;
	Clock* m_clock = nullptr;
};

std::string GetFirstSamePartOfStrings(std::string str1, std::string str2);//To be moved somewhere else