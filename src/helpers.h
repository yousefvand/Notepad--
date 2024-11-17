#pragma once

#include <QMenu>
#include <QAction>
#include <QString>
#include <QPrintDialog>
#include <QTabWidget>
#include <QMessageBox>
#include <QRegularExpression>
#include "document.h"
#include "search/searchoptions.h"

class Document;

class Helpers {
public:
    static void RemoveMe(QTabWidget* documentsTab);
    static void AddDefaultTab(QTabWidget* documentsTab);
    static bool isUntitledDocument(const QString& title);
    static void CloseTab(QTabWidget* documentsTab, int index);
    static void zMenu(QMenu* menuLanguage, QObject* receiver);
    void Print(QTabWidget* documentsTab, QWidget* parent);
    static Document* getCurrentDocument(QTabWidget* documentsTab);
    static void showInformationMessage(const QString& message);
    static void notImplemented(QWidget* parent = nullptr);
    static void gotoLineInText(QWidget* parent, CodeEditor* editor);
    static void gotoLineInEditor(QWidget* parent, CodeEditor* editor);
    static bool isValidRegularExpression(const QString& pattern);
    static int  countKeywordsInLine(const QString &line, const SearchOptions &searchOptions);
    static QString highlightKeywords(const QString& line, const SearchOptions& options);

private:
    bool validateDocument(Document* doc, QWidget* parent);
};
