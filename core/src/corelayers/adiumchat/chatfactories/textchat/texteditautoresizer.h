 /****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** No Commercial Usage
 ** This file contains pre-release code and may not be distributed.
 ** You may use this file in accordance with the terms and conditions
 ** contained in the Technology Preview License Agreement accompanying
 ** this package.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 **
 **
 **
 **
 **
 **
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#ifndef TEXTEDITAUTORESIZER_H
#define TEXTEDITAUTORESIZER_H

 #include <QtGui/qplaintextedit.h>
 #include <QtGui/qtextedit.h>
#ifdef Q_WS_MAEMO_5
 #include <QtGui/qabstractkineticscroller.h>
#endif
 #include <qutim/debug.h>

 class TextEditAutoResizer : public QObject
 {
     Q_OBJECT
 public:
     TextEditAutoResizer(QWidget *parent)
         : QObject(parent), plainTextEdit(qobject_cast<QPlainTextEdit *>(parent)),
           textEdit(qobject_cast<QTextEdit *>(parent)), edit(qobject_cast<QFrame *>(parent))
     {
         // parent must either inherit QPlainTextEdit or  QTextEdit!
         Q_ASSERT(plainTextEdit || textEdit);

         connect(parent, SIGNAL(textChanged()), this, SLOT(textEditChanged()));
         connect(parent, SIGNAL(cursorPositionChanged()), this, SLOT(textEditChanged()));
     }

 private Q_SLOTS:
     inline void textEditChanged();

 private:
     QPlainTextEdit *plainTextEdit;
     QTextEdit *textEdit;
     QFrame *edit;
 };

 void TextEditAutoResizer::textEditChanged()
 {
     QTextDocument *doc = textEdit ? textEdit->document() : plainTextEdit->document();
     QRect cursor = textEdit ? textEdit->cursorRect() : plainTextEdit->cursorRect();

     QSize s = doc->size().toSize();
     if (plainTextEdit)
         s.setHeight((s.height() + 1) * edit->fontMetrics().lineSpacing());

     const QRect fr = edit->frameRect();
     const QRect cr = edit->contentsRect();

     edit->setMinimumHeight(qMax(70, s.height() + (fr.height() - cr.height() - 1)));

     // make sure the cursor is visible in case we have a QAbstractScrollArea parent
     QPoint pos = edit->pos();
     QWidget *pw = edit->parentWidget();
     while (pw) {
#ifdef Q_WS_MAEMO_5
         if (pw->parentWidget()) {
             if (QAbstractScrollArea *area = qobject_cast<QAbstractScrollArea *>(pw->parentWidget()->parentWidget())) {
                 if (QAbstractKineticScroller * scroller
				         = area->property("kineticScroller").value<QAbstractKineticScroller *>()) {
					 scroller->ensureVisible(QPoint(edit->width(),edit->height()),0,0);
                 }
                 break;
             }
         }
#endif
         pos = pw->mapToParent(pos);
         pw = pw->parentWidget();
     }
 }


#endif
