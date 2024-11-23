#include <QObject>
#include <QAction>
#include <QTabWidget>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include "helpers.h"
#include "document.h"
#include "codeeditor.h"
#include "search/searchoptions.h"

bool Helpers::isUntitledDocument(const QString& title) {
    // Static QRegularExpression to avoid repeated creation
    static const QRegularExpression regex(
        R"((?:&?U&?n&?t&?i&?t&?l&?e&?d&?) (?:&?D&?o&?c&?u&?m&?e&?n&?t))"
        );

    return regex.match(title).hasMatch();
}

void Helpers::RemoveMe(QTabWidget* documentsTab) {
    if (documentsTab->count() > 0) {
        QWidget* firstTab = documentsTab->widget(0);
        documentsTab->removeTab(0);
        firstTab->deleteLater();  // Safe deletion
    }
}

void Helpers::AddDefaultTab(QTabWidget* documentsTab) {
    Document* defaultDoc = new Document("Untitled Document", documentsTab);
    documentsTab->addTab(defaultDoc, "Untitled Document");
    documentsTab->setCurrentWidget(defaultDoc);
}

void Helpers::CloseTab(QTabWidget* documentsTab, int index) {
    QWidget* tab = documentsTab->widget(index);
    if (tab) {
        documentsTab->removeTab(index);
        tab->deleteLater();  // Safe deletion
        qDebug() << "Tab at index" << index << "closed.";
    }
}

void Helpers::zMenu(QMenu* menuLanguage, QObject* receiver) {
    QMenu* menuZ = new QMenu("z", menuLanguage);
    QAction* actionZ80 = new QAction("Z80", menuZ);

    menuZ->addAction(actionZ80);
    menuLanguage->addMenu(menuZ);

    QObject::connect(actionZ80, &QAction::triggered, receiver, [receiver]() {
        QMetaObject::invokeMethod(receiver, "onActionZ80Triggered");
    });
}

bool Helpers::validateDocument(Document* doc, QWidget* parent) {
    if (!doc) {
        QMessageBox::warning(parent, "Print Error", "Failed to retrieve the document.");
        return false;
    }
    return true;
}

void Helpers::Print(QTabWidget* documentsTab, QWidget* parent) {
    // Create a printer object
    QPrinter printer;

    // Show a print dialog to the user
    QPrintDialog printDialog(&printer, parent);
    printDialog.setWindowTitle("Print Document");

    // Check if the user accepted the dialog
    if (printDialog.exec() != QDialog::Accepted) {
        QMessageBox::information(parent, "Print", "Printing was canceled.");
        return;
    }

    // Get the currently active document from the tab widget
    int currentTabIndex = documentsTab->currentIndex();
    if (currentTabIndex == -1) {
        QMessageBox::warning(parent, "Print Error", "No document to print.");
        return;
    }

    // Retrieve and validate the document
    Document* currentDoc = qobject_cast<Document*>(documentsTab->widget(currentTabIndex));
    if (!validateDocument(currentDoc, parent)) {
        return;
    }

    // Print the content of the editor associated with the document
    QTextDocument* textDoc = currentDoc->editor()->document();
    textDoc->print(&printer);

    QMessageBox::information(parent, "Print", "Document printed successfully.");
}

Document* Helpers::getCurrentDocument(QTabWidget* documentsTab) {
    if (!documentsTab) return nullptr;
    if (!documentsTab) {
        qDebug() << "Error: documentsTab is not initialized!";
        return nullptr;
    }
    int currentIndex = documentsTab->currentIndex();
    if (currentIndex == -1) {
        qDebug() << "No active document.";
        return nullptr;
    }

    return qobject_cast<Document*>(documentsTab->widget(currentIndex));
}

void Helpers::showInformationMessage(const QString& message) {
    QMessageBox::information(nullptr, "Information", message, QMessageBox::Ok);
}

void Helpers::notImplemented(QWidget* parent) {
    QString hardcodedMessage = "Sorry, this option is not available now.\nIt will be implemented in future versions.";
    QMessageBox::information(parent, "Information", hardcodedMessage, QMessageBox::Ok);
}

bool Helpers::isValidRegularExpression(const QString& pattern) {
    QRegularExpression regex(pattern);
    return regex.isValid();
}

int Helpers::countKeywordsInLine(const QString& line, const SearchOptions& options) {
    QString keyword = options.keyword;
    if (keyword.isEmpty()) {
        return 0;
    }

    QString pattern = options.matchWholeWord
                          ? QString("\\b%1\\b").arg(QRegularExpression::escape(keyword))
                          : QRegularExpression::escape(keyword);

    QRegularExpression regex(pattern, options.matchCase
                                          ? QRegularExpression::NoPatternOption
                                          : QRegularExpression::CaseInsensitiveOption);

    int count = 0;
    QRegularExpressionMatchIterator it = regex.globalMatch(line);
    while (it.hasNext()) {
        it.next();
        count++;
    }

    return count;
}

// FIXME: If user search for "hello" and a match found as "Hello" in retun string "hello" would return.
QString Helpers::highlightKeywords(const QString& line, const SearchOptions& options) {
    QString keyword = options.keyword;
    if (keyword.isEmpty()) {
        return line; // No keyword to highlight
    }

    // Build the regex pattern
    QString pattern = options.matchWholeWord
                          ? QString("\\b%1\\b").arg(QRegularExpression::escape(keyword)) // Whole word matching
                          : QRegularExpression::escape(keyword); // Partial match

    // Configure case sensitivity
    QRegularExpression regex(pattern, options.matchCase
                                          ? QRegularExpression::NoPatternOption
                                          : QRegularExpression::CaseInsensitiveOption);

    // Process the line and replace matches with highlighted versions
    QString highlightedLine;
    int lastIndex = 0;

    QRegularExpressionMatchIterator it = regex.globalMatch(line);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        // Append the part before the match
        highlightedLine.append(line.mid(lastIndex, match.capturedStart() - lastIndex));

        // Append the highlighted match
        highlightedLine.append("<span style='background-color: yellow; color: black;'>");
        highlightedLine.append(match.captured());
        highlightedLine.append("</span>");

        // Update the last index to the end of the match
        lastIndex = match.capturedEnd();
    }

    // Append the remaining part of the line
    highlightedLine.append(line.mid(lastIndex));

    // Ensure no `<highlight>` tags exist
    highlightedLine.replace("<highlight>", "");
    highlightedLine.replace("</highlight>", "");

    return highlightedLine;
}
