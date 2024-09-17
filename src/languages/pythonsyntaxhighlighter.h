#pragma once

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QStringList>

class PythonSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit PythonSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;

    QStringList keywords;
    QStringList builtins;
    QStringList operators;
    QStringList braces;

    void highlightMultiLineString(const QString &text, const QString &delimiter, int startIndex);
};

