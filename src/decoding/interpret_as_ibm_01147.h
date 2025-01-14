#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_01147 {
public:
    static Interpret_As_IBM_01147& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_01147();
    ~Interpret_As_IBM_01147() = default;

    Interpret_As_IBM_01147(const Interpret_As_IBM_01147&) = delete;
    Interpret_As_IBM_01147& operator=(const Interpret_As_IBM_01147&) = delete;

    QString decodeIBM01147(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
