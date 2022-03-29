#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/NamedProperties.hpp"
ColoredLine::ColoredLine(const Rgba8& color, std::string text)
	: m_color(color)
	, m_text(text)
{}

DevConsole::~DevConsole()
{
	delete m_clock;
	m_clock = nullptr;
}

void DevConsole::StartUp()
{
	//PrintString(Rgba8(255, 255, 255, 255), "Console Started");
	m_clock = new Clock();
	SetLineHeight(20.f);
}

void DevConsole::BeginFrame()
{}

void DevConsole::EndFrame()
{}

void DevConsole::ShutDown()
{}

void DevConsole::Update()
{
	if (!m_isOpen)
	{
		return;
	}
	ProcessInput();
	ProcessAutoInput();
	if (m_carotFlashCounter > 0.f)
	{
		m_carotFlashCounter -= (float)m_clock->GetLastDeltaSeconds();
	}
	if (m_carotFlashCounter <= 0.f)
	{
		m_carotFlashCounter = 0.3f;
		m_isCarotOn = !m_isCarotOn;
	}
}

void DevConsole::PrintString(const Rgba8& textColor, std::string devConsolePrintString)
{
	g_consoleMutex.lock();
	m_lines.push_back(ColoredLine(textColor, devConsolePrintString));
	if (m_lines.size() > m_maxNumofLines)
	{
		for (int i = 0; i < 10; ++i)
		{
			m_lines.erase(m_lines.begin());
		}
	}
	if ((m_lines.size() + 1) * m_lineHeight > HUD_SIZE_Y)
	{
		m_maxTextOffset = (m_lines.size() + 1) * m_lineHeight - HUD_SIZE_Y;
	}
	g_consoleMutex.unlock();
}

void DevConsole::Error(std::string devConsolePrintString)
{
	PrintString(Rgba8(255, 0, 0, 255), devConsolePrintString);
	SetIsOpen(true);
}

void DevConsole::SetLineHeight(float lineHeight)
{
	m_lineHeight = lineHeight;
}

void DevConsole::ShowHistory()
{
	if (m_historyCommands.empty())
	{
		return;
	}
	ReplaceCurrText(m_historyCommands[m_currHistoryIndex]);
	m_currHistoryIndex--;
	if (m_currHistoryIndex < 0)
	{
		m_currHistoryIndex = static_cast<int>(m_historyCommands.size()) - 1;
	}
}

void DevConsole::AutoComplete()
{
	if (m_currText.size() <= 0)
	{
		return;
	}
	std::vector<std::string> eventList = g_theEvent->GetEventList();
	std::vector<std::string> candidateList;
	for (int eventIndex = 0; eventIndex < eventList.size(); ++eventIndex)
	{
		bool isCandidate = true;
		if (m_currText.size() > eventList[eventIndex].size())
		{
			continue;
		}
		for (int charIndex = 0; charIndex < m_currText.size(); ++charIndex)
		{
			if (m_currText[charIndex] != eventList[eventIndex][charIndex])
			{
				isCandidate = false;
				continue;
			}
		}
		if (isCandidate)
		{
			candidateList.push_back(eventList[eventIndex]);
		}
	}
	if (candidateList.size() == 1)
	{
		ReplaceCurrText(candidateList[0]);
	}
	else if (candidateList.size() > 1)
	{
		PrintString(Rgba8(0, 0, 255, 255), "Here are all the possible commands:");
		std::string newCurrText = candidateList[0];
		for (int candidateIndex = 0; candidateIndex < candidateList.size(); ++candidateIndex)
		{
			PrintString(Rgba8::WHITE, candidateList[candidateIndex]);
			newCurrText = GetFirstSamePartOfStrings(newCurrText, candidateList[candidateIndex]);
		}
		ReplaceCurrText(newCurrText);
	}
}

void DevConsole::Clear()
{
	m_lines.clear();
	m_maxTextOffset = 0.f;
	m_textOffset = 0.f;
}

void DevConsole::ClearHistory()
{
	m_historyCommands.clear();
}

void DevConsole::JumpCarotToNextWord()
{
	if (m_currText.find(" ", m_currCarotIndex) != std::string::npos)
	{
		m_currCarotIndex = (int)m_currText.find(" ", m_currCarotIndex) + 1;
	}
	else
	{
		m_currCarotIndex = (int)m_currText.size();
	}
}

void DevConsole::JumpCarotToPreviousWord()
{
	if (m_currCarotIndex < 2)
	{
		return;
	}
	if (m_currText.rfind(" ", m_currCarotIndex - 2) != std::string::npos)
	{
		m_currCarotIndex = (int)m_currText.rfind(" ", m_currCarotIndex - 2) + 1;
	}
	else
	{
		m_currCarotIndex = 0;
	}
}

void DevConsole::Render(RenderContext& renderer, const Camera& camera) const
{
	if (!m_isOpen)
	{
		return;
	}
	renderer.BeginCamera(camera);
	renderer.BindTexture(renderer.m_whiteTexture);
	//renderer.DrawAABB2(AABB2(Vec2(0.f, 0.f), Vec2(HUD_SIZE_X, HUD_SIZE_Y)), Rgba8(100, 100, 100, 100));

	std::vector<Vertex_PCU> textVertices;
	std::vector<Vertex_PCU> currTextVertices;
	g_consoleMutex.lock();
	for (int coloredLineIndex = 0; coloredLineIndex < m_lines.size(); ++coloredLineIndex)
	{
		AABB2 textBox = AABB2(Vec2(0.f, (m_lines.size() - coloredLineIndex) * m_lineHeight - m_textOffset),
			Vec2(HUD_SIZE_X, HUD_SIZE_Y));

		g_theFont->AddVertsForTextInBox2D(textVertices, textBox, m_lineHeight,
			m_lines[coloredLineIndex].m_text, m_lines[coloredLineIndex].m_color, 1.f,
			ALIGN_BOTTOM_LEFT);
	}
	g_consoleMutex.unlock();
	renderer.BindTexture(g_theFont->GetTexture());
	renderer.DrawVertexArray(static_cast<int>(textVertices.size()), textVertices.data());

	AABB2 currTextBox = AABB2(Vec2::ZERO,
		Vec2(HUD_SIZE_X, m_lineHeight));
	renderer.BindTexture(renderer.m_whiteTexture);
	renderer.DrawAABB2(currTextBox, Rgba8(100, 100, 100, 200));

	Rgba8 color;
	if (m_isAutoInputComplete)
	{
		color = Rgba8::WHITE;
	}
	else {
		color = m_autoTypeColor;
	}

	g_theFont->AddVertsForTextInBox2D(currTextVertices, currTextBox, m_lineHeight,
		m_currText, color, 1.f,
		ALIGN_BOTTOM_LEFT);
	renderer.BindTexture(g_theFont->GetTexture());
	renderer.DrawVertexArray(static_cast<int>(currTextVertices.size()), currTextVertices.data());
	RenderCarot(renderer);
	RenderSelectedAABB(renderer);
	renderer.EndCamera(camera);
}

void DevConsole::RenderCarot(RenderContext& renderer) const
{
	if (!m_isCarotOn)
	{
		return;
	}
	renderer.BindTexture(renderer.m_whiteTexture);
	Vec2 start(m_currCarotIndex * m_lineHeight, 0.f);
	AABB2 carotAABB = AABB2(start,
		start + Vec2(2.f, m_lineHeight));
	renderer.DrawAABB2(carotAABB, Rgba8::WHITE);
}

void DevConsole::RenderSelectedAABB(RenderContext& renderer) const
{
	AABB2 selected = AABB2(Vec2(m_lineHeight * m_currSelectedStartIndex, 0.f), Vec2(m_lineHeight * m_currSelectedEndIndex, m_lineHeight));
	renderer.BindTexture(renderer.m_whiteTexture);
	renderer.DrawAABB2(selected, Rgba8(150, 150, 150, 100));
}

void DevConsole::ResetCarot()
{
	m_carotFlashCounter = 0.3f;
	m_isCarotOn = true;
}

void DevConsole::SetIsOpen(bool isOpen)
{
	if (m_isOpen == isOpen)
	{
		return;
	}
	m_isOpen = isOpen;
	if (isOpen)
	{
		g_theInput->PushMouseOptions(MouseMode(MOUSE_MODE_ABSOLUTE, true, false));
	}
	else
	{
		g_theInput->PopMouseOptions();
	}
	if (!isOpen)
	{
		ClearCurrText();
	}
}

bool DevConsole::IsOpen() const
{
	return m_isOpen;
}

void DevConsole::ProcessAutoInput()
{
	if (m_autoInputCounter < m_autoInputInterval)
	{
		m_autoInputCounter += (float)m_clock->GetLastDeltaSeconds();
		return;
	}
	m_autoInputCounter = 0.f;
	char c;
	if (PopCharacterAuto(&c))
	{
		AddCharacterToInput(c);
	}
	else if(!m_isAutoInputComplete) {
		PrintString(m_autoTypeColor, m_currText);
		m_isAutoInputComplete = true;
		ClearCurrText();
	}
}

void DevConsole::ProcessInput()
{
	if (m_isInputLocked)
	{
		return;
	}
	char c;
	while (g_theInput->PopCharacter(&c)) {
		if (c != '\b' && 
			c != '`' && 
			c != '\x1b' && 
			c != '\r' &&
			c != '\t' &&
			c != '\x3' &&
			c != '\x16' &&
			c != '\x18')//TODO improve this
		{
			AddCharacterToInput(c);
		}
	}

	if (g_theInput->IsKeyPressed(KEY_LEFTCTRL) && g_theInput->WasKeyJustPressed('C'))
	{
		//Copy code
		m_window->InputToClipBoard(GetSelectedString());
	}
	if (g_theInput->IsKeyPressed(KEY_LEFTCTRL) && g_theInput->WasKeyJustPressed('V'))
	{
		//Paste code
		ReplaceSelectedString();
	}
	if (g_theInput->IsKeyPressed(KEY_LEFTCTRL) && g_theInput->WasKeyJustPressed('X'))
	{
		//Cut code
		m_window->InputToClipBoard(GetSelectedString());
		ClearSelectedString();
	}
	if (g_theInput->WasKeyJustPressed(KEY_BACKSPACE))
	{
		if (m_currSelectedStartIndex == m_currSelectedEndIndex)
		{
			RemovePreviousCharacterFromInput();
		}
		else
		{
			ClearSelectedString();
		}
	}
	if (g_theInput->IsKeyPressed(KEY_BACKSPACE))
	{
		if (m_autoDeleteStartCounter < m_autoDeleteStartInterval)
		{
			m_autoDeleteStartCounter += (float)m_clock->GetLastDeltaSeconds();
		}
		else
		{
			if (m_autoDeleteCounter < m_autoDeleteInterval)
			{
				m_autoDeleteCounter += (float)m_clock->GetLastDeltaSeconds();
			}
			else
			{
				RemovePreviousCharacterFromInput();
				m_autoDeleteCounter = 0.f;
			}
		}
	}
	if (g_theInput->WasKeyJustReleased(KEY_BACKSPACE))
	{
		m_autoDeleteStartCounter = 0.f;
		m_autoInputCounter = 0.f;
	}
	if (g_theInput->WasKeyJustPressed(KEY_DELETE))
	{
		if (m_currSelectedStartIndex == m_currSelectedEndIndex)
		{
			RemoveNextCharacterFromInput();
		}
		else
		{
			ClearSelectedString();
		}
	}
	if (g_theInput->WasKeyJustPressed(KEY_HOME))
	{
		m_currCarotIndex = 0;
	}
	if (g_theInput->WasKeyJustPressed(KEY_END))
	{
		m_currCarotIndex = static_cast<int>(m_currText.size());
	}
	if (g_theInput->WasKeyJustPressed(KEY_LEFTARROW))
	{
		if (g_theInput->IsKeyPressed(KEY_LEFTCTRL))
		{
			//m_currCarotIndex = 0;
			JumpCarotToPreviousWord();
		}
		else
		{
			DecrementCarotIndex();
		}

		if (g_theInput->IsKeyPressed(KEY_SHIFT))
		{
			m_currSelectedEndIndex = m_currCarotIndex;
		}
		else
		{
			m_currSelectedStartIndex = m_currCarotIndex;
			m_currSelectedEndIndex = m_currCarotIndex;
		}
	}
	if (g_theInput->WasKeyJustPressed(KEY_RIGHTARROW))
	{
		if (g_theInput->IsKeyPressed(KEY_LEFTCTRL))
		{
			//m_currCarotIndex = static_cast<int>(m_currText.size());
			JumpCarotToNextWord();
		}
		else
		{
			IncrementCarotIndex();
		}

		if (g_theInput->IsKeyPressed(KEY_SHIFT))
		{
			m_currSelectedEndIndex = m_currCarotIndex;
		}
		else
		{
			m_currSelectedStartIndex = m_currCarotIndex;
			m_currSelectedEndIndex = m_currCarotIndex;
		}
	}
	if (g_theInput->WasKeyJustPressed(KEY_UPARROW))
	{
		if (g_theInput->IsKeyPressed(KEY_LEFTCTRL))
		{
			IncrementTextOffset(m_lineHeight);
		}
		else
		{
			ShowHistory();
		}
	}
	if (g_theInput->WasKeyJustPressed(KEY_DOWNARROW))
	{
		if (g_theInput->IsKeyPressed(KEY_LEFTCTRL))
		{
			DecrementTextOffset(m_lineHeight);
		}
	}
	if (g_theInput->WasKeyJustPressed(KEY_ENTER))
	{
		PrintString(Rgba8::WHITE, m_currText);
		AddToHistory(m_currText);
		if (!g_theEvent->Fire(m_currText))
		{
			Strings strings = SplitStringOnDelimiter(m_currText, ' ');
			std::string keyName = strings[0];
			NamedStrings keysAndArgs;
			for (int stringIndex = 1; stringIndex < strings.size(); ++stringIndex)
			{
				Strings args = SplitStringOnDelimiter(strings[stringIndex], '=');
				if (args.size() > 2)
				{
					break;
				}
				else if (args.size() == 1)
				{
					std::string tempString = keysAndArgs.GetValue("param", "");
					if (tempString == "")
					{
						keysAndArgs.SetValue("param", args[0]);
					}
					else
					{
						keysAndArgs.SetValue("param", tempString + " " + args[0]);
					}
				}
				else
				{
					keysAndArgs.SetValue(args[0], args[1]);
				}
			}
			if (!g_theEvent->Fire(keyName, keysAndArgs))
			{
				if (keyName != "quit")//Hard code
				{
					PrintString(Rgba8(255, 0, 0, 255), "Function not found.");
				}
			}
		}
		ClearCurrText();
	}

	if (g_theInput->WasKeyJustPressed(KEY_TAB))
	{
		AutoComplete();
	}
}

void DevConsole::AddCharacterToInput(char c)
{
	m_currText.insert(m_currCarotIndex, 1, c);
	IncrementCarotIndex();
	m_currSelectedStartIndex = m_currCarotIndex;
	m_currSelectedEndIndex = m_currCarotIndex;
}

void DevConsole::RemovePreviousCharacterFromInput()
{
	if (m_currCarotIndex == 0)
	{
		return;
	}
	m_currText.erase(m_currCarotIndex - 1, 1);
	DecrementCarotIndex();
	m_currSelectedStartIndex = m_currCarotIndex;
	m_currSelectedEndIndex = m_currCarotIndex;
}

void DevConsole::RemoveNextCharacterFromInput()
{
	if (m_currCarotIndex >= m_currText.size())
	{
		return;
	}
	m_currText.erase(m_currCarotIndex, 1);
}

void DevConsole::IncrementCarotIndex()
{
	if (m_currCarotIndex < m_currText.size())
	{
		m_currCarotIndex++;
	}
	ResetCarot();
}

void DevConsole::DecrementCarotIndex()
{
	if (m_currCarotIndex > 0)
	{
		m_currCarotIndex--;
	}
	ResetCarot();
}

void DevConsole::ClearCurrText()
{
	m_currText.clear();
	m_currCarotIndex = 0;
	m_currSelectedStartIndex = 0;
	m_currSelectedEndIndex = 0;
}

void DevConsole::ReplaceCurrText(std::string newText) 
{
	m_currText.clear();
	m_currText = newText;
	m_currCarotIndex = static_cast<int>(m_currText.size());
}

void DevConsole::AddToHistory(std::string command)
{
	m_historyCommands.push_back(command);
	m_currHistoryIndex = static_cast<int>(m_historyCommands.size()) - 1;
}

void DevConsole::IncrementTextOffset(float amount)
{
	m_textOffset += amount;
	if (m_textOffset > m_maxTextOffset)
	{
		m_textOffset = m_maxTextOffset;
	}
}

void DevConsole::DecrementTextOffset(float amount)
{
	m_textOffset -= amount;
	if (m_textOffset < 0.f)
	{
		m_textOffset = 0.f;
	}
}

std::string DevConsole::GetSelectedString()
{
	if (m_currSelectedStartIndex < m_currSelectedEndIndex)
	{
		return m_currText.substr(m_currSelectedStartIndex, m_currSelectedEndIndex - m_currSelectedStartIndex);
	}
	else
	{
		return m_currText.substr(m_currSelectedEndIndex, m_currSelectedStartIndex - m_currSelectedEndIndex);
	}
}

void DevConsole::ClearSelectedString()
{
	if (m_currSelectedStartIndex < m_currSelectedEndIndex)
	{
		m_currText.erase(m_currSelectedStartIndex, m_currSelectedEndIndex - m_currSelectedStartIndex);
		m_currCarotIndex = m_currSelectedStartIndex;
		m_currSelectedEndIndex = m_currSelectedStartIndex;
	}
	else
	{
		m_currText.erase(m_currSelectedEndIndex, m_currSelectedStartIndex - m_currSelectedEndIndex);
		m_currCarotIndex = m_currSelectedEndIndex;
		m_currSelectedStartIndex = m_currSelectedEndIndex;
	}
}

void DevConsole::ReplaceSelectedString()
{
	ClearSelectedString();
	m_currText.insert(m_currCarotIndex, m_window->GetFromClipBoard());
	m_currCarotIndex += (int)m_window->GetFromClipBoard().size();
	m_currSelectedEndIndex = m_currCarotIndex;
	m_currSelectedStartIndex = m_currCarotIndex;
}

void DevConsole::AppendStringToInput(std::string str)
{
	if (m_isInputLocked)
	{
		return;
	}
	m_currText.insert(m_currCarotIndex, str);
	m_currCarotIndex += (int)str.size();
	m_currSelectedEndIndex = m_currCarotIndex;
	m_currSelectedStartIndex = m_currCarotIndex;
}

void DevConsole::AutoType(std::string text, float deltaSeconds, Rgba8 color)
{
	m_isAutoInputComplete = false;
	m_autoInputInterval = deltaSeconds;
	m_autoTypeColor = color;
	for(int i = 0; i < text.size(); ++i)
	{
		m_autoInputCharacters.push(text[i]);
	}

}

bool DevConsole::PopCharacterAuto(char* out)
{
	if (!m_autoInputCharacters.empty()) {
		*out = m_autoInputCharacters.front();
		m_autoInputCharacters.pop();
		return true;
	}
	else {
		return false;
	}
}

std::string GetFirstSamePartOfStrings(std::string str1, std::string str2)
{
	int numSameChar = 0;
	int numLoop = static_cast<int>(str1.size());
	if (str2.size() < numLoop) {
		numLoop = static_cast<int>(str2.size());
	}

	for (int charIndex = 0; charIndex < numLoop; ++charIndex)
	{
		if (str1[charIndex] == str2[charIndex])
		{
			numSameChar++;
		}
	}
	return str1.substr(0, numSameChar);
}