/****************************************************************************

    RenameDlg Class
    Copyright (C) 2009 Andre Deperrois 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#include "renamedlg.h"
#include <QMessageBox>


RenameDlg::RenameDlg(QWidget *pParent) : QDialog(pParent)
{
    setWindowTitle(tr("Rename"));
    m_bEnableOverwrite = true;
    m_bExists = true;
    m_strArray.clear();
    setupLayout();
}


void RenameDlg::onButton(QAbstractButton *pButton)
{
    if (m_pButtonBox->button(QDialogButtonBox::Ok) == pButton)            onOK();
    else if (m_pButtonBox->button(QDialogButtonBox::Discard) == pButton)  reject();
    else if(m_pctrlOverwrite==pButton)                                  onOverwrite();
}


void RenameDlg::setupLayout()
{
    m_pButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Discard);
    {
        m_pctrlOverwrite = new QPushButton(tr("Overwrite"));
        m_pctrlOverwrite->setAutoDefault(false);
        m_pButtonBox->addButton(m_pctrlOverwrite, QDialogButtonBox::ActionRole);
        connect(m_pButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButton(QAbstractButton*)));
    }

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    {
        QLabel *pLabelNote = new QLabel;
        pLabelNote->setText(tr("Note : Overwrite will delete operating points and reset polars"));
        m_pctrlMessage = new QLabel("A Message here");

        m_pctrlName = new QLineEdit("");
        QLabel* NameListLabel = new QLabel(tr("Existing Names:"));
        m_pctrlNameList = new QListWidget;

        pMainLayout->setStretchFactor(m_pctrlMessage, 1);
        pMainLayout->setStretchFactor(m_pctrlName, 1);
        pMainLayout->setStretchFactor(NameListLabel, 1);
        pMainLayout->setStretchFactor(m_pctrlNameList, 5);
        pMainLayout->setStretchFactor(m_pButtonBox, 1);
        pMainLayout->setStretchFactor(pLabelNote, 1);

        pMainLayout->addWidget(m_pctrlMessage);
        pMainLayout->addWidget(m_pctrlName);

        pMainLayout->addWidget(NameListLabel);
        pMainLayout->addWidget(m_pctrlNameList);

        pMainLayout->addWidget(m_pButtonBox);
        pMainLayout->addWidget(pLabelNote);
    }

    setLayout(pMainLayout);

    connect(m_pctrlNameList, SIGNAL(currentRowChanged(int)), this, SLOT(onSelChangeList(int)));
    connect(m_pctrlNameList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(onDoubleClickList(QListWidgetItem *)));
//    connect(m_pOverwriteButton, SIGNAL(clicked()), this, SLOT(onOverwrite()));
}


void RenameDlg::initDialog(QStringList *pStrList, QString startName, QString question)
{
    m_pctrlNameList->clear();

    m_strQuestion = question;

    if(!m_bEnableOverwrite) m_pctrlOverwrite->setEnabled(false);

    if(m_strQuestion.length())
    {
        m_pctrlMessage->setText(m_strQuestion);
    }
    else
    {
        m_pctrlMessage->setText(tr("Enter a name"));
    }

    m_strName = startName;
    m_pctrlName->setText(startName);
    m_pctrlName->setFocus();
    m_pctrlName->selectAll();

    m_strArray.clear();

    if(pStrList)
    {
        for (int i=0; i<pStrList->size(); i++)
        {
            m_strArray.append(pStrList->at(i));
            m_pctrlNameList->addItem(pStrList->at(i));
        }
    }
    else
    {
        m_pctrlNameList->setEnabled(false);
        m_pctrlOverwrite->setEnabled(false);
    }
}



void RenameDlg::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            m_pButtonBox->button(QDialogButtonBox::Ok)->setFocus();
            break;
        }
        case Qt::Key_Escape:
        {
            reject();
            return;
        }
        default:
            event->ignore();
    }
}



void RenameDlg::onOverwrite()
{
    m_bExists = true;
    m_strName = m_pctrlName->text();
    done(10);
}


void RenameDlg::onOK()
{
    m_strName = m_pctrlName->text();
    if (!m_strName.length())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Must enter a name"));
        m_pctrlName->setFocus();
        return;
    }

    QString strong;

    //exists ?
    m_bExists = false;
    for (int l=0; l<m_strArray.size(); l++)
    {
        strong = m_strArray.at(l);
        if(strong == m_strName)
        {
            QString str = tr("Do you wish to overwrite ")+m_strName + " ?";
            if (QMessageBox::Yes == QMessageBox::question(window(), tr("Question"), str,
                                                          QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
            {
                m_bExists = true;
                done(10);
                return;
            }
            else return;
        }
    }

    QDialog::accept();
}



void RenameDlg::onSelChangeList(int)
{
    QListWidgetItem *pItem =  m_pctrlNameList->currentItem();

    if(pItem)
    {
        QString str = pItem->text();
        m_pctrlName->setText(str);
        m_pctrlName->selectAll();
    }
}



void RenameDlg::onDoubleClickList(QListWidgetItem *pItem)
{
//    QListWidgetItem *pItem =  m_pctrlNameList->currentItem();

    if(pItem)
    {
        QString str = pItem->text();
        m_pctrlName->setText(str);
        onOK();
    }
}












