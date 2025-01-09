#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_775 {
public:
    static Interpret_As_IBM_775& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_775();
    ~Interpret_As_IBM_775() = default;

    Interpret_As_IBM_775(const Interpret_As_IBM_775&) = delete;
    Interpret_As_IBM_775& operator=(const Interpret_As_IBM_775&) = delete;

    QString decodeIBM775(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
