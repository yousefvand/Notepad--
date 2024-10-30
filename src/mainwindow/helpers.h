#pragma once

#include <QMenu>
#include <QAction>
#include <QString>
#include <QPrintDialog>
#include <qtabwidget.h>
#include <QRegularExpression>
#include "../document.h"

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

private:
    bool validateDocument(Document* doc, QWidget* parent);
};
