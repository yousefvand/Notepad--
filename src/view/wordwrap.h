#pragma once

#include <QObject>
#include <QPlainTextEdit>
#include <QTextOption>

class WordWrap : public QObject {
    Q_OBJECT

public:
    explicit WordWrap(QObject* parent = nullptr);

    // Toggle word wrap for a given editor
    void toggle(QPlainTextEdit* editor);

    // Check if word wrap is enabled
    bool isWordWrapEnabled(QPlainTextEdit* editor) const;
};

