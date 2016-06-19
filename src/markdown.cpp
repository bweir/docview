#include "markdown.h"

#include "buffer.h"
#include "document.h"
#include "html.h"


Markdown::Markdown(int toc_depth, QObject *parent)
    : QObject(parent)
{
    setTOCDepth(toc_depth);
}

Markdown::~Markdown()
{

}

bool Markdown::setTOCDepth(int toc_depth)
{
    if (toc_depth < 0 || toc_depth > 7) return false;
    _toc_depth = toc_depth;
    return true;
}

QString Markdown::render(QString in, Markdown::RenderType rendertype)
{
    hoedown_html_flags html_flags = HOEDOWN_HTML_SKIP_HTML;
    hoedown_extensions extensions = (hoedown_extensions) (HOEDOWN_EXT_TABLES | HOEDOWN_EXT_FENCED_CODE);

    hoedown_buffer * inputbuffer = hoedown_buffer_new(1024);
    hoedown_buffer * outputbuffer = hoedown_buffer_new(64);
    hoedown_buffer_puts(inputbuffer, in.toLocal8Bit().data());

    hoedown_renderer * renderer = NULL;

    switch (rendertype) {
        case RenderContent:
            renderer = hoedown_html_renderer_new(html_flags, _toc_depth);
            break;
        case RenderTOC:
            renderer = hoedown_html_toc_renderer_new(_toc_depth);
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

