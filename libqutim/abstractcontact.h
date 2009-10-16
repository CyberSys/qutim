/****************************************************************************
 *  abstractcontact.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ABSTRACTCONTACT_H
#define ABSTRACTCONTACT_H

#include "libqutim_global.h"
#include <QIcon>
#include <QScopedPointer>

namespace qutim_sdk_0_3
{
	struct AbstractContactPrivate;
	class MetaContact;
	class Message;

	class AbstractContact : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(Status status READ status NOTIFY statusChanged)
		Q_PROPERTY(QIcon statusIcon READ statusIcon NOTIFY statusIconChanged)
		Q_PROPERTY(MetaContact * metaContact READ metaContact NOTIFY metaContactChanged)
	public:
		AbstractContact(const QString &id, QObject *parent = 0);
		virtual ~AbstractContact();
		QString id();
		virtual QString name() const;
		virtual Status status() const;
		virtual QIcon statusIcon() const;
		MetaContact *metaContact();
		virtual void sendMessage(const Message &message) = 0;
		virtual void setName(const QString &name) = 0;
	signals:
		void statusChanged(Status status);
		void statusIconChanged(const QIcon &statusIcon);
		void nameChanged(const QString &name);
	private:
		QScopedPointer<AbstractContactPrivate> p;
	};
}

#endif // ABSTRACTCONTACT_H
