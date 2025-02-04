/****************************************************************************

    AFoil Class
    Copyright (C) 2009-2016 Andre Deperrois

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


/** @file
 *
 * This file implements the QAFoil class used as the interface for direct Foil design.
 *
*/

#ifndef QAFOIL_H
#define QAFOIL_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QRadioButton>
#include <QSettings>
#include <QVector>


#include "foildesignwt.h"


class FoilTableDelegate;
class MainFrame;
class Foil;
class SplineFoil;
class XFoil;


/**
 * @brief the QAFoil class used as the interface for direct Foil design
 */
class AFoil : public QWidget
{
    Q_OBJECT

    friend class MainFrame;
    friend class FoilTableDelegate;
    friend class TwoDWidget;
    friend class GridSettingsDlg;
    friend class AFoilTableDlg;
    friend class FlapDlg;
    friend class NacaFoilDlg;
    friend class InterpolateFoilsDlg;
    friend class SplineCtrlsDlg;
    friend class TwoDPanelDlg;
    friend class FoilGeomDlg;
    friend class TEGapDlg;
    friend class LEDlg;
    friend class LECircleDlg;
    friend class FoilCoordDlg;
    friend class CAddDlg;

public:
    AFoil(QWidget *parent = NULL);
    ~AFoil();

    void setupLayout();
    void initDialog(FoilDesignWt *p2DWidget, QVector<Foil *> *poaFoil, XFoil *pXFoil);

signals:
    void projectModified();

public slots:
    void onUpdateFoilTable();

private slots:
    void onAFoilCadd();
    void onAFoilDerotateFoil();
    void onAFoilFoilCoordinates();
    void onAFoilFoilGeom();
    void onAFoilInterpolateFoils();
    void onAFoilLECircle();
    void onAFoilNacaFoils();
    void onAFoilNormalizeFoil();
    void onAFoilPanels();
    void onAFoilSetFlap();
    void onAFoilSetLERadius();
    void onAFoilSetTEGap();
    void onAFoilTableColumns();
    void onDeleteCurFoil();
    void onDuplicate();
    void onExportCurFoil();
    void onExportSplinesToFile();
    void onFoilClicked(const QModelIndex& index);
    void onFoilStyle();
    void onFoilTableCtxMenu(const QPoint &);
    void onHideAllFoils();
    void onHideCurrentFoil();
    void onNewSplines();
    void onRedo();
    void onRenameFoil();
    void onResetColumnWidths();
    void onShowAllFoils();
    void onShowCurrentFoil();
    void onShowLegend();
    void onSplineControls();
    void onStoreSplines();
    void onSplinesModified();
    void onUndo();

private:
    Foil* addNewFoil(Foil *pFoil);
    void setControls();
    void clearStack(int pos=0);
    void fillFoilTable();
    void fillTableRow(int row);
    void showFoil(Foil* pFoil, bool bShow=true);
    void setAFoilParams();
    void selectFoil(Foil* pFoil = NULL);
    void paintSplines(QPainter &painter);
    void paintFoils(QPainter &painter);
    void setTableFont();

    void loadSettings(QSettings *pSettings);
    void saveSettings(QSettings *pSettings);

    void takePicture();
    void setPicture();

    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);


private:

    FoilDesignWt *m_p2DWidget;

    QTableView *m_pctrlFoilTable;
    QStandardItemModel *m_pFoilModel;
    FoilTableDelegate *m_pFoilDelegate;

    static MainFrame *s_pMainFrame;  /**< a static pointer to the instance of the application's MainFrame object >*/

    int *m_precision;           /**< the array of digit numbers for each column of the Foil table >*/


    bool m_bStored;             /**< true if the current Picture has been stored on the Undo stack >*/


    QVector<Foil*> *m_poaFoil;   /**< a pointer to the array of Foil objects >*/
    XFoil *m_pXFoil;             /**< a void pointer to the XFoil object >*/

    SplineFoil *m_pSF;          /**< a pointer to the SplineFoil object >*/

    Foil *m_pBufferFoil;        /**< a pointer to the active buffer foil >*/


    int m_StackPos;                   /**< the current position on the Undo stack >*/
    QVector<SplineFoil> m_UndoStack;    /**< the stack of incremental modifications to the SplineFoil;
                                         we can't use the QStack though, because we need to access
                                         any point in the case of multiple undo operations >*/

};

#endif // QAFOIL_H



