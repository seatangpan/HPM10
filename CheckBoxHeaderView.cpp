#include "CheckBoxHeaderView.h"

CheckBoxHeaderView::CheckBoxHeaderView(int checkColumnIndex, Qt::Orientation orientation, QWidget *parent) : QHeaderView(orientation, parent)
{
    checkColIndex = checkColumnIndex;
    isChecked = true;
    isEnabled = true;
}

void CheckBoxHeaderView::setCheckState(bool state)
{
    isChecked = state;
}

void CheckBoxHeaderView::setEnabledStatus(bool state)
{
    isEnabled = state;
    this->updateSection(checkColIndex);
    emit signalEnabledChanged(isEnabled);
}

bool CheckBoxHeaderView::getCheckState()
{
    return isChecked;
}

void CheckBoxHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    if (logicalIndex == checkColIndex)
    {
        QStyleOptionButton option;
        QPoint topLeft(0, 5);//勾选框起始屏幕坐标
        QSize boxSize(5, 20);//勾选框大小
        option.rect = QRect(topLeft.x(), topLeft.y(), boxSize.width(), boxSize.height());
        if (isChecked)
        {
            if(isEnabled)
            {
                option.state |= (QStyle::State_On | QStyle::State_Enabled);
            }
            else
            {
                option.state |= QStyle::State_On;
            }

        }
        else
        {
            if(isEnabled)
            {
                option.state |= (QStyle::State_Off | QStyle::State_Enabled);
            }
            else
            {
                option.state |= QStyle::State_Off;
            }
        }
        //加入复选框，设置样式
        QCheckBox *check = new QCheckBox;
        QString sheet = QString("QCheckBox::indicator {width: %1px;  height: %2px;}").arg(boxSize.width()).arg(boxSize.height());
        check->setStyleSheet(sheet);
        this->style()->drawControl(QStyle::CE_CheckBox, &option, painter, check);
    }
}

void CheckBoxHeaderView::mousePressEvent(QMouseEvent *event)
{
    if (visualIndexAt(event->pos().x()) == checkColIndex)
    {
        this->updateSection(checkColIndex);
        if(isEnabled)
        {
            isChecked = !isChecked;
            emit signalCheckStateChanged(isChecked);
        }
    }
    //继承后此信号必须手动发送，否则无法响应
    emit QHeaderView::sectionClicked(visualIndexAt(event->pos().x()));
    QHeaderView::mousePressEvent(event);
}
