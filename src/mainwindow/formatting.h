#pragma once

#include <QObject>
#include <QActionGroup>
#include <QSettings>
#include <QTabWidget>

class MainWindow;

class Formatting : public QObject {
    Q_OBJECT

public:
     Formatting(QObject* parent, QTabWidget* documentsTab);

    enum EOLType { Windows, Unix, OldMac };
    Q_ENUM(EOLType)
    void applyEOL(EOLType eolType);
    void setupActions(QAction* windowsAction, QAction* unixAction, QAction* oldMacAction);
    void trimTrailingSpace();
    void trimLeadingSpace();
    void trimLeadingAndTrailingSpace();
    void eolToSpace();
    void tabToSpace(int spaces);
    void spaceToTab(int spaces);
    void spaceToTabLeading(int spaces);

private:
    MainWindow* m_mainWindow;
    QActionGroup* m_actionGroup;
    QAction* m_windowsAction;
    QAction* m_unixAction;
    QAction* m_oldMacAction;

    void updateStatusBar(const QString& message);
    void saveEolFormat(const QString& format);

    QString loadEolFormat();
    QTabWidget* m_documentsTab;

private slots:
    void onEolFormatChanged();  // Unified callback for EOL format change
};
