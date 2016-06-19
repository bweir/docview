#include "spindoctor.h"

#include <QDebug>

SpinDoctor::SpinDoctor(QObject *parent)
    : QObject(parent)
{
    QStringList b;
    b << "{{" << "}}"
        << "''" << "\n"
        << "^pub\\b" << "(:|\\||\n)";

    addRules(b);
    rebuildRules();
}

void SpinDoctor::addRules(QStringList rules)
{
    TwoPartRule rule;

    if (rules.size() % 2 != 0) return;

    for (int i = 0; i < rules.size(); i += 2)
    {
        rule.start = QRegularExpression(rules[i],
                     QRegularExpression::CaseInsensitiveOption);

        rule.end   = QRegularExpression(rules[i+1],
                     QRegularExpression::CaseInsensitiveOption);

        twopartrules.append(rule);
    }
}

QStringList SpinDoctor::filterText(QString text)
{
    QStringList output;
    int state = 0, laststate = 0;

    int offset = 0, end = 0;
    TwoPartRule rule;
    QRegularExpressionMatch matchstart;

    if (laststate < 1)
    {
        matchstart = re_tokens.match(text);
        offset = matchstart.capturedStart();

        for (state = 0; state < twopartrules.size(); state++)
        {
            rule = twopartrules[state];
            if (rule.start.pattern() == matchstart.captured())
            {
                break;
            }
        }
        if (state >= twopartrules.size()) state = -1;
    }
    else
    {
        state = laststate-1;
        rule = twopartrules[state];
    }

    while (offset >= 0)
    {
        QRegularExpressionMatch matchend = rule.end.match(text, offset + matchstart.capturedLength());
        end = matchend.capturedStart();

        int length;
        if (end == -1)
        {
            laststate = state+1;
            length = text.length() - offset;
        }
        else
        {
            length = end - offset + matchend.capturedLength();
        }

        output.append(text.mid(offset, length));

        matchstart = re_tokens.match(text, offset + length);
        offset = matchstart.capturedStart();

        for (state = 0; state < twopartrules.size(); state++)
        {
            rule = twopartrules[state];
            if (rule.start.pattern() == matchstart.captured())
                break;
        }
        if (state >= twopartrules.size()) state = -1;
    }

    return output;
}

void SpinDoctor::rebuildRules()
{
    QStringList tokens;
    QString tokenstring;
    foreach (TwoPartRule r, twopartrules)
    {
        tokens.append(r.start.pattern());
    }
    tokenstring = tokens.join("|");
    tokenstring = "("+tokenstring+")";
    re_tokens = QRegularExpression(tokenstring, 
            QRegularExpression::MultilineOption |
            QRegularExpression::CaseInsensitiveOption);

    qDebug() << re_tokens;
}


QString SpinDoctor::lstrip(QString s)
{
    for (int n = s.size(); n > 0; n--) {
        if (!s.at(s.size()-n).isSpace()) {
            return s.right(n);
        }
    }
    return "";
}

int SpinDoctor::getIndent(QString s)
{
    return s.size() - lstrip(s).size();
}


QString SpinDoctor::parseIndent(QString text)
{
    int indent = 0;
    int indentlevel = 0;
    QStringList lines;

    foreach(QString s, text.split("\n"))
    {
        indent = getIndent(s);

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

        lines.append(s.right(s.size() - indentlevel));
    }

    return lines.join("\n");
}


