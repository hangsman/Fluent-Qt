#ifndef GALLERYFRAMERATELABEL_H
#define GALLERYFRAMERATELABEL_H

#include <QElapsedTimer>
#include <QPointer>
#include <QWidget>

#include "components/foundation/FluentElement.h"

class QHideEvent;
class QShowEvent;
class QTimer;

namespace fluent::gallery {

/**
 * @brief Compact title-bar badge showing the Gallery content painting rate.
 * zh_CN: 标题栏内的紧凑徽标，显示 Gallery 内容区的绘制帧率。
 *
 * It counts paint batches produced by the monitored content subtree, never its
 * own repaints. The latest sampled rate is exposed through the accessible
 * description as well as the painted text.
 * zh_CN: 只统计被监视内容子树产生的绘制批次，不统计自身重绘；最新采样帧率同时写入无障碍描述。
 */
class GalleryFrameRateLabel final : public QWidget, public FluentElement {
public:
    explicit GalleryFrameRateLabel(QWidget* parent = nullptr);
    ~GalleryFrameRateLabel() override;

    void setDisplayEnabled(bool enabled);
    void setMonitoredRoot(QWidget* root);
    int currentFrameRate() const { return m_frameRate; }

    void onThemeUpdated() override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void refreshMeasurement();
    void sampleFrames();
    void resetMeasurement();
    void updateAccessibleDescription();

    QPointer<QWidget> m_monitoredRoot;
    QTimer* m_sampleTimer = nullptr;
    QElapsedTimer m_sampleClock;
    int m_framesInSample = 0;
    int m_frameRate = 0;
    bool m_displayEnabled = false;
    bool m_measuring = false;
    bool m_framePending = false;
};

} // namespace fluent::gallery

#endif // GALLERYFRAMERATELABEL_H
