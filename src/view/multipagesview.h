/*
 * Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     wangleiyong <wangleiyong_cm@deepin.com>
 *
 * Maintainer: wangleiyong <wangleiyong_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MULTIPAGESVIEW_H
#define MULTIPAGESVIEW_H

#include <QListView>
#include <QList>
#include <QScrollArea>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QPropertyAnimation>

#include "../widgets/applistarea.h"
#include "appgridview.h"
#include "../model/appslistmodel.h"
#include "../model/appsmanager.h"
#include "../global_util/calculate_util.h"
#include "../widgets/gradientlabel.h"
#include "../boxframe/boxframe.h"
#include "pagecontrol.h"

#include <dboxwidget.h>

DWIDGET_USE_NAMESPACE

class MultiPagesView : public QWidget, public DragPageDelegate
{
    Q_OBJECT
public:
    explicit MultiPagesView(AppsListModel::AppCategory categoryModel = AppsListModel::All, QWidget *parent = nullptr);

    void updatePageCount(AppsListModel::AppCategory category = AppsListModel::All);
    void showCurrentPage(int currentPage);
    QModelIndex selectApp(const int key);
    AppGridView *pageView(int pageIndex);
    AppsListModel *pageModel(int pageIndex);
    int currentPage(){return m_pageIndex;}
    int pageCount() { return m_pageCount;}
    QModelIndex getAppItem(int index);
    void setDataDelegate(QAbstractItemDelegate *delegate);
    void setModel(AppsListModel::AppCategory category);
    void updatePosition();

    void ShowPageView(AppsListModel::AppCategory category);

    void mousePress(QMouseEvent *e) override;
    void mouseMove(QMouseEvent *e) override;
    void mouseRelease(QMouseEvent *e) override;

signals:
    void connectViewEvent(AppGridView* pView);

private slots:
    void dragToLeft(const QModelIndex &index);
    void dragToRight(const QModelIndex &index);
    void dragStop();

protected:
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;
    void InitUI();
    void updateGradient();
    void updateGradient(QPixmap& pixmap, QPoint startPoint, QPoint topRightImg);

private:
    bool m_bDragStart;

    bool m_bMousePress;
    int m_nMousePos;
    int m_scrollValue;
    int m_scrollStart;

    GradientLabel *m_pLeftGradient;
    GradientLabel *m_pRightGradient;

    int m_pageCount;
    int m_pageIndex;
    AppsListModel::AppCategory m_category;

    AppsManager *m_appsManager;
    CalculateUtil *m_calcUtil;
    AppListArea *m_appListArea;
    AppGridViewList m_appGridViewList;
    pageAppsModelist m_pageAppsModelList;

    DHBoxWidget *m_viewBox;

    QPropertyAnimation *m_pageSwitchAnimation;

    QAbstractItemDelegate *m_delegate = nullptr;
    pageControl *m_pageControl;
};

#endif // MULTIPAGESVIEW_H
