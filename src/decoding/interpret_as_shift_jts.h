#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class Interpret_As_Shift_JTS {
public:
    static Interpret_As_Shift_JTS& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_Shift_JTS();
    QString decodeShiftJTS(const QByteArray& shiftJtsData);
};
