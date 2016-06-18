#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <QMap>
#include <QStandardPaths>
#include <QDir>

#include <QApplication>
#include <QWebView>
#include <QTemporaryFile>

#include "spindoctor.h"

#include "buffer.h"
#include "document.h"
#include "html.h"

#include <unistd.h>

QString lstrip(QString s)
{
    for (int n = s.size(); n > 0; n--) {
        if (!s.at(s.size()-n).isSpace()) {
            return s.right(n);
        }
    }
    return "";
}

int getindent(QString s)
{
    return s.size() - lstrip(s).size();
}

enum RenderType
{
    RenderContent,
    RenderTOC
};


enum Block {
    NoBlock,
    PubBlock,
    PriBlock,
    DatBlock,
    VarBlock,
    ConBlock,
    ObjBlock
};


QString getExtension(QString filename)
{
    QFileInfo fi(filename);
    return fi.suffix().toLower();
}

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

QString convertMarkdown(QString in, RenderType rendertype, int toc_level = 0)
{
    hoedown_html_flags html_flags = HOEDOWN_HTML_SKIP_HTML;
    hoedown_extensions extensions = (hoedown_extensions) (HOEDOWN_EXT_TABLES | HOEDOWN_EXT_FENCED_CODE);

    hoedown_buffer * inputbuffer = hoedown_buffer_new(1024);
    hoedown_buffer * outputbuffer = hoedown_buffer_new(64);
    hoedown_buffer_puts(inputbuffer, in.toLocal8Bit().data());

    hoedown_renderer * renderer = NULL;

    switch (rendertype) {
        case RenderContent:
            renderer = hoedown_html_renderer_new(html_flags, toc_level);
            break;
        case RenderTOC:
            renderer = hoedown_html_toc_renderer_new(toc_level);
            break;
    };

    hoedown_document * document = hoedown_document_new(renderer, extensions, 16);

    hoedown_document_render(document, outputbuffer, inputbuffer->data, inputbuffer->size);

    hoedown_buffer_free(inputbuffer);
    hoedown_document_free(document);
    hoedown_html_renderer_free(renderer);

    QString out = QString(QByteArray((char *)outputbuffer->data, (int) outputbuffer->size));

    hoedown_buffer_free(outputbuffer);

    return out;
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


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("spindoctor");

    if (argc < 2)
        return 1;

    QString filename = argv[1];
    QString text = openFile(filename);

    SpinDoctor w;

    QStringList b;
    b << "{{" << "}}"
        << "''" << "\n"
        << "^pub" << "(:|\\||\n)";

    w.addRules(b);
    w.rebuildRules();
    text = w.process(text)
        .remove("{{")
        .replace("}}","\n")
        .remove("''");

    QStringList strings = text.split("\n");

    int indent = 0;
    int indentlevel = 0;
    bool functions = false;
    bool constants = false;

   

    QMap<QString, QString> specialkeys;

    QString output = "";

    Block block = NoBlock;

    foreach(QString s, strings)
    {
        indent = getindent(s);

        if (indent > 0)
        {
            if (indentlevel == 0 || indent < indentlevel)
            {
                indentlevel = indent;
            }
        }
        else
        {
            if (!s.isEmpty())
                indentlevel = 0;
        }

        if (s.startsWith("pub ", Qt::CaseInsensitive))
        {
            if (!functions)
            {
                output += "\n## Functions\n\n";
                functions = true;
            }

            s.replace(QRegularExpression("(:|\\||\n)"),"\n");
            s.replace(QRegularExpression("^pub[ \t]*",QRegularExpression::CaseInsensitiveOption),"");

            QString func = s.trimmed();

            s.replace(QRegularExpression("\\(.*?\\)[ \t]*?"),"\n");

            output += "\n* * *\n\n";
            output += "### " + s;

            output += "    " + func + "\n\n";
        }
        else
        {
            s = s.right(s.size() - indentlevel) + "\n";

            QStringList keys;
            keys << "title" << "description" << "version";

            if (s.startsWith("@"))
            {
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
                output += s;
            }
        }
    }

    qDebug() << output;


    QString title = filename;
    if (specialkeys.contains("title"))
        title = specialkeys["title"];

    QString description;
    if (specialkeys.contains("description"))
        description = specialkeys["description"];

    if (specialkeys.contains("version"))
        title += " v"+specialkeys["version"];

    title = wrapTitle(title, description);

    QString content = convertMarkdown(output, RenderContent, 3);
    QString toc = convertMarkdown(output, RenderTOC, 3);

    QString templatetext = getTemplate("bootstrap");

    output = QString(templatetext).arg(title).arg(toc).arg(content).arg("");

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
