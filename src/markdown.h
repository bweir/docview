#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>

class Markdown
: public QObject
{
    Q_OBJECT

    int _toc_depth;

public:

    enum RenderType
    {
        RenderContent,
        RenderTOC
    };

    explicit Markdown(int toc_depth = 3, QObject * parent = 0);
    ~Markdown();
    QString render(QString text, RenderType rendertype = RenderContent);

    bool setTOCDepth(int toc_depth = 3);

};
