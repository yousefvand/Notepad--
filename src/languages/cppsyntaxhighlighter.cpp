#include "cppsyntaxhighlighter.h"

CppSyntaxHighlighter::CppSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {

    // Define keyword formats
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    typeFormat.setForeground(Qt::darkMagenta);
    typeFormat.setFontWeight(QFont::Bold);

    preprocessorFormat.setForeground(Qt::darkYellow);
    preprocessorFormat.setFontWeight(QFont::Bold);

    // List of C++ keywords
    QStringList keywordPatterns = {
        "\\balignas\\b", "\\balignof\\b", "\\band\\b", "\\band_eq\\b", "\\basm\\b", "\\batomic_cancel\\b",
        "\\batomic_commit\\b", "\\batomic_noexcept\\b", "\\bauto\\b", "\\bbreak\\b", "\\bcase\\b", "\\bcatch\\b",
        "\\bchar\\b", "\\bchar8_t\\b", "\\bchar16_t\\b", "\\bchar32_t\\b", "\\bclass\\b", "\\bcompl\\b",
        "\\bconcept\\b", "\\bconst\\b", "\\bconsteval\\b", "\\bconstexpr\\b", "\\bconstinit\\b", "\\bconst_cast\\b",
        "\\bcontinue\\b", "\\bco_await\\b", "\\bco_return\\b", "\\bco_yield\\b", "\\bdecltype\\b", "\\bdefault\\b",
        "\\bdelete\\b", "\\bdo\\b", "\\bdouble\\b", "\\bdynamic_cast\\b", "\\belse\\b", "\\benum\\b", "\\bexplicit\\b",
        "\\bexport\\b", "\\bextern\\b", "\\bfalse\\b", "\\bfloat\\b", "\\bfor\\b", "\\bfriend\\b", "\\bgoto\\b",
        "\\bif\\b", "\\binline\\b", "\\bint\\b", "\\blong\\b", "\\bmutable\\b", "\\bnamespace\\b", "\\bnew\\b",
        "\\bnoexcept\\b", "\\bnot\\b", "\\bnot_eq\\b", "\\bnullptr\\b", "\\boperator\\b", "\\bor\\b", "\\bor_eq\\b",
        "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", "\\bregister\\b", "\\breinterpret_cast\\b", "\\brequires\\b",
        "\\breturn\\b", "\\bshort\\b", "\\bsigned\\b", "\\bsizeof\\b", "\\bstatic\\b", "\\bstatic_assert\\b",
        "\\bstatic_cast\\b", "\\bstruct\\b", "\\bswitch\\b", "\\bsynchronized\\b", "\\btemplate\\b", "\\bthis\\b",
        "\\bthread_local\\b", "\\bthrow\\b", "\\btrue\\b", "\\btry\\b", "\\btypedef\\b", "\\btypeid\\b",
        "\\btypename\\b", "\\bunion\\b", "\\bunsigned\\b", "\\busing\\b", "\\bvirtual\\b", "\\bvoid\\b", "\\bvolatile\\b",
        "\\bwchar_t\\b", "\\bwhile\\b", "\\bxor\\b", "\\bxor_eq\\b"
    };

    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // List of C++ types
    QStringList typePatterns = {
        "\\bbool\\b", "\\bchar\\b", "\\bchar16_t\\b", "\\bchar32_t\\b", "\\bdouble\\b", "\\bfloat\\b",
        "\\bint\\b", "\\blong\\b", "\\bshort\\b", "\\bsigned\\b", "\\bunsigned\\b", "\\bvoid\\b", "\\bwchar_t\\b"
    };

    for (const QString &pattern : typePatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        highlightingRules.append(rule);
    }

    // Preprocessor directives
    HighlightingRule preprocessorRule;
    preprocessorRule.pattern = QRegularExpression("^#\\s*[a-zA-Z_]+");
    preprocessorRule.format = preprocessorFormat;
    highlightingRules.append(preprocessorRule);

    // Single line comments
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    HighlightingRule singleLineCommentRule;
    singleLineCommentRule.pattern = QRegularExpression("//[^\n]*");
    singleLineCommentRule.format = singleLineCommentFormat;
    highlightingRules.append(singleLineCommentRule);

    // Multi-line comments
    multiLineCommentFormat.setForeground(Qt::darkGreen);
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");

    // Quotation marks
    quotationFormat.setForeground(Qt::darkRed);
    HighlightingRule quotationRule;
    quotationRule.pattern = QRegularExpression("\".*\"");
    quotationRule.format = quotationFormat;
    highlightingRules.append(quotationRule);

    // Function names
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    HighlightingRule functionRule;
    functionRule.pattern = QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\()");
    functionRule.format = functionFormat;
    highlightingRules.append(functionRule);
}

void CppSyntaxHighlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
