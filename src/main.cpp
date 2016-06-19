#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QMap>
#include <QStandardPaths>
#include <QDir>
#include <QString>

#include <QApplication>
#include <QWebView>
#include <QTemporaryFile>

#include "spindoctor.h"
#include "markdown.h"

QString wrapPanel(QString text)
{
    return "<div class=\"panel panel-default\"><div class=\"panel-body\">" +
            text +
           "</div></div>";
}

QString wrapTitle(QString title, QString description)
{
    QFileInfo fi(title);
    if (!fi.suffix().isEmpty())
        title = fi.completeBaseName();

    return QString("<h1>%1 <small>%2</small></h1>").arg(title).arg(description);
}

QString getTemplate(QString filename)
{
    QFile file(QString(":/templates/%1.html").arg(filename));
    file.open(QFile::ReadOnly);
    QString text = file.readAll();
    file.close();

    return text;
}


QString openFile(QString filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadOnly))
    {
        return QString();
    }

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    in.setCodec("UTF-8");
    return in.readAll();
}

QString parsePubBlock(QString s)
{
    QString output;

    s.replace(QRegularExpression("(:|\\||\n)"),"\n");
    s.replace(QRegularExpression("^pub[ \t]*",QRegularExpression::CaseInsensitiveOption),"");
    
    QString func = s.trimmed();
    
    s.replace(QRegularExpression("\\(.*?\\)[ \t]*?"),"\n");
    
    output += "* * *\n\n";
    output += "### " + s + "\n\n";
    
    output += "    " + func + "\n\n";

    return output;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("spindoctor");

    if (argc < 2)
        return 1;

    QString filename = argv[1];
    QString text = openFile(filename);

    SpinDoctor spind;

    QStringList textblocks = spind.filterText(text);

    bool functions = false;

    QMap<QString, QString> specialkeys;

    QString output = "";

    foreach(QString sblock, textblocks)
    {
        sblock = sblock.remove("{{")
                .replace("}}","\n")
                .remove("''");

        sblock = spind.parseIndent(sblock);

        foreach(QString s, sblock.split("\n"))
        {
            if (s.startsWith("pub ", Qt::CaseInsensitive))
            {
                if (!functions)
                {
                    output += "\n## Functions\n\n";
                    functions = true;
                }

                output += parsePubBlock(s);
            }
            else if (s.startsWith("@"))
            {
                QStringList keys;
                keys << "title" << "description" << "version";

                foreach (QString k, keys)
                {
                    if (s.startsWith("@"+k, Qt::CaseInsensitive))
                    {
                        s = s.right(s.size() - QString("@"+k).size());
                        specialkeys[k] = s.trimmed();
                    }
                }
            }
            else
            {
                    output += s + "\n";
            }
        }
    }

   
    QString title, version, description;

    title = filename;
    if (specialkeys.contains("title"))
        title = specialkeys["title"];

    if (specialkeys.contains("description"))
        description = specialkeys["description"];

    if (specialkeys.contains("version"))
        version = specialkeys["version"];

    title = wrapTitle(title, description);

    Markdown md(3);

    QString content = md.render(output);
    QString toc = md.render(output, Markdown::RenderTOC);

    QString footer = QString("%1 v%2").arg(filename).arg(version);

    QString templatetext = getTemplate("bootstrap");

    output = QString(templatetext).arg(title).arg(toc).arg(content).arg(footer);

    QFileInfo fi(filename);

    QString tmpdir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(tmpdir);

    QTemporaryFile file;
    file.setFileTemplate(tmpdir+"/"+fi.fileName()+".XXXXXX.html");
    if (file.open())
    {
        qDebug() << "rendered file to" << file.fileName();
        file.write(qPrintable(output));
        file.close();
    }

    QWebView web;
    web.load(QUrl("file://"+file.fileName()));
    web.show();

    return app.exec();
}
