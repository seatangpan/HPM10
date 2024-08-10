#include "TranslateLang.h"
#include <QFile>
#include <QDebug>

TranslateLang::TranslateLang():m_nCurType(0)
{

}


/**
 * @brief 加载文本
 * @param nLanType: 多语言类型，0：中文 1:英文
 * @return true 加载成功
 */
bool TranslateLang::Load(int nLanType)
{
    m_nCurType = nLanType;
    QString sResFile = "";
    switch (nLanType)
    {
    case 0:
        sResFile = "./cn_HPM10.txt";
        break;
    case 1:
        sResFile = "./en_HPM10.txt";
        break;
    default:
        sResFile = "./cn_HPM10.txt";
        break;
    }
    m_langData.clear();
    QFile file(sResFile);
    //打开一个文件有两种方式。一种方式是在构造函数中指定文件名；另一种 方式是使用setFileName()函数设置文件名。
    if(file.open(QIODevice::ReadOnly))
    //打开文件使用open()函数，关闭文件使用close()函数。此处的 open()函数以只读方式打开文件，只读方式参数为QIODevice:: ReadOnly，只写方式参数为 QIODevice::WriteOnly，读写参数为QIODevice:: ReadWrite。
    {

        char buffer[2048] = "";
        while (1)
        {
            qint64 lineLen = file.readLine(buffer,sizeof(buffer));
            //在QFile中可以使用从QIODevice中继承的readLine() 函数读取文本文件的一行。
            if(lineLen!=-1)
            //如果读取成功，则readLine()函数返回实际读取的字节数；如果 读取失败，则返回“-1”。
            {
                //qDebug()<<buffer;
                buffer[8]='\0';//ID截取8个字符
                int nID = QString(buffer).toInt(NULL, 16);
                if (buffer[lineLen - 2] == '\r')
                {
                    buffer[lineLen - 2] = '\0';
                }
                if (buffer[lineLen - 1] == '\n')
                {
                    buffer[lineLen - 1] = '\0';
                }
                QString sTxt = QString(buffer + 9);
                //qDebug()<<nID<<sTxt;
                m_langData[nID] = sTxt;
            }
            else
            {
                break;
            }
        }

    }
    else
    {
        return false;
    }
    return true;
}


/**
 * @brief 按ID读取文本
 * @param nID：对应到文本行里设置的ID
 * @return 多语言文本
 */
QString TranslateLang::GetTextByID(int nID)
{
    QString sTxt = m_langData[nID];

    return sTxt;
}
