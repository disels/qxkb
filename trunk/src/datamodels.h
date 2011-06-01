////////////////////////////////////////
//  File      : datamodels.h          //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#ifndef DATAMODELS_H
#define DATAMODELS_H

#include "defs.h"
#include "x11tools.h"


enum {
    LAYOUT_COLUMN_FLAG = 0,
    LAYOUT_COLUMN_NAME = 1,
    LAYOUT_COLUMN_MAP = 2,
    LAYOUT_COLUMN_VARIANT = 3,
    LAYOUT_COLUMN_DISPLAY_NAME = 4,
    SRC_LAYOUT_COLUMN_COUNT = 3,
    DST_LAYOUT_COLUMN_COUNT = 5
};

static bool localeAwareLessThan(const QString &s1, const QString &s2)
{
    return QString::localeAwareCompare(s1, s2) < 0;
}

static QList<QString> getKeysSortedByVaue(const QHash<QString, QString>& map)
{
    QList<QString> outList;

    QMap<QString, QString> reverseMap;
    // we have to add nums as translations can be dups and them reverse map will miss items
    int i=0;
    QString fmt("%1%2");
    foreach (const QString& str, map.keys())
        reverseMap.insert(fmt.arg(map[str], QString::number(i++)), str);

    QList<QString> values = reverseMap.keys();
    qSort(values.begin(), values.end(), localeAwareLessThan);

    foreach (const QString& value, values)
        outList << reverseMap[value];

    return outList;
}


class SrcLayoutModel: public QAbstractTableModel {

Q_OBJECT

public:
    SrcLayoutModel(RulesInfo* rules,QString icoPath, QObject *parent) : QAbstractTableModel(parent)
    {
        setRules(rules);
        iconDir=icoPath;
    }
//    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const { return false; }
    int columnCount(const QModelIndex& parent) const
        {
            return !parent.isValid() ? SRC_LAYOUT_COLUMN_COUNT : 0;
        }
    int rowCount(const QModelIndex&) const
        {
            return m_rules->layouts.keys().count();
        }

    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setRules(RulesInfo* rules)
    {
        m_rules = rules;
        m_layoutKeys = getKeysSortedByVaue( m_rules->layouts);
    }
    QString getLayoutAt(int row)
    {
            return m_layoutKeys[row];
    }
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;

private:
    RulesInfo* m_rules;
    QStringList m_layoutKeys;
    QString iconDir;


signals:
    void layoutRemoved();
};

class DstLayoutModel: public QAbstractTableModel
{

Q_OBJECT

public:
    DstLayoutModel(RulesInfo* rules,XKBConf * qxkb,QString icoPath,QObject *parent): QAbstractTableModel(parent)
    {
      setRules(rules);
      setConf(qxkb);
      iconDir=icoPath;
    }
    int columnCount(const QModelIndex& parent) const  { return !parent.isValid() ? DST_LAYOUT_COLUMN_COUNT : 0; }
    int rowCount(const QModelIndex&) const { return  m_qxkb->layouts.size();}
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setRules(RulesInfo* rules) { m_rules = rules; }
    void setConf(XKBConf * qxkb) { m_qxkb= qxkb; }
    void reset() { QAbstractTableModel::reset(); }
    void emitDataChange(int row, int col) { emit dataChanged(createIndex(row,col),createIndex(row,col)); }
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    RulesInfo* m_rules;
    XKBConf *m_qxkb;
    QString iconDir;

signals:
    void layoutAdded();
};

class XkbOptionsModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    XkbOptionsModel(RulesInfo* rules, XKBConf* xkbConfig, QObject *parent) : QAbstractItemModel(parent)
        { setRules(rules,xkbConfig); }

    int columnCount(const QModelIndex& /*parent*/) const { return 1; }
    int rowCount(const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& index) const;
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    QVariant data(const QModelIndex& index, int role) const;

    void setRules(RulesInfo* rules,XKBConf* xkbConfig) { m_rules = rules; m_xkbConfig = xkbConfig;}
    void reset() { QAbstractItemModel::reset(); }
    void gotoGroup(const QString& group, QTreeView* view);

private:
    RulesInfo* m_rules;
    XKBConf* m_xkbConfig;
};




#endif // DATAMODELS_H
