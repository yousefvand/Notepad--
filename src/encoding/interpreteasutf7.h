#pragma once

#include <QString>
#include <QByteArray>
#include <QPlainTextEdit>

class InterpreteAsUtf7
{
public:
    static InterpreteAsUtf7& instance();

    QString fromUtf7(const QByteArray& utf7Data); // Main decoding function
    void execute(QPlainTextEdit* editor);         // Executes UTF-7 decoding on QPlainTextEdit

private:
    InterpreteAsUtf7();
    ~InterpreteAsUtf7();

    // Deleted copy constructor and assignment operator
    InterpreteAsUtf7(const InterpreteAsUtf7&) = delete;
    InterpreteAsUtf7& operator=(const InterpreteAsUtf7&) = delete;

    // Helpers
    QString handleLiteralCharacter(char c);
    QByteArray decodeBase64Segment(const QByteArray& base64Segment, bool& errorFlag);
    QString convertToUtf16LE(const QByteArray& decodedBytes, bool& errorFlag);
    QString processBase64Segment(QByteArray& base64Buffer, bool& errorFlag);
    QString processUtf7Data(const QByteArray& utf7Data);

    // Helpers for UTF-7 markers
    bool isBase64Character(char c);
};
