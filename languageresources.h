#ifndef LANGUAGERESOURCES_H
#define LANGUAGERESOURCES_H

#include <QString>
#include <QStringList>
#include <QMap>

namespace language {

struct StringData {
    QString mId;
    QString mString;
};

typedef QList<StringData> string_data_map;

struct Resources {
    QString mLanguageId;
    string_data_map mStringData;

    StringData &getStringDataById(const QString &id) {
        for (StringData &s : mStringData) {
            if (s.mId == id) {
                return s;
            }
        }
        mStringData.push_back({
                                  id,
                                  QString()
                              });
        return mStringData.back();
    }
};

typedef QList<Resources> language_map;
class LanguageResources
{
private:
    language_map mLanguageMap;
    QStringList mStringIds;
    QMap<QString, QString> mLanguageIds;
public:
    LanguageResources();

    Resources &getResourceByLanguageId(const QString &str) {
        for (Resources &r : mLanguageMap) {
            if (str == r.mLanguageId) {
                return r;
            }
        }

        mLanguageMap.push_back({
                                   str,
                                   string_data_map()
                               });
        return mLanguageMap.back();
    }

    language_map &getLanguageMap() {return mLanguageMap;}
    QStringList &getStringIds() {return mStringIds;}
    const language_map &getLanguageMap() const {return mLanguageMap;}
    const QStringList &getStringIds() const {return mStringIds;}
    const QMap<QString, QString> getLanguageIds() const {return mLanguageIds;}
};

};

#endif // LANGUAGERESOURCES_H
