#pragma once

#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QSettings>
#include <QPointer>

class MainWindow;

class Formatting : public QObject {
    Q_OBJECT

public:
    Formatting(MainWindow* mainWindow);

    void setupActions(QAction* windowsAction,
                      QAction* unixAction,
                      QAction* oldMacAction);

private:
    QPointer<MainWindow> m_mainWindow;
    QAction* m_windowsAction;
    QAction* m_unixAction;
    QAction* m_oldMacAction;
    QActionGroup* m_actionGroup;

    void saveEolFormat(const QString& format);
    QString loadEolFormat();

    // Callback functions for each format
    void onWindowsFormatSelected();
    void onUnixFormatSelected();
    void onOldMacFormatSelected();

    void updateStatusBar(const QString& message);
};
