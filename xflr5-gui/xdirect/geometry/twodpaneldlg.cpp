/****************************************************************************

    TwoDPanelDlg Class
    Copyright (C) 2008-2019 Andre Deperrois

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

#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>

#include "twodpaneldlg.h"

#include <objects/objects2d/foil.h>
#include <xfoil.h>
#include <misc/text/intedit.h>
#include <misc/text/doubleedit.h>

XFoil *TwoDPanelDlg::s_pXFoil;


TwoDPanelDlg::TwoDPanelDlg(QWidget *pParent) : QDialog(pParent)
{
    setupLayout();

    m_pParent = pParent;

    m_pBufferFoil = nullptr;
    m_bApplied    = false;
}



void TwoDPanelDlg::setupLayout()
{
    setWindowTitle(tr("Global Panel Refinement"));
    QGridLayout *pInputDataLayout = new QGridLayout;
    {
        QLabel *l1 = new QLabel(tr("Number of Panels"));
        QLabel *l2 = new QLabel(tr("Panel Bunching Parameter"));
        QLabel *l3 = new QLabel(tr("TE/LE Panel Density Ratio"));
        QLabel *l4 = new QLabel(tr("Refined area/LE Panel Density Ratio"));
        QLabel *l5 = new QLabel(tr("Top Side Refined Area x/c limits"));
        QLabel *l6 = new QLabel(tr("Bottom Side Refined Area x/c limits"));

        pInputDataLayout->addWidget(l1,1,1);
        pInputDataLayout->addWidget(l2,2,1);
        pInputDataLayout->addWidget(l3,3,1);
        pInputDataLayout->addWidget(l4,4,1);
        pInputDataLayout->addWidget(l5,5,1);
        pInputDataLayout->addWidget(l6,6,1);


        m_pctrlNPanels = new IntEdit(100, this);
        m_pctrlNPanels->setMax(IQX);

        m_pctrlCVpar  = new DoubleEdit;
        m_pctrlCTErat = new DoubleEdit;
        m_pctrlCTRrat = new DoubleEdit;
        m_pctrlXsRef1 = new DoubleEdit;
        m_pctrlXsRef2 = new DoubleEdit;
        m_pctrlXpRef1 = new DoubleEdit;
        m_pctrlXpRef2 = new DoubleEdit;

        pInputDataLayout->addWidget(m_pctrlNPanels, 1, 2);
        pInputDataLayout->addWidget(m_pctrlCVpar,   2, 2);
        pInputDataLayout->addWidget(m_pctrlCTErat,  3, 2);
        pInputDataLayout->addWidget(m_pctrlCTRrat,  4, 2);
        pInputDataLayout->addWidget(m_pctrlXsRef1,  5, 2);
        pInputDataLayout->addWidget(m_pctrlXsRef2,  5, 3);
        pInputDataLayout->addWidget(m_pctrlXpRef1,  6, 2);
        pInputDataLayout->addWidget(m_pctrlXpRef2,  6, 3);

        connect(m_pctrlNPanels, SIGNAL(editingFinished()), this, SLOT(onChanged()));
        connect(m_pctrlCVpar,   SIGNAL(editingFinished()), this, SLOT(onChanged()));
        connect(m_pctrlCTErat,  SIGNAL(editingFinished()), this, SLOT(onChanged()));
        connect(m_pctrlCTRrat,  SIGNAL(editingFinished()), this, SLOT(onChanged()));
        connect(m_pctrlXsRef1,  SIGNAL(editingFinished()), this, SLOT(onChanged()));
        connect(m_pctrlXsRef2,  SIGNAL(editingFinished()), this, SLOT(onChanged()));
        connect(m_pctrlXpRef1,  SIGNAL(editingFinished()), this, SLOT(onChanged()));
        connect(m_pctrlXpRef2,  SIGNAL(editingFinished()), this, SLOT(onChanged()));
    }

    m_pButtonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Apply | QDialogButtonBox::Discard);
    {
        connect(m_pButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButton(QAbstractButton*)));
    }

    QVBoxLayout *pmainLayout = new QVBoxLayout;
    {
        pmainLayout->addStretch(1);
        pmainLayout->addLayout(pInputDataLayout);
        pmainLayout->addStretch(1);
        pmainLayout->addWidget(m_pButtonBox);
        pmainLayout->addStretch(1);
        setLayout(pmainLayout);
    }

    setMinimumHeight(250);
}


void TwoDPanelDlg::initDialog()
{
    //memorize initial values
    //    npan   = pXFoil->n;
    npan   = m_pMemFoil->nb;
    cvpar  = s_pXFoil->cvpar;
    cterat = s_pXFoil->cterat;
    ctrrat = s_pXFoil->ctrrat;
    xsref1 = s_pXFoil->xsref1;
    xsref2 = s_pXFoil->xsref2;
    xpref1 = s_pXFoil->xpref1;
    xpref2 = s_pXFoil->xpref2;

    m_pctrlNPanels->setValue(npan);
    m_pctrlCVpar->setValue(cvpar);
    m_pctrlCTErat->setValue(cterat);
    m_pctrlCTRrat->setValue(ctrrat);
    m_pctrlXsRef1->setValue(xsref1);
    m_pctrlXsRef2->setValue(xsref2);
    m_pctrlXpRef1->setValue(xpref1);
    m_pctrlXpRef2->setValue(xpref2);
    m_pctrlNPanels->setFocus();
}



void TwoDPanelDlg::onButton(QAbstractButton *pButton)
{
    if      (m_pButtonBox->button(QDialogButtonBox::Save) == pButton)     onOK();
    else if (m_pButtonBox->button(QDialogButtonBox::Apply) == pButton)    onApply();
    else if (m_pButtonBox->button(QDialogButtonBox::Discard) == pButton)  reject();
}


void TwoDPanelDlg::keyPressEvent(QKeyEvent *event)
{
    // Prevent Return Key from closing App
    switch (event->key())
    {
        case Qt::Key_Escape:
        {
            done(0);
            return;
        }
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            if(!m_pButtonBox->hasFocus())
            {
                m_pButtonBox->setFocus();
                return;
            }
            else
            {
                onOK();
                return;
            }
            break;
        }
        default:
            event->ignore();
            break;
    }
}



void TwoDPanelDlg::onChanged()
{
    m_bApplied  = false;
    onApply();
}



void TwoDPanelDlg::onApply()
{
    if(m_bApplied) return;

    //reset everything and retry

    for (int i=0; i< m_pMemFoil->nb; i++)
    {
        s_pXFoil->xb[i+1] = m_pMemFoil->xb[i] ;
        s_pXFoil->yb[i+1] = m_pMemFoil->yb[i];
    }
    s_pXFoil->nb = m_pMemFoil->nb;

    s_pXFoil->lflap = false;
    s_pXFoil->lbflap = false;

    if(s_pXFoil->Preprocess())
    {
        s_pXFoil->CheckAngles();
    }
    else
    {
        QMessageBox::information(this, tr("Warning"), tr("Unrecognized foil format"));
        return;
    }

    readParams();

    s_pXFoil->pangen();

    if(s_pXFoil->n>IQX)
    {
        QString strange = QString(tr("The total number of panels cannot exceed %1")).arg(IQX);
        QMessageBox::information(this, tr("Warning"), strange);
        //reset everything and retry
        for (int i=0; i< m_pMemFoil->nb; i++)
        {
            s_pXFoil->x[i+1] = m_pMemFoil->xb[i] ;
            s_pXFoil->y[i+1] = m_pMemFoil->yb[i];
        }
        s_pXFoil->n = m_pMemFoil->nb;
    }
    else
    {
        for (int j=0; j< s_pXFoil->n; j++)
        {
            m_pBufferFoil->xb[j] = s_pXFoil->x[j+1];
            m_pBufferFoil->yb[j] = s_pXFoil->y[j+1];
        }
        m_pBufferFoil->nb = s_pXFoil->n;
        m_pBufferFoil->initFoil();
        m_pBufferFoil->setFlap();
    }
    m_bApplied = true;
    m_bModified = true;

    m_pParent->update();
}


void TwoDPanelDlg::onOK()
{
    if(!m_bModified)
    {
        done(0);
    }
    else
    {
        onApply();
        done(1);
    }
}


void TwoDPanelDlg::readParams()
{
    s_pXFoil->npan   = m_pctrlNPanels->value();
    s_pXFoil->cvpar  = m_pctrlCVpar->value();
    s_pXFoil->cterat = m_pctrlCTErat->value();
    s_pXFoil->ctrrat = m_pctrlCTRrat->value();
    s_pXFoil->xsref1 = m_pctrlXsRef1->value();
    s_pXFoil->xsref2 = m_pctrlXsRef2->value();
    s_pXFoil->xpref1 = m_pctrlXpRef1->value();
    s_pXFoil->xpref2 = m_pctrlXpRef2->value();
}







