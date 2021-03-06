/*
* Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
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
#pragma once

#include <DComboBox>
#include <QTimeEdit>
#include <DListWidget>
#include <DLineEdit>
#include <QTime>
DWIDGET_USE_NAMESPACE

class CTimeEditCtrl : public DComboBox
{
    Q_OBJECT

public:
    CTimeEditCtrl(QWidget *parent = nullptr);
    ~CTimeEditCtrl();
    void setTime(QTime time);
    QTime getTime();
private:
    void initUI();
    void initConnection();
    void showPopup();
    void hidePopup();
    void addlistvalue(QStringList vListData, DListWidget *listw, int currentvalue);
public slots:
    void slotcurrentValueChanged(int value);
    void slotcurrentValueChanged(const QString &text);
    void slotcurrentValueChangedClose(int value);
    void slotEidtChange();
private:
    DLineEdit                 *m_timeEdit;
    DListWidget               *m_pListWidget;
    int                       m_type = 0; // 0 h,1 m
    int                       m_pos = 0;
    QStringList               m_hourslist;
    QStringList               m_minlist;
    QTime                     m_time;
};
