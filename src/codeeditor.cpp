#include "codeeditor.h"
#include <QPainter>
#include <QTextBlock>
#include "mainwindow/helpers.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), lineNumberArea(new LineNumberArea(this)) {

    QPalette p = this->palette();
    p.setColor(QPalette::Text, Qt::black);
    this->setPalette(p);
    this->setStyleSheet("QPlainTextEdit { background-color: white; }");

    QFont font;
    font.setFamily("sans serif");
    font.setFixedPitch(true);
    font.setPointSize(12);
    this->setFont(font);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 5 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int) {
    int marginWidth = lineNumberAreaWidth();
    setViewportMargins(marginWidth, 0, 0, 0);

    qDebug() << "Line number area width updated to:" << marginWidth;

    // Ensure the viewport is refreshed immediately
    viewport()->update();
    updateGeometry();
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (!document()) return;  // Ensure the document exists

    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));

    // Force recalculation of the layout
    document()->adjustSize();
    updateGeometry();
    viewport()->update();
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::yellow).lighter(160);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
    qDebug() << "Highlighted current line at position:" << textCursor().position();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    int currentLineNumber = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            if (blockNumber == currentLineNumber) {
                QFont boldFont = painter.font();
                boldFont.setBold(true);
                painter.setFont(boldFont);
                painter.setPen(Qt::blue);
            } else {
                painter.setFont(QFont());
                painter.setPen(Qt::black);
            }

            painter.drawText(-5, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight | Qt::AlignVCenter, number);

            QTextLayout *layout = block.layout();
            for (int i = 0; i < layout->lineCount(); ++i) {
                QTextLine line = layout->lineAt(i);
                int lineStartPosition = block.position() + line.textStart();
                int lineEndPosition = lineStartPosition + line.textLength();

                if (textCursor().position() >= lineStartPosition && textCursor().position() <= lineEndPosition) {
                    painter.setPen(Qt::red);
                } else {
                    painter.setPen(Qt::blue);
                }

                int lineTop = top + static_cast<int>(line.y());
                int lineBottom = lineTop + fontMetrics().height();

                if (lineBottom > event->rect().bottom()) {
                    break;
                }

                painter.drawLine(0, lineBottom, lineNumberArea->width(), lineBottom);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::applyIndentation(bool useTabs, int indentationWidth) {
    // TODO: Implement indentation
}
