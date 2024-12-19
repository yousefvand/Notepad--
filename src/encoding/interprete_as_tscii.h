#pragma once

#include <QPlainTextEdit>
#include <QString>

class Interprete_As_TSCII {
public:
    static Interprete_As_TSCII& instance();

    void execute(QPlainTextEdit* editor);

    ~Interprete_As_TSCII(); // Ensure destructor is declared here

private:
    Interprete_As_TSCII(); // Private constructor for singleton
    Interprete_As_TSCII(const Interprete_As_TSCII&) = delete;
    Interprete_As_TSCII& operator=(const Interprete_As_TSCII&) = delete;
};
