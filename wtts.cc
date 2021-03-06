#include "wtts.hh"
#include "wstring_qt.hh"
#include "utf8.hh"
#include "langcoder.hh"
#include "language.hh"
#include "htmlescape.hh"

#include <QUrl>
#include <QTextCodec>
#include <QRegExp>
namespace WebTTS
{

    WebTssMaker::WebTssMaker()
    {}
    WebTssMaker::WebTssMaker(Config::WebTtss const & tss, qint32 langCode):flag("")
    {
        lang = LangCoder::intToCode2(langCode);
        if(lang.size())
        {
            flag = QString("<img src=\"qrcx://localhost/flags/%1.png\" class=\"tssflag\">").arg(Language::countryCodeForId(langCode));
        }
        else
        {
            flag = QString("<img src=\"qrcx://localhost/icons/internet.png\" class=\"tssflag\">");
            lang.append("other");
        }
        QRegExp regex = QRegExp( QString(",? ?%1 ?,?").arg(lang), Qt::CaseInsensitive );
        for(unsigned i =0; i < tss.size(); i++ )
        {
            if(tss[i].enabled && tss[i].url.size() && (!tss[i].langlist.size() || tss[i].langlist.contains(regex)))
            {
                wts.push_back(tss[i]);
            }
        }

    }
    std::string WebTssMaker::MakeTssView(QString word)
    {
        if(!word.size() || !wts.size()) return std::string();
        //QString lang = LangCoder::intToCode2(langcode);
        std::string result("<span class=\"webtssview\">");

       result += flag.toUtf8().data();

       for( Config::WebTtss::const_iterator i = wts.begin(); i != wts.end(); ++i )
       {
           QUrl tmpUrl(i->url);
           tmpUrl.addQueryItem("webtts","");
           QByteArray url = tmpUrl.toEncoded();
           QString ttsWord = word.size() > (int)i->maxlength? word.left( i->maxlength):word;
           url.replace( "%25GDWORD%25", ttsWord.toUtf8().toPercentEncoding() )
                   .replace("%25GDLANG%25",lang.toUtf8().toPercentEncoding());

           result += std::string("<label>")
                   + Html::escape(i->name.toUtf8().data())
                   +"</label><a href=\""
                   +url.data()
                   +"\"><img src=\"qrcx://localhost/icons/tssspeacker.png\" alt=\"Play\" class=\"tssplay\"></a>";
       }

       result +="</span>";
        return result;

    }
    QByteArray WebTssMaker::getTTsEncodedUrl(unsigned ttsIndex,QString word)
    {
        if(ttsIndex>=wts.size()) return QByteArray();
        QUrl tmpUrl(wts[ttsIndex].url);
        tmpUrl.addQueryItem("webtts","");
        QByteArray url = tmpUrl.toEncoded();
        QString ttsWord = word.size() > (int)wts[ttsIndex].maxlength? word.left( wts[ttsIndex].maxlength):word;
        url.replace( "%25GDWORD%25", ttsWord.toUtf8().toPercentEncoding() )
                .replace("%25GDLANG%25",lang.toUtf8().toPercentEncoding());
        return url.append("&webtts");
    }

    QByteArray WebTssMaker::getTTsEncodedUrl(QString ttsName,QString word)
    {
        if(ttsName.isEmpty()) return QByteArray();
        for(unsigned ttsIndex = 0; ttsIndex < wts.size();ttsIndex++)
        {
            if(wts[ttsIndex].name==ttsName)
            {
                QUrl tmpUrl(wts[ttsIndex].url);
                tmpUrl.addQueryItem("webtts","");
                QByteArray url = tmpUrl.toEncoded();
                QString ttsWord = word.size() >(int)wts[ttsIndex].maxlength? word.left(wts[ttsIndex].maxlength):word;
                url.replace( "%25GDWORD%25", ttsWord.toUtf8().toPercentEncoding() )
                        .replace("%25GDLANG%25",lang.toUtf8().toPercentEncoding());
                return url.append("&webtts");
            }
        }
        return QByteArray();
    }

}


