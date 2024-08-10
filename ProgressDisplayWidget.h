#ifndef PROCESSPROGRESSDISPLAYWIDGET_H
#define PROCESSPROGRESSDISPLAYWIDGET_H

#include <QWidget>

//流程进度展示控件
class ProgressDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    ProgressDisplayWidget(QWidget *parent = nullptr);
    ~ProgressDisplayWidget()override;
    void setProcessList(const QStringList &value);
    void setCurrentStep(int value);

protected:
    void paintEvent(QPaintEvent *event)override;

private:
    QStringList processList;
    int currentStep{1};
    QColor lineColor{"#C0C4CC"};
    QColor stepsCompletedColor{"#41CD52"};//已完成步骤的颜色
    QColor stepUncompleteColor{"#333333"};//未完成步骤的颜色
    QColor currentStepColor{"#FC5531"};//当前步骤颜色
};
#endif // PROCESSPROGRESSDISPLAYWIDGET_H
