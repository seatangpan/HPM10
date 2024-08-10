#ifndef CHECKBOXHEADERVIEW_H
#define CHECKBOXHEADERVIEW_H

#include <QtGui>
#include <QPainter>
#include <QHeaderView>
#include <QStyleOptionButton>
#include <QStyle>
#include <QCheckBox>
#include <QEvent>

class CheckBoxHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    CheckBoxHeaderView(int checkColumnIndex, Qt::Orientation orientation, QWidget *parent = nullptr);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int     checkColIndex;  //表头列下标
    bool    isChecked;      //勾选框状态
    bool    isEnabled;

public:

    void setCheckState(bool state);             //设置复选框状态

    void setEnabledStatus(bool state);

    bool getCheckState();

signals:

    void signalCheckStateChanged(bool state);   //勾选状态发生改变信号

    void signalEnabledChanged(bool state);   //勾选状态发生改变信号
};

#endif // CHECKBOXHEADERVIEW_H
