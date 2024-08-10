#include "ProgressDisplayWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

ProgressDisplayWidget::ProgressDisplayWidget(QWidget *parent)
    : QWidget(parent)
{
    setPalette(Qt::white);
}

ProgressDisplayWidget::~ProgressDisplayWidget()
{
}

void drawSmallCircle(QPainter * painter, int x, int y,int radius,const QColor & color,const QString & text)
{
    QRect boundingRect = QRect(-radius,-radius,radius * 2,radius * 2);

    painter->save();
    painter->translate(x,y);

    QLinearGradient linearGradient(boundingRect.topLeft(),boundingRect.bottomRight());
    linearGradient.setColorAt(0.0,color);
    linearGradient.setColorAt(1.0,color.lighter());

    painter->setBrush(linearGradient);
    painter->setPen(QPen(QBrush(linearGradient), 3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));//渐变线

    painter->drawEllipse(boundingRect);

    painter->setBrush(Qt::white);
    painter->drawEllipse(boundingRect.adjusted(1,1,-1,-1));

    painter->setPen(color);
    painter->drawText(boundingRect,Qt::AlignCenter,text);

    painter->restore();
}

void ProgressDisplayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    auto font = painter.font();
    font.setPixelSize(12);
    font.setBold(false);
    painter.setFont(font);

    auto& rect = event->rect();
    auto width = rect.width();
    auto height = rect.height();

    int lineY = static_cast<int>(height * 0.3) ;
    int startX = 80;
    int size = processList.size();

    int radius = 15;
    int spacerLenth = (width - 2 * startX - (radius * 2 * size)) / (size - 1);
    for(int i = 0;i < size;++i)
    {
        int x = startX + radius + (spacerLenth * i) + (radius * 2 * i);
        QColor * color;
        if(i < (size - 1))
        {
            painter.save();
            if(i < currentStep)
            {
                color = &stepsCompletedColor;
            }
            else
            {
                color = &lineColor;
            }
            painter.setPen(QPen(*color,3));
            painter.drawLine(QPoint(x + radius,lineY),QPoint(x + radius + spacerLenth,lineY));
            painter.restore();
        }

        if(i < currentStep)
        {
            color = &stepsCompletedColor;
        }
        else if(i == currentStep)
        {
            color = &currentStepColor;
        }
        else
        {
            color = &stepUncompleteColor;
        }
        drawSmallCircle(&painter,x,lineY+1,radius,*color,QString::number(i+1));

        int textRectW;
        if(i == 0 || i == (size - 1))
        {
            textRectW = 2 * (startX - 20) + radius * 2;
        }
        else
        {
            textRectW = spacerLenth + radius * 2;
        }

        int rextRectX;
        if(i == 0)
        {
            rextRectX = 20;
        }
        else if(i == (size - 1))
        {
            rextRectX = startX + i * (radius * 2) + spacerLenth * (i - 1) + (spacerLenth - (textRectW / 2) + radius);
        }
        else
        {
            rextRectX = startX + spacerLenth/2 + spacerLenth * (i - 1) + i * 2 * radius;
        }

        QRect textRect = QRect(rextRectX,
                               lineY + radius * 2,
                               textRectW,
                               height - lineY - radius * 2);

        painter.setPen(*color);
        painter.drawText(textRect,Qt::AlignTop | Qt::AlignHCenter,processList[i]);
    }
}

//设置当前步骤
void ProgressDisplayWidget::setCurrentStep(int value)
{
    currentStep = value;
    update();
}

void ProgressDisplayWidget::setProcessList(const QStringList &value)
{
    processList = value;
    currentStep = 0;
    update();
}
