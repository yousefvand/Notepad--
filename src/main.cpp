#include "mainwindow.h"
#include <QApplication>

#define COLOR_RESET       "\033[0m"
#define COLOR_RED         "\033[31m"  // For errors or warnings
#define COLOR_GREEN       "\033[32m"  // For success messages
#define COLOR_YELLOW      "\033[33m"  // For warnings
#define COLOR_BLUE        "\033[34m"  // For informational messages
#define COLOR_MAGENTA     "\033[35m"
#define COLOR_CYAN        "\033[36m"

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    int line = context.line;

    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s[DEBUG]%s %s (%s:%d, %s)\n",
                COLOR_BLUE, COLOR_RESET, localMsg.constData(), file, line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "%s[INFO]%s %s (%s:%d, %s)\n",
                COLOR_CYAN, COLOR_RESET, localMsg.constData(), file, line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s[WARNING]%s %s (%s:%d, %s)\n",
                COLOR_YELLOW, COLOR_RESET, localMsg.constData(), file, line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s[CRITICAL]%s %s (%s:%d, %s)\n",
                COLOR_RED, COLOR_RESET, localMsg.constData(), file, line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s[FATAL]%s %s (%s:%d, %s)\n",
                COLOR_MAGENTA, COLOR_RESET, localMsg.constData(), file, line, function);
        abort();
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<QStringList>("QStringList");

    MainWindow w;
    w.show();

    qInstallMessageHandler(customMessageHandler);
    return app.exec();
}


