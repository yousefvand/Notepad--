#pragma once

#include <QStyledItemDelegate>
#include <QTextDocument>

class SystemTextDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit SystemTextDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
