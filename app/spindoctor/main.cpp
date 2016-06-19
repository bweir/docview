#include <QCoreApplication>
#include <QCommandLineParser>
#include <QObject>
#include <QDebug>
#include <QRegularExpression>
#include <QFileInfo>

#ifndef VERSION
#define VERSION "0.0.0"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Parallax Inc.");
    QCoreApplication::setOrganizationDomain("www.parallax.com");
    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setApplicationName(QObject::tr("spindoctor"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(
            QObject::tr("\nMake beautiful documentation from Spin objects"
                "\nCopyright 2016 by %1").arg(QCoreApplication::organizationName()));

    parser.addPositionalArgument("file",  QObject::tr("Spin file to show"), "FILE");

    parser.process(app);

    if (parser.positionalArguments().size() < 1)
    {
        qDebug() << "Error: No object passed";
        return 1;
    }

    return 0;
}
