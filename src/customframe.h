#ifndef CustomFrame_H
#define CustomFrame_H

#include <QFrame>

class CustomFrame : public QFrame
{
    Q_OBJECT
public:
    CustomFrame(QWidget *parent = nullptr);
    void setBColor(QColor normalC);
    void setRoundState(bool lstate, bool tstate, bool rstate, bool bstate);
    void setTextStr(QFont font, QColor tc, QString strc, int flag = Qt::AlignCenter);
    void setTextStr(QString strc);
    void setTextColor(QColor tc);
    void setTextFont(QFont font);
    void setTextAlign(int flag = Qt::AlignCenter);
    void setRadius(int radius = 8);
    void setboreder(int framew = 0);
    QString getTextStr()
    {
        return  m_text;
    }
    void setFixedSize(int w, int h);
protected:
    void paintEvent ( QPaintEvent *e);
private:
    QColor                   m_bnormalColor = "#FFFFFF";
    QColor                   m_tnormalColor = "#000000";
    QFont                    m_font;
    bool                     m_bflag = false;
    bool                     m_fixsizeflag = false;
    int                      m_textflag = Qt::AlignCenter;//对齐方式
    QString                  m_text;
    int                      m_radius = 8;
    int                      m_borderframew = 0;
    bool                     m_lstate = false;
    bool                     m_tstate = false;
    bool                     m_rstate = false;
    bool                     m_bstate = false;
};

#endif // TODYBUTTON_H
