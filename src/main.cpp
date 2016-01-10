#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "spindoctor.h"

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


//    // Functions for html.h, 
//
//    hoedown_renderer * hoedown_html_renderer_new(hoedown_html_flags render_flags, int nesting_level); 
//    hoedown_renderer * hoedown_html_toc_renderer_new(int nesting_level);
//
//    void hoedown_html_renderer_free(hoedown_renderer *renderer);
//
//
//    // Functions for document.h, the markdown renderer
//
//    hoedown_document * hoedown_document_new(const hoedown_renderer *renderer, hoedown_extensions extensions, size_t max_nesting); 
//
//    void hoedown_document_render(hoedown_document *doc, hoedown_buffer *ob, const uint8_t *data, size_t size);
//    void hoedown_document_free(hoedown_document *doc);




/*


int main(int argc, char **argv)
{
    struct option_data data;
    FILE *file = stdin;
    hoedown_buffer *ib, *ob;
    hoedown_renderer *renderer = NULL;
    void (*renderer_free) (hoedown_renderer *) = NULL;
    hoedown_document *document;

    data.basename = argv[0];
    data.done = 0;
    data.show_time = 0;
    data.iunit = DEF_IUNIT;
    data.ounit = DEF_OUNIT;
    data.filename = NULL;
    data.renderer = RENDERER_HTML;
    data.toc_level = 0;
    data.html_flags = 0;
    data.extensions = 0;
    data.max_nesting = DEF_MAX_NESTING;

    argc = parse_options(argc, argv, parse_short_option, parse_long_option, parse_argument, &data);
    if (data.done) return 0;
    if (!argc) return 1;

    if (data.filename) {
        file = fopen(data.filename, "r");
        if (!file) {
            fprintf(stderr, "Unable to open input file \"%s\": %s\n", data.filename, strerror(errno));
            return 5;
        }
    }

    ib = hoedown_buffer_new(data.iunit);

    if (hoedown_buffer_putf(ib, file)) {
        fprintf(stderr, "I/O errors found while reading input.\n");
        return 5;
    }

    if (file != stdin) fclose(file);

    switch (data.renderer) {
        case RENDERER_HTML:
            renderer = hoedown_html_renderer_new(data.html_flags, data.toc_level);
            renderer_free = hoedown_html_renderer_free;
            break;
        case RENDERER_HTML_TOC:
            renderer = hoedown_html_toc_renderer_new(data.toc_level);
            renderer_free = hoedown_html_renderer_free;
            break;
    };

    ob = hoedown_buffer_new(data.ounit);
    document = hoedown_document_new(renderer, data.extensions, data.max_nesting);

    t1 = clock();
    hoedown_document_render(document, ob, ib->data, ib->size);
    t2 = clock();

    hoedown_buffer_free(ib);
    hoedown_document_free(document);
    renderer_free(renderer);

    (void)fwrite(ob->data, 1, ob->size, stdout);
    hoedown_buffer_free(ob);

    return 0;
}
*/




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
    QCoreApplication a(argc, argv);

    if (argc < 2)
        return 1;

    QString text = openFile(argv[1]);


    //    qDebug() << "THE TEXT: " << text;

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

    QString output = "";

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
            indentlevel = 0;
        }

        s = s.right(s.size() - indentlevel) + "\n";

        if (s.startsWith("pub ", Qt::CaseInsensitive))
        {
            if (!functions)
            {
                output += "\n## Functions\n\n";
                functions = true;
            }

            s.replace(QRegularExpression("^pub[ \t]*",QRegularExpression::CaseInsensitiveOption),"### ")
             .replace(QRegularExpression("(:|\\||\n)"),"\n")
             .replace(QRegularExpression("\\(.*?\\)[ \t]*?"),"\n");


        }

        output += s;
    }


    qDebug() << output;

    return 0;
}
