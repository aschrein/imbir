#pragma once
#include <string>
#include <vector>
#include <functional>
#include <imgui_memory_editor.h>
#include <ImGuiColorTextEdit/TextEditor.h>
#include <Windows.h>
#include <log.h>

using OnChange = std::function<void(void)>;

HRESULT disassembler(std::vector<byte> const &, std::string &, const char *comment);
HRESULT assembler(std::string const &assembly, vector<byte> &result_bytecode);

bool TokenizeCStyleNumber(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);
bool TokenizeCStyleComment(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);
bool TokenizeCStyleCharacterLiteral(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);
bool TokenizeCStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);
bool TokenizeCStylePunctuation(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);
std::vector<std::string> getDXBCBuiltinFuncs();

const TextEditor::LanguageDefinition& DXBC()
{
  static bool inited = false;
  static TextEditor::LanguageDefinition langDef;
  if (!inited)
  {
    static const char* const cppKeywords[] = {
      "o0", "o1", "o2", "o3", "o4", "o5",
      "v0", "v1", "v2", "v3", "v4", "v5",
      "l", "vThreadID"
    };
    for (auto& k : cppKeywords)
      langDef.mKeywords.insert(k);

    for (auto& k : getDXBCBuiltinFuncs())
    {
      TextEditor::Identifier id;
      id.mDeclaration = "Built-in function";
      langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
    }

    langDef.mTokenize = [](const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, TextEditor::PaletteIndex & paletteIndex) -> bool
    {
      paletteIndex = TextEditor::PaletteIndex::Max;

      while (in_begin < in_end && isblank(*in_begin))
        in_begin++;

      if (in_begin == in_end)
      {
        out_begin = in_end;
        out_end = in_end;
        paletteIndex = TextEditor::PaletteIndex::Default;
      }
      else if (TokenizeCStyleComment(in_begin, in_end, out_begin, out_end))
        paletteIndex = TextEditor::PaletteIndex::Comment;
      else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
        paletteIndex = TextEditor::PaletteIndex::CharLiteral;
      else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
        paletteIndex = TextEditor::PaletteIndex::Identifier;
      else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
        paletteIndex = TextEditor::PaletteIndex::Number;
      else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
        paletteIndex = TextEditor::PaletteIndex::Punctuation;

      return paletteIndex != TextEditor::PaletteIndex::Max;
    };

    langDef.mCommentStart = "/*";
    langDef.mCommentEnd = "*/";

    langDef.mCaseSensitive = true;
    langDef.mAutoIndentation = false;

    langDef.mName = "DXBC";

    inited = true;
  }
  return langDef;
}

class DXBCEditor
{
private:
  std::string m_dxasm;
  OnChange m_onChange;
  
  std::vector<byte> m_dxbc;

  MemoryEditor m_dxbcEd;
  TextEditor m_dxasmEd;
  bool m_dxasmDirty = true;
  //TextEditor hlslEd;
  void updateDisasm()
  {
    auto res = disassembler(m_dxbc, m_dxasm, "");
    if (res)
    {
      m_dxasmEd.SetText("");
      addLog("[ERROR][DXBCEditor] while disassembling, 'res' = %i\n", res);
    }
    else
    {
      m_dxasmEd.SetText(m_dxasm);
    }
  }
public:
  DXBCEditor()
  {
    m_dxasmEd.SetLanguageDefinition(DXBC());
  }
  void setBinary(byte const *pBytecode, size_t size, OnChange onChange)
  {
    assert(pBytecode && size);
    m_dxbc = std::vector<byte>(pBytecode, pBytecode + size);
    m_onChange = onChange;
    updateDisasm();
    
  }
  void render()
  {
    m_dxasmEd.Render("Shader Editor. CTRL-B to build.");
    if (m_dxasmEd.IsTextChanged())
    {
      //addLog("[DEBUG][DXBCEditor] shader has been changed.\n");
      m_dxasmDirty = true;
    }
    if (m_dxasmDirty && ImGui::GetIO().KeyCtrl && ImGui::IsKeyDown('B'))
    {
      auto res = assembler(m_dxasmEd.GetText(), m_dxbc);
      if (res)
      {
        m_dxbc = {};
        addLog("[ERROR][DXBCEditor] while assembling, 'res' = %i\n", res);
      }
      else
      {
        updateDisasm();
      }
      m_dxasmDirty = false;
    }
    m_dxbcEd.DrawWindow("Byte code", &m_dxbc[0], m_dxbc.size());
  }
};