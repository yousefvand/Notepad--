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
    m_showAllCharacters = Settings::instance()->loadSetting("View", "ShowAllCharacters", "false") == "true";
    m_showIndentGuide = Settings::instance()->loadSetting("View", "ShowIndentGuide", "false") == "true";
    m_showWrapSymbol = Settings::instance()->loadSetting("View", "ShowWrapSymbol", "false") == "true";
    m_tabWidth = Settings::instance()->loadSetting("View", "TabWidth", "4").toInt();
    m_showMathRendering = Settings::instance()->loadSetting("View", "MathRendering", "false") == "true";

    if (m_showAllCharacters) {
        m_showTabs = true;
        m_showSpaces = true;
        m_showEOL = true;
    }
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

void CodeEditor::setShowAllCharacters(bool enabled) {
    if (m_showAllCharacters != enabled) {
        m_showAllCharacters = enabled;
        m_showTabs = enabled;
        m_showSpaces = enabled;
        m_showEOL = enabled;
        viewport()->update();
    }
}

void CodeEditor::setShowIndentGuide(bool enabled) {
    if (m_showIndentGuide != enabled) {
        m_showIndentGuide = enabled;
        viewport()->update();
    }
}

void CodeEditor::setShowWrapSymbol(bool enabled) {
    if (m_showWrapSymbol != enabled) {
        m_showWrapSymbol = enabled;
        viewport()->update();
    }
}

// TODO: Implement in UI
void CodeEditor::setTabWidth(int width = 4) {
    m_tabWidth = width;
    viewport()->update(); // Trigger a repaint to apply the new width
}

void CodeEditor::setShowMathRendering(bool enabled) {
    Q_UNUSED(enabled);
    // TODO: Implement
}

void CodeEditor::paintTabs(QPainter& painter, const QTextBlock& block, int top) {
    QTextLayout* layout = block.layout();
    if (!layout) return;

    QFontMetrics metrics(font());
    QString text = block.text();

    for (int i = 0; i < text.size(); ++i) {
        if (text.at(i) == '\t') {
            QTextLine line = layout->lineForTextPosition(i);
            if (!line.isValid()) continue;

            int x = line.cursorToX(i) + contentOffset().x();
            QPoint position(x + metrics.ascent(), top + metrics.ascent());
            painter.drawText(position, "→");
        }
    }
}

void CodeEditor::paintSpaces(QPainter& painter, const QTextBlock& block, int top) {
    QTextLayout* layout = block.layout();
    if (!layout) return;

    QFontMetrics metrics(font());
    QString text = block.text();

    for (int i = 0; i < text.size(); ++i) {
        if (text.at(i) == ' ') {
            QTextLine line = layout->lineForTextPosition(i);
            if (!line.isValid()) continue;

            int x = line.cursorToX(i) + contentOffset().x();
            QPoint position(
                x + metrics.horizontalAdvance(' ') / 4,
                top + metrics.ascent() / 2 + metrics.height() / 3
                );
            painter.drawText(position, ".");
        }
    }
}

void CodeEditor::paintEOL(QPainter& painter, const QTextBlock& block, int top, int bottom) {
    Q_UNUSED(bottom);
    QFontMetrics metrics(font());
    QString text = block.text();
    QTextCursor blockCursor(block);

    blockCursor.setPosition(block.position() + text.length());
    QRect rect = cursorRect(blockCursor);

    QPoint position(rect.left() + metrics.horizontalAdvance(' '), top + metrics.ascent());
    painter.drawText(position, "↵");
}

void CodeEditor::paintIndentGuides(QPainter& painter, const QTextBlock& block, int top, int bottom) {
    QFontMetrics metrics(font());
    QString blockText = block.text();
    int indentWidth = metrics.horizontalAdvance(' ') * 4;  // Assuming 4 spaces per indent level

    int horizontalOffset = 0;
    for (const QChar& ch : blockText) {
        if (ch == '\t') {
            horizontalOffset += indentWidth;
        } else if (ch == ' ') {
            horizontalOffset += metrics.horizontalAdvance(' ');
        } else {
            break;
        }
    }

    if (horizontalOffset > 0) {
        int x = horizontalOffset + contentOffset().x();
        painter.drawLine(QPoint(x, top), QPoint(x, bottom));
    }
}

void CodeEditor::paintWrapSymbols(QPainter& painter, const QTextBlock& block, int top, int bottom) {
    Q_UNUSED(bottom);
    QTextLayout* layout = block.layout();
    if (!layout || layout->lineCount() <= 1) return;

    QFontMetrics metrics(font());

    for (int i = 1; i < layout->lineCount(); ++i) {
        QTextLine line = layout->lineAt(i);

        int y = top + line.y() + metrics.ascent();
        int x = viewport()->width() - metrics.horizontalAdvance("↩") - 5;

        painter.drawText(QPoint(x, y), "↩");
    }
}

void CodeEditor::paintEvent(QPaintEvent* event) {
    QPlainTextEdit::paintEvent(event);

    QPainter painter(viewport());
    painter.setPen(Qt::gray);

    QTextBlock block = document()->firstBlock();
    QFontMetrics metrics(font());

    // Draw indent guides if enabled
    if (m_showIndentGuide) {
        while (block.isValid()) {
            if (block.isVisible()) {
                QRect blockRect = blockBoundingGeometry(block).translated(contentOffset()).toRect();
                paintIndentGuides(painter, block, blockRect.top(), blockRect.bottom());
            }
            block = block.next();
        }
        block = document()->firstBlock();  // Reset block for next drawing loop
    }

    // Handle Tabs, Spaces, EOL, and Wrap Symbols
    while (block.isValid()) {
        QRect blockRect = blockBoundingGeometry(block).translated(contentOffset()).toRect();

        if (block.isVisible()) {
            // Draw Tabs
            if (m_showTabs) {
                paintTabs(painter, block, blockRect.top());
            }

            // Draw Spaces
            if (m_showSpaces) {
                paintSpaces(painter, block, blockRect.top());
            }

            // Draw EOL
            if (m_showEOL) {
                paintEOL(painter, block, blockRect.top(), blockRect.bottom());
            }

            // Draw Wrap Symbols
            if (m_showWrapSymbol) {
                paintWrapSymbols(painter, block, blockRect.top(), blockRect.bottom());
            }
        }

        block = block.next();
    }
}

void CodeEditor::zoomIn() {
    QFont currentFont = this->font();
    int currentSize = currentFont.pointSize();
    if (currentSize < 72) {
        currentFont.setPointSize(currentSize + 1);
        this->setFont(currentFont);
    }
}

void CodeEditor::zoomOut() {
    QFont currentFont = this->font();
    int currentSize = currentFont.pointSize();
    if (currentSize > 8) {
        currentFont.setPointSize(currentSize - 1);
        this->setFont(currentFont);
    }
}

void CodeEditor::defaultZoom() {
    QFont currentFont = this->font();
    currentFont.setPointSize(12);
    this->setFont(currentFont);
}


