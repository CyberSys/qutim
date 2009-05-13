#include "plugpackagemodel.h"
#include "utils/plugversion.h"
#include <QDebug>

plugPackageModel::plugPackageModel(QObject* parent)
: QAbstractItemModel(parent), m_root_node(new plugPackageItem)
{
}

plugPackageModel::~plugPackageModel() {
	delete(m_root_node);
}

// void plugPackageModel::setRootNode(plugPackageItem* plug_package_item) {
// 	reset();
// 	delete(m_root_node);
// 	m_root_node = plug_package_item;
// }

QModelIndex plugPackageModel::index(int row, int column, const QModelIndex& parent) const {
	if (!m_root_node)
		return QModelIndex();
	return createIndex(	row,
						column,
						nodeFromIndex(parent)->getChildData(row));
}

plugPackageItem *plugPackageModel::nodeFromIndex(const QModelIndex& index) const {
	if (index.isValid())
		return static_cast<plugPackageItem *>(index.internalPointer());
	else
		return m_root_node;
}

int plugPackageModel::rowCount(const QModelIndex& parent) const {
	plugPackageItem *parentNode = nodeFromIndex(parent);
	if (!parentNode)
		return 0;
	return parentNode->childrenCount();
}

int plugPackageModel::columnCount(const QModelIndex& parent) const
{
	return 1; //from example (=
}

QModelIndex plugPackageModel::parent(const QModelIndex& child) const {
	plugPackageItem *node = nodeFromIndex(child);
	if (!node)
		return QModelIndex();
	plugPackageItem *parentNode = node->getParent();
	if (!parentNode)
		return QModelIndex();
	plugPackageItem *grandParentNode = parentNode->getParent();
	if (!grandParentNode)
		return QModelIndex();
	int row = grandParentNode->indexOf(parentNode);
	return createIndex(row, child.column(), parentNode);
}

QVariant plugPackageModel::data(const QModelIndex& index, int role) const {
	plugPackageItem *node = nodeFromIndex(index);
	if (index.column()==0)	{
		switch(role) {
			case Qt::DisplayRole:
				return node->getItemData()->packageItem.properties.value("name");
			case Qt::DecorationRole:
				return node->getItemData()->icon;
			case InstalledRole:
				return node->getItemData()->attribute;
                        case CheckedRole:
                                return node->getItemData()->checked;
			case CategoryRole:
				return m_category_nodes.contains(node->item_name);
			case SummaryRole:
				return node->getItemData()->packageItem.properties.value("shortdesc");
			default:
				return QVariant();
		}
	}
	return QVariant();
}

bool plugPackageModel::hasChildren(const QModelIndex& parent) const {
return QAbstractItemModel::hasChildren(parent);
}


QVariant plugPackageModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();
	
	if (orientation == Qt::Horizontal)
		return tr("Packages");
	else
		return QString("Row %1").arg(section);
}

void plugPackageModel::addItem(const ItemData& item, const QString& name) {
	plugPackageItem *category_node = m_category_nodes.value(item.packageItem.properties.value("type"));
	if (!category_node) {
		ItemData category_item = ItemData (group,QIcon(":/icons/hi64-action-package.png"));
		category_item.packageItem.properties.insert("name", item.packageItem.properties.value("type"));
		category_node = new plugPackageItem (category_item,
										item.packageItem.properties.value("type"));
 		m_category_nodes.insert(item.packageItem.properties.value("type"),category_node);
		beginInsertRows(QModelIndex(),m_root_node->childrenCount(),m_root_node->childrenCount());
		m_root_node->addChild(category_node, m_root_node->childrenCount());
		endInsertRows();
	}
	if (m_packages.contains(name)) {
		ItemData update_item = item;
		plugVersion currentVersion (m_packages.value(name)->getItemData()->packageItem.properties.value("version"));
		plugVersion replaceVersion (item.packageItem.properties.value("version"));
		if (replaceVersion>currentVersion) {
			if ((m_packages.value(name)->getItemData()->attribute == installed)) {
				//FIXME полностью отказаться от использования id в пользу name
				update_item.attribute = isUpgradable;
				update_item.packageItem.id = m_packages.value(name)->getItemData()->packageItem.id; //Id имеется только у установленных пакетов
			}
			m_packages.value(name)->setItem(update_item,name);
		}
	}
	else {
		plugPackageItem *node = new plugPackageItem (item, name);
		m_packages.insert(name,node);
		qDebug () << "insert item:" << name;
		beginInsertRows(createIndex(category_node->childrenCount(), 0, category_node),category_node->childrenCount(),category_node->childrenCount());
		category_node->addChild(node,category_node->childrenCount());
		endInsertRows();
	}
	return;
}

void plugPackageModel::clear() {
	reset();
	delete(m_root_node);
	m_category_nodes.clear();
        m_checked_packages.clear();
	m_packages.clear();
 	m_root_node = new plugPackageItem;
}

QHash<QString, plugPackageItem *> plugPackageModel::getCheckedPackages() {
    return m_checked_packages;
}

bool plugPackageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != CheckedRole)
        return false;
    plugPackageItem *node = nodeFromIndex(index);
    if (node->getItemData()->type==group)
        return false;
    int installedRole = node->getItemData()->attribute;
    int checkedRole = node->getItemData()->checked;
    QString name = node->getItemData()->packageItem.properties.value("type") + "/" + node->getItemData()->packageItem.properties.value("name");
    qDebug() << name;
    switch (checkedRole) {
        case unchecked:
            switch (installedRole) {
                case isInstallable:
                    node->getItemData()->checked = markedForInstall;
                    break;
                case isUpgradable:
                    node->getItemData()->checked = markedForUpgrade;
                    break;
                case isDowngradable:
                    node->getItemData()->checked = markedForDowngrade;
                    break;
            }
            m_checked_packages.insert(name,node);
            break;
        case markedForInstall:
            node->getItemData()->checked = unchecked;
            m_checked_packages.remove(name);
            break;
    }
    emit dataChanged(createIndex(0, 1), createIndex(m_category_nodes.size(), 1));
    return false;
}
 void plugPackageModel::uncheckAll() {
    qDebug() << m_checked_packages;
    QHash<QString,plugPackageItem *>::const_iterator it = m_checked_packages.begin();
    for (it = m_checked_packages.begin(); it!=m_checked_packages.end();it++) {
        it.value()->getItemData()->checked = unchecked;
    }
    m_checked_packages.clear();
 }

 void plugPackageModel::upgradeAll() {
    QHash<QString,plugPackageItem *>::const_iterator it = m_packages.begin();
    for (it = m_packages.begin(); it!=m_packages.end();it++) {
        if (it.value()->getItemData()->attribute == isUpgradable && !m_checked_packages.contains(it.key())) {
            m_checked_packages.insert(it.key(),it.value());
            it.value()->getItemData()->checked == markedForUpgrade;

        }
    }
 }
