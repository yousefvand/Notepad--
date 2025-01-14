#pragma once

#include <QString>
#include <QPlainTextEdit>

class Interpret_As_UTF_32_BE {
public:
    // Singleton instance accessor
    static Interpret_As_UTF_32_BE& instance();

    // Main functionality: Decode and interpret as UTF-32BE
    void execute(QPlainTextEdit* editor);

private:
    // Private constructor and destructor to enforce singleton
    Interpret_As_UTF_32_BE();
    ~Interpret_As_UTF_32_BE();

    // Disable copy and assignment
    Interpret_As_UTF_32_BE(const Interpret_As_UTF_32_BE&) = delete;
    Interpret_As_UTF_32_BE& operator=(const Interpret_As_UTF_32_BE&) = delete;
};
