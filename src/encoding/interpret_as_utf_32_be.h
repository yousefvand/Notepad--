#pragma once

#include <QString>
#include <QPlainTextEdit>

class InterpreteAsUTF32BE {
public:
    // Singleton instance accessor
    static InterpreteAsUTF32BE& instance();

    // Main functionality: Decode and interpret as UTF-32BE
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor and destructor to enforce singleton
    InterpreteAsUTF32BE();
    ~InterpreteAsUTF32BE();

    // Disable copy and assignment
    InterpreteAsUTF32BE(const InterpreteAsUTF32BE&) = delete;
    InterpreteAsUTF32BE& operator=(const InterpreteAsUTF32BE&) = delete;
};
