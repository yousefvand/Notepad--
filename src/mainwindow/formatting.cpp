#include <QDebug>
#include <QStatusBar>
#include <QTextCursor>
#include "formatting.h"
#include "../mainwindow.h"
#include "../settings.h"
#include "../helpers.h"
#include "../codeeditor.h"

Formatting::Formatting(QObject* parent, QTabWidget* documentsTab)
    : QObject(parent), m_documentsTab(documentsTab) {

    m_actionGroup = new QActionGroup(this);
    m_actionGroup->setExclusive(true);  // Only one action can be checked at a time
}

void Formatting::setupActions(QAction* windowsAction, QAction* unixAction, QAction* oldMacAction) {
    // Initialize actions
    m_windowsAction = windowsAction;
    m_unixAction = unixAction;
    m_oldMacAction = oldMacAction;

    // Add actions to the group
    m_actionGroup->addAction(m_windowsAction);
    m_actionGroup->addAction(m_unixAction);
    m_actionGroup->addAction(m_oldMacAction);

    // Connect actions to the unified callback
    connect(m_actionGroup, &QActionGroup::triggered, this, &Formatting::onEolFormatChanged);

    // Load the saved EOL format from settings and set the corresponding action checked
    QString savedFormat = Settings::instance()->loadSetting("EOL", "Type", "Unix").toString();

    if (savedFormat == "Windows") {
        m_windowsAction->setChecked(true);
        updateStatusBar("EOL format: Windows");
    } else if (savedFormat == "Unix") {
        m_unixAction->setChecked(true);
        updateStatusBar("EOL format: Unix");
    } else if (savedFormat == "OldMac") {
        m_oldMacAction->setChecked(true);
        updateStatusBar("EOL format: Old Mac");
    }
}

void Formatting::onEolFormatChanged() {
    QAction* selectedAction = m_actionGroup->checkedAction();
    QString format;

    if (selectedAction == m_windowsAction) {
        format = "Windows";
        applyEOL(Windows);
    } else if (selectedAction == m_unixAction) {
        format = "Unix";
        applyEOL(Unix);
    } else if (selectedAction == m_oldMacAction) {
        format = "OldMac";
        applyEOL(OldMac);
    }

    // Save the selected format to Settings
    Settings::instance()->saveSetting("EOL", "Type", format);

    // Update status bar
    updateStatusBar(QString("EOL format changed to: %1").arg(format));
}

void Formatting::updateStatusBar(const QString& message) {
    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parent());
    if (mainWindow && mainWindow->statusBar()) {
        mainWindow->statusBar()->showMessage(message, 2000);  // Show message for 2 seconds
    } else {
        qDebug() << "Warning: Status bar is not available.";
    }
}

void Formatting::applyEOL(EOLType eolType) {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;

    QPlainTextEdit* editor = doc->editor();
    QString eol;

    // Determine the EOL based on the selected option
    switch (eolType) {
    case Windows:
        eol = "\r\n";  // Windows EOL
        break;
    case Unix:
        eol = "\n";  // Unix EOL
        break;
    case OldMac:
        eol = "\r";  // Old Mac EOL
        break;
    }

    // Retrieve the current content
    QString content = editor->toPlainText();

    // Use a single replacement to convert all line breaks
    content.replace(QRegularExpression("\r\n|\r|\n"), eol);

    // Replace the content of the document in one go
    editor->clear();  // Clear the current content
    editor->setPlainText(content);  // Set the modified content

    // Mark the document as modified
    doc->setModified(true);
}

void Formatting::trimTrailingSpace() {
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;  // Ensure document is valid

    QTextDocument* textDoc = doc->editor()->document();
    QTextCursor cursor(textDoc);

    cursor.beginEditBlock();  // Group all changes into a single undo step

    for (QTextBlock block = textDoc->begin(); block != textDoc->end(); block = block.next()) {
        QString trimmedText = block.text().trimmed();  // Remove trailing spaces
        cursor.setPosition(block.position());
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.insertText(trimmedText);
    }

    cursor.endEditBlock();  // End the grouped undo step
}

void Formatting::trimLeadingSpace() {
    // Use a static QRegularExpression to avoid repeated creation.
    static const QRegularExpression leadingSpaceRegex("^\\s+");

    // Get the current document using helper function.
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;  // Ensure the document is valid.

    QTextDocument* textDoc = doc->editor()->document();
    QTextCursor cursor(textDoc);

    cursor.beginEditBlock();  // Begin grouped undo step.

    // Iterate over all blocks (lines) in the document.
    for (QTextBlock block = textDoc->begin(); block != textDoc->end(); block = block.next()) {
        QString text = block.text();

        // Use the static QRegularExpression to trim leading spaces.
        QString trimmedText = text.remove(leadingSpaceRegex);

        cursor.setPosition(block.position());
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.insertText(trimmedText);
    }

    cursor.endEditBlock();  // End grouped undo step.
}

void Formatting::trimLeadingAndTrailingSpace() {
    // Use a static QRegularExpression to avoid unnecessary creation.
    static const QRegularExpression leadingTrailingSpaceRegex("^\\s+|\\s+$");

    // Get the current document using the helper function.
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;  // Ensure the document is valid.

    QTextDocument* textDoc = doc->editor()->document();
    QTextCursor cursor(textDoc);

    cursor.beginEditBlock();  // Begin grouped undo step.

    // Iterate over all lines (blocks) in the document.
    for (QTextBlock block = textDoc->begin(); block != textDoc->end(); block = block.next()) {
        QString text = block.text();

        // Use the static regular expression to trim both leading and trailing spaces.
        QString trimmedText = text.remove(leadingTrailingSpaceRegex);

        cursor.setPosition(block.position());
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.insertText(trimmedText);
    }

    cursor.endEditBlock();  // End grouped undo step.
}

void Formatting::eolToSpace() {
    // Get the current document using the helper function.
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) return;  // Ensure the document is valid.

    QTextDocument* textDoc = doc->editor()->document();
    QString content = textDoc->toPlainText();  // Retrieve the entire text as plain text.

    // Replace all EOL characters (Windows, Unix, OldMac) with spaces.
    content.replace("\r\n", " ")  // Windows EOL
        .replace('\n', ' ')    // Unix EOL
        .replace('\r', ' ');   // Old Mac EOL

    // Replace the document's content with the modified content.
    QTextCursor cursor(textDoc);
    cursor.beginEditBlock();  // Start grouped undo step.
    cursor.select(QTextCursor::Document);
    cursor.removeSelectedText();  // Clear existing text.
    cursor.insertText(content);   // Insert modified text.
    cursor.endEditBlock();  // End grouped undo step.
}

void Formatting::tabToSpace(int spaces) {
    if (spaces <= 0) {
        qDebug() << "Invalid number of spaces. Must be greater than 0.";
        return;
    }

    // Get the current document
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        qDebug() << "No active document.";
        return;
    }

    // Access the text editor
    auto* editor = doc->editor();
    if (!editor) {
        qDebug() << "No valid editor found.";
        return;
    }

    // Create the space string dynamically
    QString spaceString(spaces, ' ');

    // Use a QTextCursor to navigate through the document content
    QTextCursor cursor(editor->textCursor());
    cursor.movePosition(QTextCursor::Start);

    while (!cursor.atEnd()) {
        cursor.select(QTextCursor::LineUnderCursor);
        QString lineText = cursor.selectedText();

        // Replace tabs with spaces
        QString modifiedLine = lineText.replace("\t", spaceString);

        // Replace the line in the document
        cursor.insertText(modifiedLine);
        cursor.movePosition(QTextCursor::NextBlock);
    }

    qDebug() << "Replaced all tabs with" << spaces << "spaces.";
}

void Formatting::spaceToTab(int spaces) {
    if (spaces <= 0) {
        qDebug() << "Invalid number of spaces. Must be greater than 0.";
        return;
    }

    // Get the active document and its editor
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        qDebug() << "No active document.";
        return;
    }

    QPlainTextEdit* editor = doc->editor();
    if (!editor) {
        qDebug() << "No valid editor found.";
        return;
    }

    // Create the space string to match and replace
    QString spaceString(spaces, ' ');

    // Start an edit block to batch changes (improves undo/redo performance)
    QTextCursor cursor(editor->document());
    cursor.beginEditBlock();

    // Iterate through each block (line) in the document
    for (QTextBlock block = editor->document()->begin(); block.isValid(); block = block.next()) {
        QString lineText = block.text();

        // Replace all occurrences of the specified number of spaces with a tab
        QString modifiedLine = lineText.replace(spaceString, "\t");

        // Update the block content
        cursor.setPosition(block.position());
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.insertText(modifiedLine);
    }

    cursor.endEditBlock();  // End the edit block

    qDebug() << "Replaced every" << spaces << "spaces with tabs.";
}

void Formatting::spaceToTabLeading(int spaces) {
    if (spaces <= 0) {
        qDebug() << "Invalid number of spaces. Must be greater than 0.";
        return;
    }

    // Get the current document
    Document* doc = Helpers::getCurrentDocument(m_documentsTab);
    if (!doc) {
        Helpers::showInformationMessage("No active document.");
        return;
    }

    // Access the text editor
    auto* editor = doc->editor();
    if (!editor) {
        Helpers::showInformationMessage("No valid editor found.");
        return;
    }

    // Create the space pattern for leading spaces
    QString spacePattern = QString("^%1").arg(QString(spaces, ' '));

    // Prepare the text cursor to modify the document
    QTextCursor cursor(editor->document());
    cursor.beginEditBlock();  // Start an edit block for batch operations

    while (!cursor.atEnd()) {
        cursor.select(QTextCursor::LineUnderCursor);
        QString lineText = cursor.selectedText();

        // Replace leading spaces with tabs
        QString modifiedLine = lineText.replace(QRegularExpression(spacePattern), "\t");

        // Update the line with the modified text
        cursor.insertText(modifiedLine);
        cursor.movePosition(QTextCursor::NextBlock);
    }

    cursor.endEditBlock();  // End the edit block
    qDebug() << "Replaced leading spaces with tabs.";
}

