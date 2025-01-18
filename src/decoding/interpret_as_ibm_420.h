#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_420 {
public:
    static Interpret_As_IBM_420& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_420();
    ~Interpret_As_IBM_420() = default;

    Interpret_As_IBM_420(const Interpret_As_IBM_420&) = delete;
    Interpret_As_IBM_420& operator=(const Interpret_As_IBM_420&) = delete;

    QString decodeIBM420(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
