#pragma once

#include <QPlainTextEdit>
#include <QString>
#include <QByteArray>

class Interpret_As_GB18030 {
public:
    // Singleton instance access
    static Interpret_As_GB18030& instance();

    // Execute decoding for the provided editor
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor for Singleton pattern
    Interpret_As_GB18030();
    ~Interpret_As_GB18030() = default;

    // Disable copy construction and assignment
    Interpret_As_GB18030(const Interpret_As_GB18030&) = delete;
    Interpret_As_GB18030& operator=(const Interpret_As_GB18030&) = delete;

    // Decoding method for GB18030 data
    QString decodeGB18030(const QByteArray& rawData);
};
