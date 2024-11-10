#pragma once

#include <QTabWidget>
#include <qplaintextedit.h>

class TextOperations
{
public:
    TextOperations(QTabWidget* documentsTab);
    void handleCheckboxToggle(bool checked);

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void deleteText();
    void selectAll();
    void convertToUpperCase();
    void convertToLowerCase();
    void duplicateCurrentLine();
    void deleteCurrentLine();
    void moveLineUp();
    void moveLineDown();


private:
    QString m_eolType;
    QTabWidget* m_documentsTab;
    void selectRestOfLine(QPlainTextEdit* editor);
};

