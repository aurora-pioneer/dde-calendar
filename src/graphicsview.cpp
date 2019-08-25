﻿/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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
#include "graphicsview.h"
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>

#include "scheduleitem.h"
#include "schedulecoormanage.h"
#include "schceduledlg.h"
#include "dbmanager.h"
#include <QMenu>
//m_graphicsScene->setSceneRect(0,0,763,1032);

CGraphicsView::CGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_coorManage = new CScheduleCoorManage;

    m_LRPen.setColor(QColor(255, 255, 255));
    m_LRPen.setStyle(Qt::DotLine);
    m_TBPen.setColor(QColor(255, 255, 255));
    m_TBPen.setStyle(Qt::DotLine);
    m_LRFlag = true;
    m_TBFlag = true;
    m_margins = QMargins(0, 0, 0, 0);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsScene = new QGraphicsScene(this);
    m_graphicsScene->setSceneRect(0, 0, width(), height());
    setScene(m_graphicsScene);

    m_dayInterval = width();
    m_timeInterval = height() / 24.0;

    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(viewWidth / 2,  viewHeight / 2 - 1000);
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    scrollBarValueChangedSlot();

    m_editAction = new QAction(tr("Edit"), this);
    m_deleteAction = new QAction(tr("Delete"), this);
    m_createAction = new QAction(tr("Create"), this);
}

CGraphicsView::~CGraphicsView()
{
    clearSchdule();
}

void CGraphicsView::setMargins(int left, int top, int right, int bottom)
{
    m_margins = QMargins(left, top, right, bottom);
    setViewportMargins(m_margins);
}

void CGraphicsView::setRange( int w, int h, QDate begindate, QDate enddate )
{
    m_graphicsScene->setSceneRect(0, 0, w, h);
    m_coorManage->setRange(w, h, begindate, enddate);
    int totalDay = begindate.daysTo(enddate) + 1;
    m_dayInterval = w * 1.0 / totalDay;
    m_timeInterval = h / 24.0;
    m_totalDay = totalDay;
    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(viewWidth / 2,  viewHeight / 2 - 2000);
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    scrollBarValueChangedSlot();
}

void CGraphicsView::addSchduleItem( const ScheduleInfo &info )
{
    if (info.beginDateTime.date().day() != info.endDateTime.date().day()) {
        ScheduleInfo sinfo = info;
        sinfo.endDateTime = QDateTime(info.beginDateTime.date(), QTime(24, 0, 0));
        CScheduleItem *bitem = new CScheduleItem(m_coorManage, 0, m_graphicsScene);
        bitem->setData(sinfo);
        m_vScheduleItem.append(bitem);
        sinfo = info;
        sinfo.beginDateTime = QDateTime(info.endDateTime.date(), QTime(0, 0, 0));
        CScheduleItem *enditem = new CScheduleItem(m_coorManage, 0, m_graphicsScene);
        enditem->setData(sinfo);
        m_vScheduleItem.append(enditem);
    } else {
        CScheduleItem *item = new CScheduleItem(m_coorManage, 0, m_graphicsScene);
        item->setData(info);
        m_vScheduleItem.append(item);
    }
}

void CGraphicsView::deleteSchduleItem( CScheduleItem *item )
{
    int id = item->getData().id;
    for (int i = 0; i < m_vScheduleItem.size(); i++) {
        if (m_vScheduleItem[i]->getData().id == id) {
            m_vScheduleItem.remove(i);
            m_graphicsScene->removeItem(item);
            delete m_vScheduleItem[i];
            m_vScheduleItem[i] = NULL;
            i--;
        }
    }
}

void CGraphicsView::clearSchdule()
{
    for (int i = 0; i < m_vScheduleItem.size(); i++) {
        m_graphicsScene->removeItem(m_vScheduleItem.at(i));
        delete m_vScheduleItem[i];
        m_vScheduleItem[i] = NULL;
    }
    m_vScheduleItem.clear();
}

/************************************************************************
Function:       onViewEvent()
Description:    执行父类事件
Input:          event 事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::onViewEvent( QEvent *event )
{
    //防止为空
    if (NULL == event)
        return;
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::mousePressEvent(t_event);
    }
    break;
    case QEvent::MouseMove: {
        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::mouseMoveEvent(t_event);
    }
    break;
    case QEvent::MouseButtonRelease: {
        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::mouseReleaseEvent(t_event);
    }
    break;
    case QEvent::MouseButtonDblClick: {
        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::mouseDoubleClickEvent(t_event);
    }
    break;
    case QEvent::Wheel: {
        QWheelEvent *t_event = dynamic_cast<QWheelEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::wheelEvent(t_event);
    }
    break;
    case QEvent::KeyPress: {
        QKeyEvent *t_event = dynamic_cast<QKeyEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::keyPressEvent(t_event);
    }
    break;
    case QEvent::KeyRelease: {
        QKeyEvent *t_event = dynamic_cast<QKeyEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::keyReleaseEvent(t_event);
    }
    break;
    case QEvent::FocusIn: {
        QFocusEvent *t_event = dynamic_cast<QFocusEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::focusInEvent(t_event);
    }
    break;
    case QEvent::FocusOut: {
        QFocusEvent *t_event = dynamic_cast<QFocusEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::focusOutEvent(t_event);
    }
    break;
    case QEvent::Enter: {
        QGraphicsView::enterEvent(event);
    }
    break;
    case QEvent::Leave: {
        QGraphicsView::leaveEvent(event);
    }
    break;
    case QEvent::ContextMenu: {
        QContextMenuEvent *t_event = dynamic_cast<QContextMenuEvent *>(event);
        if (NULL != t_event)
            QGraphicsView::contextMenuEvent(t_event);
    }
    break;
    default:
        break;
    }
}

/************************************************************************
Function:       mousePressEvent()
Description:    鼠标按下事件
Input:          event 鼠标事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::mousePressEvent( QMouseEvent *event )
{
    if (event->button() == Qt::RightButton) {
        CScheduleItem *item = dynamic_cast<CScheduleItem *>(itemAt(event->pos()));
        if (item == NULL) {
            QMenu menu(this);
            menu.addAction(m_createAction);
            QAction *action_t = menu.exec(QCursor::pos());
            if (action_t == m_createAction) {
                QPointF senceposs = mapToScene(event->pos());
                CSchceduleDlg dlg(1, this);
                QDateTime tDatatime = m_coorManage->getDate(senceposs);
                dlg.setDate(tDatatime);
                if (dlg.exec() == DDialog::Accepted) {
                    ScheduleInfo info = dlg.getData();
                    info.id = ScheduleDbManager::addSchedule(info);
                    emit signalsUpdateShcedule(info.id);
                }
            }
        } else {
            QMenu menu(this);
            menu.addAction(m_editAction);
            menu.addAction(m_deleteAction);
            QAction *action_t = menu.exec(QCursor::pos());
            if (action_t == m_editAction) {
                CSchceduleDlg dlg(0, this);
                dlg.setData(item->getData());
                if (dlg.exec() == DDialog::Accepted) {
                    ScheduleInfo info = dlg.getData();
                    info.id = item->getData().id;
                    ScheduleDbManager::updateScheduleInfo(info);
                    emit signalsUpdateShcedule(info.id);
                }
            } else if (action_t == m_deleteAction) {
                ScheduleDbManager::deleteScheduleInfoById(item->getData().id);
                //deleteSchduleItem(item);
                //scene()->update();
                emit signalsUpdateShcedule(item->getData().id);
            }
        }
    }
}

void CGraphicsView::mouseReleaseEvent( QMouseEvent *event )
{

}


void CGraphicsView::mouseDoubleClickEvent( QMouseEvent *event )
{

}

void CGraphicsView::mouseMoveEvent( QMouseEvent *event )
{

}

#ifndef QT_NO_WHEELEVENT
/************************************************************************
Function:       wheelEvent()
Description:    鼠标滚轮事件
Input:          event 滚轮事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::wheelEvent( QWheelEvent *event )
{
    int test = event -> delta();
    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(viewWidth / 2,  viewHeight / 2 - test);
    QPointF centerpos = mapToScene(newCenter);
    centerOn(centerpos.x(), centerpos.y() + 2);

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    scrollBarValueChangedSlot();
}
#endif

/************************************************************************
Function:       keyPressEvent()
Description:    键盘按下事件
Input:          event 键盘事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::keyPressEvent( QKeyEvent *event )
{

}

/************************************************************************
Function:       keyReleaseEvent()
Description:    键盘释放事件
Input:          event 键盘事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::keyReleaseEvent( QKeyEvent *event )
{

}

/************************************************************************
Function:       focusInEvent()
Description:    焦点进入事件
Input:          event 焦点事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::focusInEvent( QFocusEvent *event )
{

}

/************************************************************************
Function:       focusOutEvent()
Description:    焦点离开事件
Input:          event 焦点事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::focusOutEvent( QFocusEvent *event )
{

}

/************************************************************************
Function:       enterEvent()
Description:    进入事件
Input:          event 事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::enterEvent( QEvent *event )
{

}

/************************************************************************
Function:       leaveEvent()
Description:    离开事件
Input:          event 事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::leaveEvent( QEvent *event )
{

}

/************************************************************************
Function:       contextMenuEvent()
Description:    右键菜单事件
Input:          event 右键菜单事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::contextMenuEvent( QContextMenuEvent *event )
{

}

/************************************************************************
Function:       resizeEvent()
Description:    窗口大小改变事件
Input:          event 窗口大小改变事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::resizeEvent( QResizeEvent *event )
{
    scrollBarValueChangedSlot();
    QGraphicsView::resizeEvent(event);
}
/************************************************************************
Function:       setLargeScale()
Description:    设置大刻度
Input:          vLRLarge 左右刻度，vTBLarge 上下刻度
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::setLargeScale(const QVector<int> &vLRLarge, const QVector<int> &vTBLarge)
{
    m_vLRLarge = vLRLarge;
    m_vTBLarge = vTBLarge;
    update();
}

/************************************************************************
Function:       paintEvent()
Description:    绘制事件
Input:          event 事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

    QPainter t_painter(viewport());
    t_painter.setCompositionMode(QPainter::CompositionMode_Difference  ); //设置混合模式
    int t_width = viewport()->width();
    int t_height = viewport()->height();
    //绘制垂直线
    if (m_TBFlag) {
        t_painter.save();
        t_painter.setPen(m_TBPen);
        for (int i = 0; i < m_vTBLarge.size(); ++i)
            t_painter.drawLine(QPoint(m_vTBLarge[i] - 1, 0), QPoint(m_vTBLarge[i] - 1, t_height));
        t_painter.restore();
        if (m_totalDay == 7) {
            t_painter.save();
            for (int i = 0; i != 7; ++i) {

                int d = checkDay(i - m_firstWeekDay);
                QColor color("#E6EEF2");
                color.setAlphaF(0.05);
                t_painter.setBrush(QBrush(color));
                t_painter.setPen(Qt::NoPen);
                if (d == 6 || d == 7) {
                    t_painter.drawRect(QRect(0 + i * m_dayInterval, 0, m_dayInterval, t_height));
                }
            }
            t_painter.restore();
        }
    }
    //绘制水平线
    if (m_LRFlag) {
        t_painter.save();
        t_painter.setPen(m_LRPen);
        for (int i = 0; i < m_vLRLarge.size(); ++i)
            t_painter.drawLine(QPoint(0, m_vLRLarge[i] - 1), QPoint(t_width, m_vLRLarge[i] - 1));
        t_painter.restore();
    }
}

void CGraphicsView::scrollBarValueChangedSlot()
{
    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    m_vLRLarge.clear();
    m_vTBLarge.clear();
    QPointF leftToprealPos = mapToScene(QPoint(0, 0));
    QPointF leftBttomrealPos = mapToScene(QPoint(0, viewHeight));
    QPointF rightToprealPos = mapToScene(QPoint(viewWidth, 0));

    for (int i = m_dayInterval; i < scene()->width(); i = i + m_dayInterval) {
        m_vTBLarge.append(i);
    }
    int beginpos = ((int)leftToprealPos.y() / m_timeInterval) * m_timeInterval;
    if (beginpos < leftToprealPos.y()) {
        beginpos = (beginpos / m_timeInterval + 1) * m_timeInterval;
    }
    QVector<int> vHours;
    for (int i = beginpos; i < leftBttomrealPos.y(); i = i + m_timeInterval) {
        QPoint point = mapFromScene(leftBttomrealPos.x(), i);
        m_vLRLarge.append(point.y());
        vHours.append(i / m_timeInterval);
    }
    emit signalsPosHours(m_vLRLarge, vHours);
    scene()->update();
}

int CGraphicsView::checkDay(int weekday)
{
    if (weekday <= 0)
        return weekday += 7;

    if (weekday > 7)
        return weekday -= 7;

    return weekday;
}

/************************************************************************
Function:       setLargeScaleInfo()
Description:    设置大刻度显示
Input:          LRFlag 水平刻度，TBFlag 垂直刻度
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::setLargeScaleFlag(const bool &LRFlag, const bool &TBFlag)
{
    m_LRFlag = LRFlag;
    m_TBFlag = TBFlag;
    scene()->update();
}

void CGraphicsView::getLargeScaleFlag(bool &LRFlag, bool &TBFlag)
{
    LRFlag = m_LRFlag;
    TBFlag = m_TBFlag;
}

void CGraphicsView::setLargeScalePen(const QPen &LRPen, const QPen &TBPen)
{
    m_LRPen = LRPen;
    m_TBPen = TBPen;
    update();
}

void CGraphicsView::setFirstWeekday(int weekday)
{
    m_firstWeekDay = weekday;
}
