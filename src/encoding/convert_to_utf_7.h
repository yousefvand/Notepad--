#pragma once
#include <QString>

class CodeEditor;

class Convert_To_Utf_7 {
public:
    static Convert_To_Utf_7& instance();
    void execute(CodeEditor* editor);

private:
    Convert_To_Utf_7() = default;
    ~Convert_To_Utf_7() = default;

    Convert_To_Utf_7(const Convert_To_Utf_7&) = delete;
    Convert_To_Utf_7& operator=(const Convert_To_Utf_7&) = delete;

    QString encodeToUtf7(const QString& text);  // Custom UTF-7 encoding
};

