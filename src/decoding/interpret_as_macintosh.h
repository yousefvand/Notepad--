#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>
#include <unordered_map>

class Interpret_As_macintosh {
public:
    // Singleton instance access
    static Interpret_As_macintosh& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_macintosh();
    ~Interpret_As_macintosh() = default;

    // Disable copy construction and assignment
    Interpret_As_macintosh(const Interpret_As_macintosh&) = delete;
    Interpret_As_macintosh& operator=(const Interpret_As_macintosh&) = delete;

    // Decoding method for macintosh data
    QString decodeMacintosh(const QByteArray& rawData);

    // Macintosh encoding table
    static const std::unordered_map<unsigned char, QChar> macintoshTable;
};
