#include "codeeditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QTabWidget>
#include "settings.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), lineNumberArea(new LineNumberArea(this)) {

    QTabWidget* doc = qobject_cast<QTabWidget*>(parent);
    m_documentsTab = doc;
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

    m_useTabs = Settings::instance()->loadSetting("Indentation", "Option", "Tabs") == "Tabs";
    m_indentationWidth = Settings::instance()->loadSetting("Indentation", "Size", "1").toInt();
    m_showTabs = Settings::instance()->loadSetting("View", "ShowTabs", "false") == "true";
    m_showSpaces = Settings::instance()->loadSetting("View", "ShowSpaces", "false") == "true";
    m_showEOL = Settings::instance()->loadSetting("View", "ShowEOL", "false") == "true";
    m_tabWidth = Settings::instance()->loadSetting("View", "TabWidth", "4").toInt();
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
    m_useTabs = useTabs;
    m_indentationWidth = indentationWidth;
    QTextCursor cursor = textCursor();
    QString indentation = useTabs ? QString(indentationWidth, '\t')
                                  : QString(indentationWidth, ' ');
    cursor.insertText(indentation);
}

QTabWidget* CodeEditor::DocumentsTab() {
    return m_documentsTab;
}

void CodeEditor::highlightAllOccurrences(const QString& keyword) {
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextCursor cursor(document());
    QTextCursor highlightCursor(document());
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::cyan);

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = document()->find(keyword, cursor);
        if (!cursor.isNull()) {
            QTextEdit::ExtraSelection selection;
            selection.cursor = cursor;
            selection.format = highlightFormat;
            extraSelections.append(selection);
        }
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::goToLineInText(int lineNumber) {
    if (lineNumber < 1 || lineNumber > document()->blockCount()) {
        qWarning() << "Line number: " << lineNumber << ". Line number is out of range.";
        return;
    }

    QTextCursor cursor(document()->findBlockByNumber(lineNumber - 1));
    setTextCursor(cursor);
    centerCursor(); // Ensure the cursor is centered in the view

    qDebug() << "Moved to line:" << lineNumber;

    // Log current cursor position
    qDebug() << "Cursor position after move:" << textCursor().position();
}

void CodeEditor::gotoLineInEditor(int lineNumber) {
    // Ensure the line number is within the valid range
    if (lineNumber > 0 && lineNumber <= blockCount()) {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber - 1);
        setTextCursor(cursor);
        centerCursor();
    } else {
        qWarning() << "Invalid Line Number: " << lineNumber << " .The specified line is out of range.";
    }
}

void CodeEditor::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Tab) {
        // User pressed Tab
        QTextCursor cursor = textCursor();

        // Determine the indentation string (tabs or spaces)
        QString indentation = m_useTabs ? QString(m_indentationWidth, '\t')
                                        : QString(m_indentationWidth, ' ');

        // Insert the indentation
        cursor.insertText(indentation);

        // Prevent further handling of the Tab key
        return;
    } else if (event->key() == Qt::Key_Backtab) {
        // User pressed Shift+Tab (outdent logic)
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);

        QString blockText = cursor.selectedText();
        if (m_useTabs && blockText.startsWith(QString(m_indentationWidth, '\t'))) {
            cursor.removeSelectedText();
        } else if (!m_useTabs && blockText.startsWith(QString(m_indentationWidth, ' '))) {
            cursor.removeSelectedText();
        }

        // Prevent further handling of Shift+Tab
        return;
    }

    // Pass other keys to the default handler
    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::setShowTabs(bool enabled) {
    if (m_showTabs != enabled) {
        m_showTabs = enabled;
        viewport()->update(); // Redraw the editor to show or hide tab symbols
    }
}

void CodeEditor::setShowSpaces(bool enabled) {
    if (m_showSpaces != enabled) {
        m_showSpaces = enabled;
        viewport()->update();
    }
}

void CodeEditor::setShowEOL(bool enabled) {
    if (m_showEOL != enabled) {
        m_showEOL = enabled;
        viewport()->update();
    }
}

bool CodeEditor::showTabs() const {
    return m_showTabs;
}

bool CodeEditor::showSpaces() const {
    return m_showSpaces;
}

// TODO: Implement in UI
void CodeEditor::setTabWidth(int width = 4) {
    m_tabWidth = width;
    viewport()->update(); // Trigger a repaint to apply the new width
}

void CodeEditor::paintEvent(QPaintEvent *event) {
    QPlainTextEdit::paintEvent(event);

    QPainter painter(viewport());
    painter.setPen(Qt::gray);

    QTextBlock block = document()->firstBlock();
    QFontMetrics metrics(font());

    while (block.isValid()) {
        QString text = block.text();
        int blockStart = block.position();
        QTextCursor blockCursor(block);

        // Handle Tabs and Spaces in the line
        for (int i = 0; i < text.length(); ++i) {
            if (text[i] == '\t' && m_showTabs) {
                // Move the cursor to the tab character
                blockCursor.setPosition(blockStart + i);

                // Get the rectangle of the cursor position
                QRect rect = cursorRect(blockCursor);

                // Adjust the position for the tab symbol
                QPoint position(rect.left() + metrics.ascent(), rect.top() + metrics.ascent());

                // Draw the tab symbol
                painter.drawText(position, "→");
            } else if (text[i] == ' ' && m_showSpaces) {
                // Move the cursor to the space character
                blockCursor.setPosition(blockStart + i);

                // Get the rectangle of the cursor position
                QRect rect = cursorRect(blockCursor);

                // Adjust the position for the space dot
                QPoint position(
                    rect.left() + metrics.horizontalAdvance(' ') / 4,  // Push right slightly
                    rect.top() + metrics.ascent() / 2 + metrics.height() / 3  // Vertically center the dot lower
                    );

                // Draw the space as a dot
                painter.drawText(position, ".");
            }
        }

        // Add the EOL character if enabled
        if (m_showEOL) {
            blockCursor.setPosition(blockStart + text.length());  // Move to the end of the line
            QRect rect = cursorRect(blockCursor);

            // Position for the EOL character
            QPoint position(rect.left() + metrics.horizontalAdvance(' '), rect.top() + metrics.ascent());

            // Draw the EOL character (e.g., "↵")
            painter.drawText(position, "↵");
        }

        block = block.next();
    }
}
