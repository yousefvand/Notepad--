#include "systemtextdelegate.h"
#include <QPainter>
#include <QFontMetrics>
#include <QTextDocument>

SystemTextDelegate::SystemTextDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void SystemTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) return;

    // Retrieve content from the model
    QString content = index.data(Qt::DisplayRole).toString();
    QString keyword = index.data(Qt::UserRole).toString();

    // Highlight keywords dynamically if plain text
    if (!content.contains("<span") && !keyword.isEmpty()) {
        content.replace(
            keyword,
            QString("<span style='background-color: yellow;'>%1</span>").arg(keyword),
            Qt::CaseInsensitive);
    }

    painter->save();

    // Clip the painter to the item's rectangle to prevent overlapping
    QRect rect = option.rect;
    painter->setClipRect(rect);

    if (content.contains("<span")) {
        // Render HTML using QTextDocument
        QTextDocument doc;
        doc.setHtml(content);
        doc.setTextWidth(rect.width());

        // Translate the painter to align with the item's rectangle
        painter->translate(rect.topLeft());
        doc.drawContents(painter);
    } else {
        // Elide plain text if it exceeds the available space
        QFontMetrics metrics(option.font);
        QString elidedText = metrics.elidedText(content, Qt::ElideRight, rect.width());

        // Draw the elided text
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, elidedText);
    }

    painter->restore();
}

QSize SystemTextDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (!index.isValid()) return QStyledItemDelegate::sizeHint(option, index);

    // Retrieve content from the model
    QString content = index.data(Qt::DisplayRole).toString();

    if (content.contains("<span")) {
        // Calculate height dynamically for HTML content
        QTextDocument doc;
        doc.setHtml(content);
        doc.setTextWidth(option.rect.width());
        return QSize(option.rect.width(), doc.size().height());
    } else {
        // Fallback for plain text
        QFontMetrics metrics(option.font);
        int height = metrics.height();
        return QSize(option.rect.width(), height);
    }
}
