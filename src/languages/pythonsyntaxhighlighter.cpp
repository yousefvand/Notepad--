#include "pythonsyntaxhighlighter.h"

PythonSyntaxHighlighter::PythonSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {

    // Define the formats
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);

    singleLineCommentFormat.setForeground(Qt::darkGreen);

    quotationFormat.setForeground(Qt::darkRed);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);

    // Define the keyword lists
    keywords = QStringList() << "and" << "as" << "assert" << "break" << "class" << "continue"
                             << "def" << "del" << "elif" << "else" << "except" << "False"
                             << "finally" << "for" << "from" << "global" << "if" << "import"
                             << "in" << "is" << "lambda" << "None" << "nonlocal" << "not"
                             << "or" << "pass" << "raise" << "return" << "True" << "try"
                             << "while" << "with" << "yield";

    builtins = QStringList() << "abs" << "dict" << "help" << "min" << "setattr"
                             << "all" << "dir" << "hex" << "next" << "slice"
                             << "any" << "divmod" << "id" << "object" << "sorted"
                             << "ascii" << "enumerate" << "input" << "oct" << "staticmethod"
                             << "bin" << "eval" << "int" << "open" << "str"
                             << "bool" << "exec" << "isinstance" << "ord" << "sum"
                             << "bytearray" << "filter" << "issubclass" << "pow" << "super"
                             << "bytes" << "float" << "iter" << "print" << "tuple"
                             << "callable" << "format" << "len" << "property" << "type"
                             << "chr" << "frozenset" << "list" << "range" << "vars"
                             << "classmethod" << "getattr" << "locals" << "repr" << "zip"
                             << "compile" << "globals" << "map" << "reversed" << "__import__"
                             << "complex" << "hasattr" << "max" << "round" << "delattr"
                             << "hash" << "memoryview" << "set";

    operators = QStringList() << "=" << "==" << "!=" << "<" << "<=" << ">" << ">=" << "+" << "-"
                              << "*" << "/" << "//" << "%" << "**" << "<<" << ">>" << "&"
                              << "|" << "^" << "~";

    braces = QStringList() << "{" << "}" << "(" << ")" << "[" << "]";
}

void PythonSyntaxHighlighter::highlightBlock(const QString &text) {
    // Highlight keywords
    for (const QString &keyword : keywords) {
        int index = text.indexOf(keyword);
        while (index >= 0) {
            int length = keyword.length();
            setFormat(index, length, keywordFormat);
            index = text.indexOf(keyword, index + length);
        }
    }

    // Highlight built-ins
    for (const QString &builtin : builtins) {
        int index = text.indexOf(builtin);
        while (index >= 0) {
            int length = builtin.length();
            setFormat(index, length, functionFormat);
            index = text.indexOf(builtin, index + length);
        }
    }

    // Highlight comments
    int commentIndex = text.indexOf("#");
    if (commentIndex >= 0) {
        setFormat(commentIndex, text.length() - commentIndex, singleLineCommentFormat);
    }

    // Highlight strings
    int startIndex = 0;
    while (startIndex >= 0) {
        int singleQuoteIndex = text.indexOf("'", startIndex);
        int doubleQuoteIndex = text.indexOf("\"", startIndex);
        if (singleQuoteIndex >= 0 && (singleQuoteIndex < doubleQuoteIndex || doubleQuoteIndex < 0)) {
            highlightMultiLineString(text, "'", singleQuoteIndex);
            startIndex = singleQuoteIndex + 1;
        } else if (doubleQuoteIndex >= 0) {
            highlightMultiLineString(text, "\"", doubleQuoteIndex);
            startIndex = doubleQuoteIndex + 1;
        } else {
            break;
        }
    }

    // Highlight operators and braces
    for (const QString &op : operators) {
        int index = text.indexOf(op);
        while (index >= 0) {
            setFormat(index, op.length(), keywordFormat);
            index = text.indexOf(op, index + op.length());
        }
    }

    for (const QString &brace : braces) {
        int index = text.indexOf(brace);
        while (index >= 0) {
            setFormat(index, 1, keywordFormat);
            index = text.indexOf(brace, index + 1);
        }
    }
}

void PythonSyntaxHighlighter::highlightMultiLineString(const QString &text, const QString &delimiter, int startIndex) {
    int endIndex = text.indexOf(delimiter, startIndex + 1);
    int length;
    if (endIndex == -1) {
        setCurrentBlockState(1);
        length = text.length() - startIndex;
    } else {
        length = endIndex - startIndex + 1;
    }
    setFormat(startIndex, length, quotationFormat);
}
