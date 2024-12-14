#include "wordwrap.h"

WordWrap::WordWrap(QObject* parent)
    : QObject(parent) {}

void WordWrap::toggle(QPlainTextEdit* editor) {
    if (!editor) return;

    QTextOption option = editor->document()->defaultTextOption();
    if (option.wrapMode() == QTextOption::NoWrap) {
        option.setWrapMode(QTextOption::WordWrap);
    } else {
        option.setWrapMode(QTextOption::NoWrap);
    }
    editor->document()->setDefaultTextOption(option);
}

bool WordWrap::isWordWrapEnabled(QPlainTextEdit* editor) const {
    if (!editor) return false;

    return editor->document()->defaultTextOption().wrapMode() == QTextOption::WordWrap;
}
