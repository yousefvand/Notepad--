#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QString>

class MoveToNewView : public QWidget {
    Q_OBJECT

public:
    explicit MoveToNewView(QTabWidget *tabWidget, QWidget *parent = nullptr);

    void execute();

private:
    QTabWidget *m_tabWidget;
};

