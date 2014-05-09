#ifndef LANGUAGERESOURCES_H
#define LANGUAGERESOURCES_H

#include <QString>
#include <QStringList>
#include <QMap>

namespace language {
typedef QMap<QString, QString> string_data_map;
struct Resources {
    QString mLanguageId;
    string_data_map mStringData;
};

typedef QMap<QString, Resources> language_map;
class LanguageResources
{
private:
    language_map mLanguageMap;
    QStringList mStringIds;
    QMap<QString, QString> mLanguageIds;
public:
    LanguageResources();

    language_map &getLanguageMap() {return mLanguageMap;}
    QStringList &getStringIds() {return mStringIds;}
    const language_map &getLanguageMap() const {return mLanguageMap;}
    const QStringList &getStringIds() const {return mStringIds;}
    const QMap<QString, QString> getLanguageIds() const {return mLanguageIds;}
};

};

#endif // LANGUAGERESOURCES_H
