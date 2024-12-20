#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class Interpret_As_TIS_620 {
public:
    static Interpret_As_TIS_620& instance();

    // Main decoding method
    QString decodeTIS620(const QByteArray& tis620Data);

    // Executes the TIS-620 interpretation for a given editor
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_TIS_620() = default;
    ~Interpret_As_TIS_620() = default;

    Interpret_As_TIS_620(const Interpret_As_TIS_620&) = delete;
    Interpret_As_TIS_620& operator=(const Interpret_As_TIS_620&) = delete;

    uint32_t tis620ToUnicode(uint8_t tis620Char);
};
