#ifndef TRANSLATELANG_H
#define TRANSLATELANG_H

#include <QString>
#include <QMap>

#define LAN_INSTANCE  TranslateLang::GetInst()
#define GET_LAN(x)    TranslateLang::GetInst()->GetTextByID(x)

/**
 * @brief  多语言切换
 * @date   2023-12-13
 * @author tangpan
 */
class TranslateLang
{
public:
    static TranslateLang* GetInst()
    {
        static TranslateLang st_Trans;
        return &st_Trans;
    }

    /**
     * @brief 加载文本
     * @param nLanType: 多语言类型，0：中文 1:英文
     * @return true 加载成功
     */
    bool Load(int nLanType);

    /**
     * @brief 按ID读取文本
     * @param nID：对应到文本行里设置的ID
     * @return 多语言文本
     */
    QString GetTextByID(int nID);

    //获取当前语言
    int GetCurType(){return m_nCurType;}

private:
    TranslateLang();
    QMap<int, QString> m_langData;//语言文本
    int m_nCurType;
};

#endif // TRANSLATELANG_H
