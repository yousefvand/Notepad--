#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <QByteArray>

class Interpret_As_Utf_7 {
public:
    static Interpret_As_Utf_7& instance();

    // Execute UTF-7 interpretation for the given editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_Utf_7() = default;
    ~Interpret_As_Utf_7() = default;

    Interpret_As_Utf_7(const Interpret_As_Utf_7&) = delete;
    Interpret_As_Utf_7& operator=(const Interpret_As_Utf_7&) = delete;

    QString decodeUTF7(const QByteArray& utf7Data);
};
