#pragma once

#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QTabWidget;

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void highlightCurrentLine();
    void applyIndentation(bool useTabs, int indentationWidth);
    QTabWidget* DocumentsTab();
    void highlightAllOccurrences(const QString& keyword);
    void goToLineInText(int lineNumber);
    void gotoLineInEditor(int lineNumber);
    void setShowTabs(bool enabled);
    void setShowSpaces(bool enabled);
    void setShowEOL(bool enabled);
    void setShowAllCharacters(bool enabled);
    void setShowIndentGuide(bool enabled);
    void setShowWrapSymbol(bool enabled);
    void setTabWidth(int width);
    void zoomIn();
    void zoomOut();
    void defaultZoom();
    void setShowMathRendering(bool enabled);

protected:
    void resizeEvent(QResizeEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void textChanged(); // FIXME: Remove this line.

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
    QTabWidget* m_documentsTab;
    bool m_useTabs;
    int m_indentationWidth;
    bool m_showTabs = false;
    bool m_showSpaces = false;
    bool m_showEOL = false;
    bool m_showAllCharacters = false;
    bool m_showIndentGuide = false;
    bool m_showWrapSymbol = false;
    int m_tabWidth;
    bool m_showMathRendering = false;

    void paintTabs(QPainter& painter, const QTextBlock& block, int top);
    void paintSpaces(QPainter& painter, const QTextBlock& block, int top);
    void paintEOL(QPainter& painter, const QTextBlock& block, int top, int bottom);
    void paintIndentGuides(QPainter& painter, const QTextBlock& block, int top, int bottom);
    void paintWrapSymbols(QPainter& painter, const QTextBlock& block, int top, int bottom);
};

class LineNumberArea : public QWidget {
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

