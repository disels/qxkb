////////////////////////////////////////
//  File      : datamodels.cpp        //
//  Written by: disels@gmail.com      //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "datamodels.h"


QVariant SrcLayoutModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    QString colNames[] = {"", tr("Layout Name"), tr("Map")};
    if (orientation == Qt::Horizontal) {
        return colNames[section];
    }
    return QVariant();
}

QVariant SrcLayoutModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    return QVariant();

    int col = index.column();
    int row = index.row();
    QHash<QString, QString> layouts = m_rules->layouts;
    QString layout = m_layoutKeys[row];

    QSvgRenderer *flagSVG ;
    QIcon icons;

    QString PNG = iconDir+"/"+layout+".png";
    QString SVG = iconDir+"/"+layout+".svg";
    bool isPNG = QFile::exists(PNG);
    bool isSVG = QFile::exists(SVG);

    if (isSVG)
    {
      flagSVG = new QSvgRenderer(SVG,NULL);
      isSVG = isSVG && flagSVG->isValid();
    }
    if (isSVG)
    {
        QPixmap pix(48,22);
        QPainter *painter =new QPainter();
        painter->begin(&pix);
        flagSVG->render(painter,QRectF(0,0,48,22));
        painter->end();
        icons = (QIcon(pix));
    }
    else if (isPNG)
          icons = QIcon(PNG);

    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::DecorationRole)
    {
        switch(col)
        {
          case LAYOUT_COLUMN_FLAG: return QIcon::fromTheme(layout, icons).pixmap(QSize(48,32));
        }
    } else if (role == Qt::DisplayRole) {
        switch(col)
            {
                case LAYOUT_COLUMN_NAME: return layouts.value(layout);
                case LAYOUT_COLUMN_MAP: return layout;
                break;
                default: ;
            }
    }
    return QVariant();
}

Qt::ItemFlags SrcLayoutModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
}

QMimeData* SrcLayoutModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = QAbstractTableModel::mimeData(indexes);
    mimeData->setText(indexes.first().data().toString());
    return mimeData;
}

bool SrcLayoutModel::dropMimeData(const QMimeData *data,Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(data);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    if (action == Qt::IgnoreAction)
        return false;

    emit layoutRemoved();
    return true;
}


QVariant DstLayoutModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    QString colNames[] = {"", tr("Layout Name"), tr("Map"), tr("Variant"), tr("Label")};
    if (orientation == Qt::Horizontal) {
        return colNames[section];
    }
    return QVariant();
}

QVariant DstLayoutModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int col = index.column();
    int row = index.row();
    QHash<QString, QString> layouts = m_rules->layouts;

    LayoutUnit lu = m_antico->layouts[row];

    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::DecorationRole) {
        switch(col) {
            case LAYOUT_COLUMN_FLAG:
                return QIcon::fromTheme(lu.layout, QIcon(iconDir+"/"+lu.layout+".png")).pixmap(QSize(48,32));  //QPixmap(iconDir+"/"+lu.layout+".png");
        }
    } else if (role == Qt::DisplayRole) {
        switch(col) {
            case LAYOUT_COLUMN_NAME: return layouts[lu.layout];
            case LAYOUT_COLUMN_MAP: return lu.layout;
            case LAYOUT_COLUMN_VARIANT: return lu.variant;
            case LAYOUT_COLUMN_DISPLAY_NAME: return lu.getDisplayName();
            break;
            default: ;
        }
    }
    return QVariant();
}

bool DstLayoutModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(data);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    if (action == Qt::IgnoreAction)
        return false;

    emit layoutAdded();
    return true;
}

Qt::ItemFlags DstLayoutModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if ( m_antico->layouts.size() >= 4)
        return Qt::ItemIsDragEnabled | defaultFlags;
    return Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled | defaultFlags;
};


QVariant XkbOptionsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (role == Qt::DisplayRole) {
        if( ! index.parent().isValid() )
            return m_rules->optionGroups.values()[row].description;
        else {
            int groupRow = index.parent().row();
            QString xkbGroupNm = m_rules->optionGroups.keys()[groupRow];
            const XkbOptionGroup& xkbGroup = m_rules->optionGroups[xkbGroupNm];
            return xkbGroup.options[row].description;
        }
    }
    else if( role==Qt::CheckStateRole && index.parent().isValid() ) {
        int groupRow = index.parent().row();
        QString xkbGroupNm = m_rules->optionGroups.keys()[groupRow];
        const XkbOptionGroup& xkbGroup = m_rules->optionGroups[xkbGroupNm];

        return m_xkbConfig->options.indexOf(xkbGroup.options[row].name) == -1 ? Qt::Unchecked : Qt::Checked;
    }
    return QVariant();
}

 int XkbOptionsModel::rowCount(const QModelIndex& parent) const
    {
        if( ! parent.isValid() )
            return m_rules->optionGroups.count();
        if( ! parent.parent().isValid() )
            return m_rules->optionGroups.values()[parent.row()].options.count();
        return 0;
    }

    QModelIndex XkbOptionsModel::parent(const QModelIndex& index) const
    {
        if (!index.isValid() )
            return QModelIndex();
        if( index.internalId() < 100 )
            return QModelIndex();
        return createIndex(((index.internalId() - index.row())/100) - 1, index.column());
    }

    QModelIndex XkbOptionsModel::index(int row, int column, const QModelIndex& parent) const
    {
        if(!parent.isValid()) return createIndex(row, column);
        return createIndex(row, column, (100 * (parent.row()+1)) + row);
    }

 Qt::ItemFlags XkbOptionsModel::flags ( const QModelIndex & index ) const
    {
        if( ! index.isValid() )
            return 0;

        if( !index.parent().isValid() )
            return Qt::ItemIsEnabled;

        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }

 bool XkbOptionsModel::setData ( const QModelIndex & index, const QVariant & value, int role)
    {
        int groupRow = index.parent().row();
        if( groupRow < 0 ) return false;

        QString xkbGroupNm = m_rules->optionGroups.keys()[groupRow];
        const XkbOptionGroup& xkbGroup = m_rules->optionGroups[xkbGroupNm];
        const XkbOption& option = xkbGroup.options[index.row()];

        if( value.toInt() == Qt::Checked ) {
            if( xkbGroup.exclusive ) {
                // clear if exclusive (TODO: radiobutton)
                int idx = m_xkbConfig->options.indexOf(QRegExp(xkbGroupNm+".*"));
                if( idx >= 0 ) {
                    for(int i=0; i<xkbGroup.options.count(); i++)
                        if( xkbGroup.options[i].name == m_xkbConfig->options[idx] ) {
                            setData(createIndex(i, index.column(), (quint32)index.internalId()-index.row()+i), Qt::Unchecked, role);
                            break;
                        }
                //    m_kxkbConfig->m_options.removeAt(idx);
                //    idx = m_kxkbConfig->m_options.indexOf(QRegExp(xkbGroupNm+".*"));
                }
            }
            if( m_xkbConfig->options.indexOf(option.name) < 0 ) {
                m_xkbConfig->options.append(option.name);
            }
        }
        else {
            m_xkbConfig->options.removeAll(option.name);
        }
        emit dataChanged(index, index);
        return true;
    }

 void  XkbOptionsModel::gotoGroup(const QString& group, QTreeView* view)
    {
        int index = m_rules->optionGroups.keys().indexOf(group);
        if( index != -1 ) {
            QModelIndex modelIdx = createIndex(index,0);
            view->setExpanded(modelIdx, true);
            view->scrollTo(modelIdx, QAbstractItemView::PositionAtTop);
            view->selectionModel()->setCurrentIndex(modelIdx, QItemSelectionModel::Current);
            view->setFocus(Qt::OtherFocusReason);
        }
        else
            qDebug() << "XkbOptionsModel:can't scroll to group" << group;
    }
