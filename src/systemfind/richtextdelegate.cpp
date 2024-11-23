#include "richtextdelegate.h"
#include <QPainter>
#include <QTextDocument>

RichTextDelegate::RichTextDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void RichTextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();

    QString text = index.data().toString();

    // Replace <highlight> tags with HTML for yellow background highlight
    text.replace("<highlight>", "<span style=\"background-color: yellow;\">");
    text.replace("</highlight>", "</span>");

    QTextDocument doc;
    doc.setHtml(text);
    doc.setDefaultFont(option.font);

    // Calculate indentation based on item level and apply line spacing
    int indentLevel = index.model()->data(index, Qt::UserRole + 1).toInt(); // Assumed indent level stored in UserRole + 1
    int indentWidth = 20;  // Adjust width per indent level
    int spacing = 30;      // Space between lines to prevent overlap

    QRect rect = option.rect;
    rect.adjust(indentLevel * indentWidth, 0, 0, 0); // Offset based on indentation level

    // Draw the selection background if the item is selected
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    // Set document width to fit within item rect
    doc.setTextWidth(rect.width());

    // Draw each line separately with adjusted line spacing
    int yOffset = rect.top();
    for (int i = 0; i < doc.lineCount(); ++i) {
        QRect lineRect = rect;
        lineRect.moveTop(yOffset);

        // Draw the document content
        painter->translate(lineRect.topLeft());
        doc.drawContents(painter, QRectF(0, 0, lineRect.width(), spacing));
        painter->translate(-lineRect.topLeft()); // Reset translation for next line

        // Update yOffset for the next line
        yOffset += spacing;
    }

    painter->restore();
}

QSize RichTextDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QTextDocument doc;
    QString text = index.data().toString();

    // Set HTML with line spacing
    text.replace("<highlight>", "<span style=\"background-color: yellow;\">");
    text.replace("</highlight>", "</span>");
    doc.setHtml(text);

    int indentLevel = index.model()->data(index, Qt::UserRole + 1).toInt();
    int indentWidth = 20;
    int spacing = 30;

    // Account for indent and extra spacing in the height
    QSize baseSize(doc.idealWidth(), doc.size().height());
    return QSize(baseSize.width() + indentLevel * indentWidth, baseSize.height() + spacing);
}
