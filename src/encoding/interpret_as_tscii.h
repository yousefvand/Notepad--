#pragma once

#include <QPlainTextEdit>
#include <QString>

class Interpret_As_TSCII {
public:
    static Interpret_As_TSCII& instance();

    void execute(QPlainTextEdit* editor);

    ~Interpret_As_TSCII(); // Ensure destructor is declared here

private:
    Interpret_As_TSCII(); // Private constructor for singleton
    Interpret_As_TSCII(const Interpret_As_TSCII&) = delete;
    Interpret_As_TSCII& operator=(const Interpret_As_TSCII&) = delete;
};
