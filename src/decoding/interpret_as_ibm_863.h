#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_IBM_863 {
public:
    static Interpret_As_IBM_863& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_IBM_863();
    ~Interpret_As_IBM_863() = default;

    Interpret_As_IBM_863(const Interpret_As_IBM_863&) = delete;
    Interpret_As_IBM_863& operator=(const Interpret_As_IBM_863&) = delete;

    QString decodeIBM863(const QByteArray& rawData);
    static const std::unordered_map<unsigned char, QChar> ebcdicTable;
};
