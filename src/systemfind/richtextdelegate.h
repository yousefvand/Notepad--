#pragma once

#include <QStyledItemDelegate>

class RichTextDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit RichTextDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
