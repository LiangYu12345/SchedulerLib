#ifndef LAYEREDLAYOUT_H
#define LAYEREDLAYOUT_H

#include <QLayout>

/*!
 * \brief 分层的布局器
 * \details 控件布局分为三层重叠布局（重叠布局建议参考QStackedLayout），
 * 一层是背景层（一个透明的QWidget）：通常显示地图；
 * 二层是前景层（一个透明的QWidget）：通常显示一些按钮文本等控件；
 * 三层是弹窗层：显示弹窗的指定消息，只允许同时存在一个弹窗。
 * 第一层和第二层之所以分开，是因为地图刷新较快，会引起地图同一层重叠的其他控件重绘。所以把地图和其他界面分开，优化性能问题。
 * 背景层和前景层添加控件的时候，以俄罗斯方块的的逻辑向Qt::Alignment的方向排列控件，且控件的尺寸由QSizePolicy决定.
 */
class LayeredLayout : public QLayout
{
    Q_OBJECT
public:
    LayeredLayout();
};

#endif // LAYEREDLAYOUT_H
