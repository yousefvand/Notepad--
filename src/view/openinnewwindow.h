#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QString>

class OpenInNewWindow : public QWidget {
    Q_OBJECT

public:
    explicit OpenInNewWindow(QTabWidget* tabWidget, QWidget* parent = nullptr);

    void execute();

private:
    QTabWidget* m_tabWidget;
};
