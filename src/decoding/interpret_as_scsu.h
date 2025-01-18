#pragma once

#include <QString>
#include <QPlainTextEdit>
#include <QByteArray>

class Interpret_As_SCSU {
public:
    static Interpret_As_SCSU& instance();
    void execute(QPlainTextEdit* editor);

private:
    Interpret_As_SCSU() = default;
    ~Interpret_As_SCSU() = default;
    Interpret_As_SCSU(const Interpret_As_SCSU&) = delete;
    Interpret_As_SCSU& operator=(const Interpret_As_SCSU&) = delete;

    QString decodeSCSU(const QByteArray& scsuData);
};
