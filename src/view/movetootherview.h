#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QString>

class MoveToOtherView : public QWidget {
    Q_OBJECT

public:
    explicit MoveToOtherView(QTabWidget *tabWidget, QWidget *parent = nullptr)
        : QWidget(parent), m_tabWidget(tabWidget) {}

    void execute();

private:
    QTabWidget *m_tabWidget;
};
