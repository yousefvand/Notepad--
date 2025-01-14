#include "interpret_as_koi8_r.h"
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include "../codeeditor.h"

// Singleton instance
Interpret_As_KOI8_R& Interpret_As_KOI8_R::instance() {
    static Interpret_As_KOI8_R instance;
    return instance;
}

// KOI8-R to Unicode mapping (with explicit casting)
const QMap<uint8_t, QChar> koi8rToUnicode = {
    { uint8_t(0xC0), QChar(0x0410) }, { uint8_t(0xC1), QChar(0x0411) },
    { uint8_t(0xC2), QChar(0x0412) }, { uint8_t(0xC3), QChar(0x0413) },
    { uint8_t(0xC4), QChar(0x0414) }, { uint8_t(0xC5), QChar(0x0415) },
    { uint8_t(0xC6), QChar(0x0416) }, { uint8_t(0xC7), QChar(0x0417) },
    { uint8_t(0xC8), QChar(0x0418) }, { uint8_t(0xC9), QChar(0x0419) },
    { uint8_t(0xCA), QChar(0x041A) }, { uint8_t(0xCB), QChar(0x041B) },
    { uint8_t(0xCC), QChar(0x041C) }, { uint8_t(0xCD), QChar(0x041D) },
    { uint8_t(0xCE), QChar(0x041E) }, { uint8_t(0xCF), QChar(0x041F) },
    { uint8_t(0xD0), QChar(0x0420) }, { uint8_t(0xD1), QChar(0x0421) },
    { uint8_t(0xD2), QChar(0x0422) }, { uint8_t(0xD3), QChar(0x0423) },
    { uint8_t(0xD4), QChar(0x0424) }, { uint8_t(0xD5), QChar(0x0425) },
    { uint8_t(0xD6), QChar(0x0426) }, { uint8_t(0xD7), QChar(0x0427) },
    { uint8_t(0xD8), QChar(0x0428) }, { uint8_t(0xD9), QChar(0x0429) },
    { uint8_t(0xDA), QChar(0x042A) }, { uint8_t(0xDB), QChar(0x042B) },
    { uint8_t(0xDC), QChar(0x042C) }, { uint8_t(0xDD), QChar(0x042D) },
    { uint8_t(0xDE), QChar(0x042E) }, { uint8_t(0xDF), QChar(0x042F) },

    { uint8_t(0xE0), QChar(0x0430) }, { uint8_t(0xE1), QChar(0x0431) },
    { uint8_t(0xE2), QChar(0x0432) }, { uint8_t(0xE3), QChar(0x0433) },
    { uint8_t(0xE4), QChar(0x0434) }, { uint8_t(0xE5), QChar(0x0435) },
    { uint8_t(0xE6), QChar(0x0436) }, { uint8_t(0xE7), QChar(0x0437) },
    { uint8_t(0xE8), QChar(0x0438) }, { uint8_t(0xE9), QChar(0x0439) },
    { uint8_t(0xEA), QChar(0x043A) }, { uint8_t(0xEB), QChar(0x043B) },
    { uint8_t(0xEC), QChar(0x043C) }, { uint8_t(0xED), QChar(0x043D) },
    { uint8_t(0xEE), QChar(0x043E) }, { uint8_t(0xEF), QChar(0x043F) },
    { uint8_t(0xF0), QChar(0x0440) }, { uint8_t(0xF1), QChar(0x0441) },
    { uint8_t(0xF2), QChar(0x0442) }, { uint8_t(0xF3), QChar(0x0443) },
    { uint8_t(0xF4), QChar(0x0444) }, { uint8_t(0xF5), QChar(0x0445) },
    { uint8_t(0xF6), QChar(0x0446) }, { uint8_t(0xF7), QChar(0x0447) },
    { uint8_t(0xF8), QChar(0x0448) }, { uint8_t(0xF9), QChar(0x0449) },
    { uint8_t(0xFA), QChar(0x044A) }, { uint8_t(0xFB), QChar(0x044B) },
    { uint8_t(0xFC), QChar(0x044C) }, { uint8_t(0xFD), QChar(0x044D) },
    { uint8_t(0xFE), QChar(0x044E) }, { uint8_t(0xFF), QChar(0x044F) }
};

// KOI8-R Decoding Logic
QString Interpret_As_KOI8_R::decodeKOI8R(const QByteArray& koi8Data) {
    QString result;
    for (char byte : koi8Data) {
        uint8_t koi8Char = static_cast<uint8_t>(byte);
        result.append(koi8rToUnicode.value(koi8Char, QChar(0xFFFD)));  // U+FFFD for unmapped bytes
    }
    return result;
}

// Execute KOI8-R Interpretation
void Interpret_As_KOI8_R::execute(QPlainTextEdit* editor) {
    if (!editor) return;

    CodeEditor* codeEditor = qobject_cast<CodeEditor*>(editor);
    QString filePath = codeEditor->filePath();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[ERROR] Cannot open file: " << filePath;
        return;
    }

    QByteArray koi8Data = file.readAll();
    file.close();

    QString decodedText = decodeKOI8R(koi8Data);
    codeEditor->setPlainText(decodedText);
    qDebug() << "[DEBUG] KOI8-R Decoding Applied to File:" << filePath;
}
