#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_918 {
public:
    static Interpret_As_IBM_918& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_918();
    ~Interpret_As_IBM_918() = default;

    Interpret_As_IBM_918(const Interpret_As_IBM_918&) = delete;
    Interpret_As_IBM_918& operator=(const Interpret_As_IBM_918&) = delete;

    QString decodeIBM918(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
